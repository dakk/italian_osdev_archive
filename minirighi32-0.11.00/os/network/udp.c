/*!	\file network/udp.c
 *	\brief UDP (User Datagram Protocol) layer.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2003-01-21 Andrea Righi:
 *			Suppressed deferencing type-punned pointer
 *			warning.
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <kernel/console.h>
#include <kernel/queue.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <net/network.h>
#include <kernel/net/ip.h>
#include <kernel/net/socket.h>
#include <kernel/net/udp.h>

//! \brief
//!	Calculate the UDP checksum (calculated with the whole
//!	packet).
//! \param buff The UDP packet.
//! \param len The UDP packet length.
//! \param src_addr The IP source address (in network format).
//! \param dest_addr The IP destination address (in network format).
//! \return The result of the checksum.
uint16_t udp_checksum(const void *buff, size_t len, in_addr_t src_addr, in_addr_t dest_addr)
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

	sum += htons(IPPROTO_UDP);
	sum += htons(length);

	// Add the carries						//
	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	// Return the one's complement of sum				//
	return ( (uint16_t)(~sum)  );
}

// Dump the UDP packet.
void dump_udp_packet(udp_t *packet, in_addr_t ip_src, in_addr_t ip_dst)
{
	uint16_t len = ntohs(packet->udp_len) - sizeof(udp_t);
	int i;

	kprintf("\nUDP packet received (%u bytes of data)"
		"\nsender  = %u.%u.%u.%u:%u"
		"\nreceiver= %u.%u.%u.%u:%u",
		len,

		IP_A(ntohl(ip_src)), IP_B(ntohl(ip_src)),
		IP_C(ntohl(ip_src)), IP_D(ntohl(ip_src)),
		ntohs(packet->udp_src),

		IP_A(ntohl(ip_dst)), IP_B(ntohl(ip_dst)),
		IP_C(ntohl(ip_dst)), IP_D(ntohl(ip_dst)),
		ntohs(packet->udp_dst)
	);
	kprintf("\nData:\n");
	for( i = 0; i < len; i++ )
		kputchar( (((uint8_t *)packet)+sizeof(udp_t))[i]  );
}

//! \brief
//!	Process an UDP packet received from the IP layer.
//! \param packet The UDP packet.
//! \param ip_src The IP source address (in network format).
//! \param ip_dst The IP destination address (in network format).
//! \return 0 if success, otherwise the error code.
int to_udp_layer(udp_t *packet, in_addr_t ip_src, in_addr_t ip_dst)
{
	uint16_t chk;

	// For UDP packets the checksum is optional; a value of 0 in
	// the checksum means the checksum is not used. A value of
	// 0xFFFF means the checksum is used and its value is 0.
	if ( packet->udp_chk != 0 )
	{
		if( packet->udp_chk == 0xffff )
		{
			packet->udp_chk = 0x0000;
		}

		// Calculate the checksum.
		chk = udp_checksum(packet, ntohs(packet->udp_len), ip_src, ip_dst);
		if( chk )
		{
			return( -EPROTO );
		}

		if ( packet->udp_chk == 0x0000 )
		{
			packet->udp_chk = 0xffff;
		}
	}
#ifdef UDP_DEBUG
	dump_udp_packet( packet, ip_src, ip_dst );
#endif
	return( 0 );
}

extern semaphore_t ip_mutex;
extern queue_t *ip_packet_queue;
extern wait_queue_t ip_wait;
extern int ip_packet_count;
extern clock_t ip_timestamp;

//! Receive an UDP packet.
int udp_recvfrom(struct sock *sk, void *buf, size_t len, int flags, struct sockaddr_in *from, socklen_t *fromlen) {
	queue_t *entry;
	ip_t *packet;
	udp_t *udp_packet;
	size_t udp_len;
	int n;
	clock_t timestamp = 0;

	DOWN( &ip_mutex );

	for( ; ; )
	{
		// Get the first UDP packet in the queue.
		if( (ip_packet_count != 0) && (timestamp != ip_timestamp) )
		{
			// Update the current timestamp;
			timestamp = ip_timestamp;
			// Look into the ip packets queue.
			queue_for_each( entry, n, ip_packet_queue )
			{
				packet = queue_get_entry( entry );
				if( packet->ip_proto == IPPROTO_UDP )
				{
					// IP packet filtering.
					if( ip_packet_filter(packet, from) )
					{
						continue;
					}

					// Compare the port number.
					udp_packet = (void *)packet + (packet->ip_hdr_len * 4);
					if( udp_packet->udp_dst == from->sin_port )
					{
						goto found_udp;
					}
				}
			}
		}

		// UDP packet not found.
		UP( &ip_mutex );

		if( (flags & MSG_DONTWAIT) == MSG_DONTWAIT )
		{
			// Nonblocking I/O.
			return( -EAGAIN );
		}

		// Blocking I/O.
		wait_event( ip_wait, (ip_packet_count != 0) && (timestamp != ip_timestamp) );

		DOWN( &ip_mutex );
	}

found_udp:
	if( (flags & MSG_PEEK) == 0 )
	{
		// Remove the UDP packet from the queue.
		queue_del_entry( &ip_packet_queue, entry );

		// Update the ip packets counter.
		ip_packet_count--;
	}

	UP( &ip_mutex );

	// Get the packet length.
	udp_len = ntohs( packet->ip_len ) - packet->ip_hdr_len * 4 - sizeof(udp_t);
	len = MIN( len, udp_len );

	// Copy the content of the packet into the buffer.
	memcpy( buf, (void *)udp_packet + sizeof(udp_t), len );

	// Update the sockaddr structure.
	from->sin_family = AF_INET;
	from->sin_addr.s_addr = packet->ip_src;
	from->sin_port = udp_packet->udp_src;

	// Update the length of the socket structure.
	*fromlen = sizeof(from->sin_family) +
		sizeof(from->sin_addr) +
		sizeof(from->sin_port);

	if( (flags & MSG_PEEK) == 0 )
	{
		// Free the packet.
		kfree( packet );
	}

	// Return the length of the received packet.
	if( (flags & MSG_TRUNC) == 0 )
	{
		// Return the bytes copied into the buffer.
		return( len );
	}
	else
	{
		// Return the real length of the UDP packet.
		return( udp_len );
	}
}

//! Send an udp packet.
int udp_sendto(struct sock *sk, const void *msg, size_t len, int flags, const struct sockaddr_in *to, socklen_t tolen) {
	udp_t *packet;
	int tot_len;

	if( to->sin_family != AF_INET )
		return( -EINVAL );

	if( to->sin_port == 0 )
		return( -EINVAL );

	packet = kmalloc(len + sizeof(udp_t), GFP_KERNEL);
	if( packet == NULL )
	{
		return( -ENOMEM );
	}

	// Copy the data into the packet.
	memcpy( packet + 1, msg, len );
	len += sizeof(udp_t);

        // Create the UDP header.
	packet->udp_src = new_local_port();
	packet->udp_dst = to->sin_port;
	packet->udp_len = htonl( len );
	packet->udp_chk = 0;

	// Calculate the checksum of the UDP message.
	packet->udp_chk = udp_checksum( packet, len, get_host_ip(), to->sin_addr.s_addr );

	// Send the IP packet.
	tot_len = send_ip_packet(to->sin_addr.s_addr, packet, len, IP_DEFAULT_TTL, IPPROTO_UDP);

	// Free the memory of the packet.
	kfree( packet );

	if ( tot_len < 0 )
	{
		// Something wrong from at the IP layer.
		return( tot_len );
	}

	return( len );
}
