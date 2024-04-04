/*!	\file network/tcp.c
 *	\brief TCP (Transmition Control Protocol) layer.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2003-01-21 Andrea Righi:
 *			Suppressed deferencing type-punned pointer
 *			warning.
 *	\note Copyright (&copy;) 2003 Andrea Righi
 *
 *	\todo
 *	Complete the states machine for the TCP connections.
 */

#include <const.h>
#include <string.h>

#include <arch/clock.h>
#include <kernel/console.h>
#include <kernel/speaker.h>
#include <kernel/kmalloc.h>
#include <kernel/semaphore.h>
#include <net/network.h>
#include <kernel/net/ip.h>
#include <kernel/net/tcp.h>

#define TCP_DEBUG

tcp_state_t *tcp_conn = NULL;

semaphore_t tcp_mutex;

//! \brief Calculate the TCP checksum.
//! \param buff The TCP packet.
//! \param len The size of the TCP packet.
//! \param src_addr The IP source address (in network format).
//! \param dest_addr The IP destination address (in network format).
//! \return The result of the checksum.
uint16_t tcp_checksum(const void *buff, size_t len, in_addr_t src_addr, in_addr_t dest_addr)
{
	const uint16_t *buf=buff;
	uint16_t *ip_src=(void *)&src_addr, *ip_dst=(void *)&dest_addr;
	uint32_t sum;
	size_t length=len;

	// Calculate the sum						//
	sum = 0;
	while (len > 1)
	{
		sum += *buf++;
		if (sum & 0x80000000)
			sum = (sum & 0xFFFF) + (sum >> 16);
		len -= 2;
	}

	if ( len & 1 )
		// Add the padding if the packet lenght is odd		//
		sum += *((uint8_t *)buf);

	// Add the pseudo-header					//
	sum += *(ip_src++);
	sum += *ip_src;
	sum += *(ip_dst++);
	sum += *ip_dst;
	sum += htons(IPPROTO_TCP);
	sum += htons(length);

	// Add the carries						//
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	// Return the one's complement of sum				//
	return ( (uint16_t)(~sum)  );
}

void connect_sound()
{
// Beeps when a TCP connection is opened				//
	sound(0x580);
	delay(20);

	nosound();
}

void disconnect_sound()
{
// Beeps when a TCP connection is closed				//
	sound(0x420);
	delay(20);

	nosound();
}

//! Dump the TCP packet contents on the console.
void dump_tcp_packet(tcp_t *packet, size_t len, in_addr_t ip_src, in_addr_t ip_dst)
{
	uint8_t *data_buf = ((uint8_t *)packet) + (packet->tcp_hdr_len * 4);
	int i;

	kprintf("\nTCP packet received: %u(%u) bytes"
		"\nsender  = %u.%u.%u.%u:%u"
		"\nreceiver= %u.%u.%u.%u:%u"
		"\nseq=%u, ack=%u, win=%u",
		len - (packet->tcp_hdr_len) * 4,
		len,

		IP_A(ntohl(ip_src)), IP_B(ntohl(ip_src)),
		IP_C(ntohl(ip_src)), IP_D(ntohl(ip_src)),
		ntohs(packet->tcp_src),

		IP_A(ntohl(ip_dst)), IP_B(ntohl(ip_dst)),
		IP_C(ntohl(ip_dst)), IP_D(ntohl(ip_dst)),
		ntohs(packet->tcp_dst),

		ntohl(packet->tcp_seq_num), ntohl(packet->tcp_ack_num),
		ntohs(packet->tcp_win_size)
	);

	// Dump the flags.
	kprintf("\nURG:%u ACK:%u PSH:%u RST:%u SYN:%u FIN:%u",
		packet->tcp_urg,
		packet->tcp_ack,
		packet->tcp_psh,
		packet->tcp_rst,
		packet->tcp_syn,
		packet->tcp_fin
	);

	kprintf("\nData:\n");
	for( i = 0; i < (len-packet->tcp_hdr_len * 4); i++ )
		kputchar( data_buf[ i ]  );
}

// Process a TCP packet from the IP layer.
int to_tcp_layer(tcp_t *packet, size_t len, in_addr_t ip_src, in_addr_t ip_dst)
{
	uint16_t chk;

	// Calculate the TCP checksum.
	chk = tcp_checksum( packet, len, ip_src, ip_dst );
	if( chk )
	{
		return( -EPROTO );
	}
#ifdef TCP_DEBUG
	dump_tcp_packet( packet, len, ip_src, ip_dst );
#endif

	DOWN( &tcp_mutex );

	if( (packet->tcp_syn) && (tcp_conn != NULL) )
	{
		// For now only one connection is allowed :-(		//
		kprintf("\nOnly one TCP connection is allowed!");

		// Send an RST						//
		packet->tcp_ack_num = htonl(ntohl(packet->tcp_seq_num) + len - packet->tcp_hdr_len*4 + 1);
		packet->tcp_seq_num = tcp_conn->seq_num;
		packet->tcp_hdr_len = 5;
		len = sizeof(tcp_t);
		packet->tcp_ack = 1;
		packet->tcp_rst = 1;
		packet->tcp_syn = 0;
		packet->tcp_chk = 0;
		packet->tcp_chk = tcp_checksum(packet, len, ip_dst, ip_src);
		send_ip_packet(ip_src, packet, len, 255, IPPROTO_TCP);

		goto out;
	}

	if ( (packet->tcp_syn) && !(packet->tcp_ack) && (tcp_conn==NULL) )
	{
		// SYN received!					//
		// Open a new connection using sockets			//
		tcp_conn = kmalloc(sizeof(tcp_state_t), GFP_KERNEL);
		memset(tcp_conn, 0, sizeof(tcp_state_t));

		tcp_conn->socket.ip_src = ip_src;
		tcp_conn->socket.port_src = packet->tcp_src;
		tcp_conn->socket.ip_dst = ip_dst;
		tcp_conn->socket.port_dst = packet->tcp_dst;
		tcp_conn->state = TCP_SYN_RCVD;

		// Send SYN + ACK (seq+1)				//
		// Swap source and destination ports			//
		packet->tcp_dst ^= packet->tcp_src;
		packet->tcp_src ^= packet->tcp_dst;
		packet->tcp_dst ^= packet->tcp_src;

		packet->tcp_ack_num = htonl(ntohl(packet->tcp_seq_num) + 1);
		packet->tcp_seq_num = tcp_conn->seq_num = htonl( sys_times(NULL) );
		tcp_conn->seq_num = htonl(ntohl(tcp_conn->seq_num) + 1);
		packet->tcp_ack = 1;
		packet->tcp_chk = 0;
		packet->tcp_chk = tcp_checksum(packet, len, ip_dst, ip_src);
		send_ip_packet(ip_src, packet, len, 255, IPPROTO_TCP);

		goto out;
	}
	if ( (packet->tcp_rst) && (tcp_conn!=NULL) )
	{
		// Free the TCP connection structure		//
		kfree(tcp_conn);
		tcp_conn = NULL;
		disconnect_sound();
		kprintf("\nTCP connection closed!\n");

		goto out;
	}
	if ( (packet->tcp_fin) && (tcp_conn!=NULL) )
	{
		if ( tcp_conn->state != TCP_ESTABLISHED )
			goto out;

		// FIN received!					//
		// Close the connection					//

		// Send ACK (seq+1) and close also our connection	//
		// setting the FIN flag on				//

		// Swap source and destination ports			//
		packet->tcp_dst ^= packet->tcp_src;
		packet->tcp_src ^= packet->tcp_dst;
		packet->tcp_dst ^= packet->tcp_src;

		packet->tcp_ack_num = htonl(ntohl(packet->tcp_seq_num) + len - packet->tcp_hdr_len*4 + 1);
		packet->tcp_seq_num = tcp_conn->seq_num;
		packet->tcp_hdr_len = 5;
		len = sizeof(tcp_t);
		packet->tcp_ack = 1;
		packet->tcp_chk = 0;
		packet->tcp_chk = tcp_checksum(packet, len, ip_dst, ip_src);
		send_ip_packet(ip_src, packet, len, 255, IPPROTO_TCP);

		// Go into the CLOSING state (wait for ACK)		//
		tcp_conn->state = TCP_CLOSING;

		goto out;
	}
	if ( (packet->tcp_ack) && (tcp_conn->state==TCP_SYN_RCVD) )
	{
		// Connection established!				//
		kprintf("\nTCP connection established!");
		connect_sound();
		tcp_conn->state = TCP_ESTABLISHED;

		goto out;
	}

	if ( tcp_conn != NULL )
	{
		// In the other cases ack every packet			//
		if ( (tcp_conn->state == TCP_ESTABLISHED) )
		{
			if ( (packet->tcp_ack) && ((len - packet->tcp_hdr_len*4)==0) )
			{
				// Empty packet with ack! Do not reply	//
				goto out;
			}

			// Swap source and destination ports		//
			packet->tcp_dst ^= packet->tcp_src;
			packet->tcp_src ^= packet->tcp_dst;
			packet->tcp_dst ^= packet->tcp_src;

			// Send an echo packet to acknowledge		//
			packet->tcp_ack_num = htonl(ntohl(packet->tcp_seq_num) + len - packet->tcp_hdr_len*4);
			// tcp_conn->seq_num = htonl(ntohl(tcp_conn->seq_num) + 1);
			packet->tcp_seq_num = tcp_conn->seq_num;
			tcp_conn->seq_num = htonl(ntohl(tcp_conn->seq_num) + len - packet->tcp_hdr_len*4);
			// packet->tcp_hdr_len = 5;
			// len = sizeof(tcp_t);
			packet->tcp_ack = 1;
			packet->tcp_chk = 0;
			packet->tcp_chk = tcp_checksum(packet, len, ip_dst, ip_src);
			send_ip_packet(ip_src, packet, len, 255, IPPROTO_TCP);

			goto out;
		}
		if ( (tcp_conn->state == TCP_CLOSING) )
		{
			// ACK received!				//
			// Free the TCP connection structure		//
			kfree(tcp_conn);
			tcp_conn = NULL;
			disconnect_sound();
			kprintf("\nTCP connection closed!\n");

			goto out;
		}
	}

out:
	UP( &tcp_mutex );
	return( 0 );
}

int tcp_module_init()
{
	init_MUTEX( &tcp_mutex );
	return( 0 );
}

int tcp_module_close()
{
	return( 0 );
}

