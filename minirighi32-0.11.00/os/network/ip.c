/*!	\file network/ip.c
 *	\brief IP (Internet Protocol) layer.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-09
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <arch/clock.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/kmalloc.h>
#include <kernel/queue.h>
#include <kernel/sched.h>
#include <net/network.h>
#include <kernel/net/arp.h>
#include <kernel/net/eth.h>
#include <kernel/net/icmp.h>
#include <kernel/net/socket.h>
#include <kernel/net/tcp.h>
#include <kernel/net/udp.h>

#include <kernel/net/ip.h>

//! Maximum number of ip packets that can be stored in the ip queue.
#define MAX_IP_PACKETS		64

//! A queue to store ip packets received.
DECLARE_QUEUE( ip_packet_queue );

//! Current number of packets in the ip queue.
int ip_packet_count = 0;

//! A timestamp to see if there are new packets in the queue.
clock_t ip_timestamp = 0;

//! A wait queue for the threads that are waiting for ip packets.
DECLARE_WAITQUEUE( ip_wait );

//! A semaphore for the ip packet queue mutual exclusion.
DECLARE_MUTEX( ip_mutex );

//! Store a packet into the receive buffer.
void push_ip_packet( ip_t *packet )
{
	uint8_t *buf;

	DOWN( &ip_mutex );

	buf = kmalloc( packet->ip_len, GFP_KERNEL );
	if( buf == NULL )
	{
		printk( KERN_ERR
			"%s(): out of kernel memory!\n",
			__FUNCTION__ );
		UP( &ip_mutex );
		return;
	}

	// Store the packet into the buffer.
	memcpy( buf, packet, packet->ip_len );

	// Update the packet counter.
	if( ip_packet_count < MAX_IP_PACKETS )
	{
		// Increment the packets counter.
		ip_packet_count++;
	}
	else
	{
		ip_t *tmp;

		// Delete the head of the queue.
		tmp = queue_get_entry( ip_packet_queue );
		queue_del_head( &ip_packet_queue );
		kfree( tmp );
	}

	// Insert the packet into the queue.
	add_queue( &ip_packet_queue, buf );

	// Update the ip timestamp.
	ip_timestamp = MAX( sys_get_ticks(), ip_timestamp + 1 );

	UP( &ip_mutex );

	wakeup_queue( ip_wait );
}

//! Remove a packet from the receive buffer.
void pop_ip_packet( ip_t *buf )
{
	DOWN( &ip_mutex );

	if( rem_queue(&ip_packet_queue, buf) == 0 )
	{
		// Free the packet buffer.
		kfree( buf );
		ip_packet_count--;
	}

	UP( &ip_mutex );
}

//! Empty the ip packet queue and free the memory space.
void ip_clear_queue()
{
	queue_t *entry, *dummy;
	ip_t *packet;
	int n;

	DOWN( &ip_mutex );

	// Clear the ip packet queue.
	queue_for_each_safe( entry, dummy, n, ip_packet_queue )
	{
		packet = queue_get_entry( entry );
		queue_del_entry( &ip_packet_queue, entry );
		kfree( packet );
	}

	// Zero the packet counter.
	ip_packet_count = 0;

	UP( &ip_mutex );
}

//! \brief Calculate the IP header checksum.
//! \param buf The IP header content.
//! \param hdr_len The IP header length.
//! \return The result of the checksum.
uint16_t ip_checksum(const void *buf, size_t hdr_len)
{
	unsigned long sum = 0;
	const uint16_t *ip1;

	ip1 = buf;
	while (hdr_len > 1)
	{
		sum += *ip1++;
		if (sum & 0x80000000)
			sum = (sum & 0xFFFF) + (sum >> 16);
		hdr_len -= 2;
	}

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return(~sum);
}

//! \brief Dump an IP packet to the standard output.
//! \param packet The IP packet.
void dump_ip_packet(ip_t *packet)
{
	int i;
	uint8_t *data = ((void *)packet + (packet->ip_hdr_len * sizeof(uint32_t)));

	// Dump the IP header						//
	kprintf("\n(ip header)"
		"\nhdr_len:%u dwords, ip_version:%u, ToS:%#04X, tot_len:%u"
		"\nid:%#010x, off:%#010x, TTL:%u, proto:%#04X, ip_chk:%#06X"
		"\nip_src:%u.%u.%u.%u, ip_dst:%u.%u.%u.%u",
		packet->ip_hdr_len, packet->ip_version, packet->ip_tos,
		ntohs(packet->ip_len), ntohs(packet->ip_id), ntohs(packet->ip_off),
		packet->ip_ttl, packet->ip_proto,
		ntohs(packet->ip_chk),

		IP_A(ntohl(packet->ip_src)), IP_B(ntohl(packet->ip_src)),
		IP_C(ntohl(packet->ip_src)), IP_D(ntohl(packet->ip_src)),

		IP_A(ntohl(packet->ip_dst)), IP_B(ntohl(packet->ip_dst)),
		IP_C(ntohl(packet->ip_dst)), IP_D(ntohl(packet->ip_dst))
	);
	// Dump the IP data section					//
	kprintf("\n(ip data)\n");
	for (i=0; i<(ntohs(packet->ip_len) - packet->ip_hdr_len * sizeof(uint32_t)); i++)
		if ( isprint(data[i]) )
			kputchar(data[i]);
		else
			kputchar('.');
}

//! This is the IP address of this host (expressed in network format).
static in_addr_t host_ip = 0;

//! This is the IP netmask of this host (expressed in network format).
static in_addr_t host_netmask = 0;

//! \brief Set the IP and netmask for this host.
//! \param ip The IP address in network format.
//! \param netmask The netmask in network format.
void set_host_ip_net(in_addr_t ip, in_addr_t netmask)
{
	host_ip = ip;
	host_netmask = netmask;
}

//! \brief Get the IP address of this host.
//! \return
//!	The IP address expressed in network format.
in_addr_t get_host_ip()
{
	return( host_ip );
}

//! \brief Get the netmask of this host.
//! \return
//!	The netmask expressed in network format.
in_addr_t get_host_netmask()
{
	return( host_netmask );
}

//! \brief Get the network address of this host.
//! \return
//!	The network expressed in network format.
in_addr_t get_host_net()
{
	return( host_ip & host_netmask );
}

//! \brief Get the broadcast address of the local network.
//! \return
//!	The broadcast address of this local network expressed
//!	in network format.
in_addr_t get_host_bcast()
{
	return( (~host_netmask) | (host_ip & host_netmask) );
}

//! \brief
//!	Converts an internet IP addres from number-and-dot string
//!	notation into binary data network byte order.
//! \param cp
//!	The string of the number-and-dot notation IP address.
//! \param inp
//!	The destination where the binary network byte order address
//!	will be put.
//! \return Nonzero if the address is valid, zero if not.
int inet_aton(const char *cp, in_addr_t *inp)
{
	static const in_addr_t max[4] = { 0xFFFFFFFF, 0xFFFFFF, 0xFFFF, 0xFF };
	in_addr_t val;
	char c;
	union iaddr {
	  uint8_t bytes[4];
	  uint32_t word;
	} res;
	uint8_t *pp = res.bytes;
	int digit,base;

	res.word = 0;

	c = *cp;
	for (;;) {
		// Collect number up to '.'				//
		// Values are specified as for C:			//
		// 0x=hex, 0=octal, isdigit=decimal.			//
		if (!isdigit(c)) goto ret_0;
		val = 0; base = 10; digit = 0;
		for (;;) {
			if (isdigit(c)) {
				val = (val * base) + (c - '0');
				c = *++cp;
				digit = 1;
			} else {
				break;
			}
		}
		if (c == '.') {
			// Internet format:				//
			//	a.b.c.d					//
			//	a.b.c	(with c treated as 16 bits)	//
			//	a.b	(with b treated as 24 bits)	//
			if (pp > res.bytes + 2 || val > 0xff) {
				goto ret_0;
			}
			*pp++ = val;
			c = *++cp;
		} else
			break;
	}

	// Check for trailing characters				//
	if (c != '\0' && (!isascii(c) || !isspace(c))) {
		goto ret_0;
	}

	// Did we get a valid digit?					//
	if (!digit) {
		goto ret_0;
	}

	// Check whether the last part is in its limits depending on	//
	// the number of parts in total.				//
	if (val > max[pp - res.bytes]) {
		goto ret_0;
	}

	if (inp != NULL) {
		*inp = res.word | htonl (val);
	}

	return (1);
ret_0:
	return (0);
}

//! Check if an ip packet must be filtered in association with
//! a socket (filtering policy).
//! \return
//!	\li FALSE the packet is accepted;
//!	\li TRUE the packet is filtered.
bool ip_packet_filter( ip_t *packet, struct sockaddr_in *from ) {
	if (from->sin_family == AF_INET) {
		if (from->sin_addr.s_addr == packet->ip_src) {
			// Exact ip match => packet accepted.
			return FALSE;
		}
		if ((from->sin_addr.s_addr == INADDR_ANY) || (from->sin_addr.s_addr == INADDR_BROADCAST)) {
			// The packet is ever accepted.
			return FALSE;
		}
		if ((from->sin_addr.s_addr == INADDR_LOOPBACK) && (packet->ip_src == get_host_ip())) {
			// Accept packets from the loopback address.
			return FALSE;
		}
		if ((from->sin_addr.s_addr == get_host_ip()) && (packet->ip_src == INADDR_LOOPBACK)) {
			// Accept packets from the loopback address.
			return FALSE;
		}
		if (from->sin_addr.s_addr == get_host_bcast()) {
			// Accept only the broadcast packet for the current network.
			if (((packet->ip_dst) & host_netmask) == get_host_net())  {
				return FALSE;
			}
		}
	}
	// The packet must be filtered.
	return TRUE;
}

//! \brief Process an IP packet received from the ethernet layer.
//! \param packet The IP packet.
void to_ip_layer(ip_t *packet)
{
	// Calculate the header checksum.
	if (ip_checksum(packet, packet->ip_hdr_len * sizeof(uint32_t))) {
		printk(KERN_WARNING
			"%s(): ip header checksum error!!!\n",
			__FUNCTION__);
		return;
	}

	// Identify the right protocol.
	switch( packet->ip_proto )
	{
		case IPPROTO_ICMP:
			// ICMP (Internet Control Message Protocol).
			if (to_icmp_layer( packet ) == 0) {
				push_ip_packet(packet);
			}
		break;

		case IPPROTO_IGMP:
			// Internet Group Message Protocol).
			printk(KERN_WARNING
				"%s(): igmp protocol not yet implemented!\n",
				__FUNCTION__);
			push_ip_packet( packet );
		break;

		case IPPROTO_TCP:
			// TCP (Transmition Control Protocol).
			if( to_tcp_layer(
				((void *)packet + (packet->ip_hdr_len * 4)),
				(ntohs(packet->ip_len) - (packet->ip_hdr_len * 4)),
				packet->ip_src,
				packet->ip_dst) == 0 )
			{
				// This is a valid TCP packet.
				push_ip_packet( packet );
			}
		break;

		case IPPROTO_UDP:
			// UDP (User Datagram Protocol).
			if( to_udp_layer(
				((void *)packet + (packet->ip_hdr_len * 4)),
				packet->ip_src,
				packet->ip_dst) == 0 )
			{
				// This is a valid UDP packet.
				push_ip_packet( packet );
			}
		break;

		default:
			// Unknown protocol: store it into the buffer.
			push_ip_packet( packet );
		break;
	}
}

//! \brief Send an IP packet to the ethernet layer.
//! \param ip_to The IP destination address in network format.
//! \param data The buffer of data to be sent.
//! \param len The size of the buffer to be sent.
//! \param ttl TTL (Time To Live).
//! \param proto The upper-layer protocol type.
//! \return
//!	\li The number of bytes sent in case of success;
//!	\li -#EMSGSIZE the packet is too big to be sent;
//!	\li -#ENOMEM cannot allocate the packet structure;
//!	\li -#ENETUNREACH destination address not found;
int send_ip_packet( uint32_t ip_to, const void *data, size_t len, uint8_t ttl, uint8_t proto )
{
	uint8_t eth_to[ ETH_ADDR_LEN ];
	size_t packet_len = len + sizeof(ip_t);
	ip_t *packet;
	int tot_len;

	// Check if the network is down.
	if (get_host_ip() == 0) {
		return( -ENETDOWN );
	}

	if( packet_len > IP_FRAME_LEN )
	{
		return( -EMSGSIZE );
	}

	packet = kmalloc(packet_len, GFP_KERNEL);
	if( packet == NULL )
	{
		return( -ENOMEM );
	}

	// Translate INADDR_ANY into 127.0.0.1.
	if( ip_to == INADDR_ANY )
	{
		ip_to = INADDR_LOOPBACK;
	}

	// Create the IP header						//
	packet->ip_version = IP_V4;
	packet->ip_hdr_len = sizeof(ip_t) / sizeof(uint32_t);
	packet->ip_tos = 0;
	packet->ip_len = htons(packet_len);
	// TODO: better session id generation! :)
	packet->ip_id = htons( 0xdead );
	packet->ip_off = htons(IP_FLAG_DF | 0);
	packet->ip_ttl = ttl;
	packet->ip_proto = proto;
	packet->ip_chk = 0;
	packet->ip_dst = ip_to;
	packet->ip_src = get_host_ip();
	packet->ip_chk = ip_checksum( packet, sizeof(ip_t) );

	// Copy the data into the packet				//
	memcpy(packet + 1, data, len);

	// Translate the IP address into ethernet address using
	// the ARP protocol.
	if (!arp_ip_to_eth(eth_to, ip_to)) {
		// The ethernet address was not found!
		kfree( packet );
		return( -ENETUNREACH );
	}

	// Handle also special addresses.
	if( (ip_to == INADDR_LOOPBACK) || (ip_to == get_host_ip()) ) {
		// Perform the loopback operation at network layer.
		to_ip_layer( packet );
		tot_len = packet_len;
	} else if ((ip_to == INADDR_BROADCAST) || (ip_to == get_host_bcast())) {
		// We assume the loopback operation has gone correctly.
		tot_len = send_eth_packet(eth_to, packet, packet_len, htons(ETH_FRAME_IP));
		// Send it to the ethernet layer and also to the host
		// netowrk layer.
		to_ip_layer( packet );
	} else {
		// Go to the ethernet layer...					//
		tot_len = send_eth_packet(eth_to, packet, packet_len, htons(ETH_FRAME_IP));
	}

	// Free the memory of the packet				//
	kfree( packet );

	if ( tot_len < 0 )
	{
		// Something goes wrong at the ethernet layer		//
		return( tot_len );
	}

	// Well done!							//
	return( packet_len );
}

//! \brief Send a raw IP packet.
/**
 * This is a standard interface to use sockets also for raw ip packets.
 */
int ip_sendto(struct sock *sk, const void *msg, size_t len, int flags, const struct sockaddr_in *to, socklen_t tolen)
{
	int __ret;

	if (to->sin_family != AF_INET)
		return -EINVAL;

	// Send the raw IP packet. The port of the socket is used to
	// specify the upper level protocol (over ip).
	// This is not a standard, it is only available in minirighi...
	__ret = send_ip_packet(to->sin_addr.s_addr, msg, len, IP_DEFAULT_TTL, sk->protocol);

	return __ret;
}

//! \brief Receive an IP packet.
//! \param buf The buffer where the received packet will be copied.
//! \param len The maximum size of the buffer.
//! \return The size of the packet received in bytes.
int ip_recvfrom(struct sock *sk, ip_t *buf, size_t len, int flags, struct sockaddr_in *from, socklen_t *fromlen) {
	ip_t *packet;
	queue_t *entry;
	int n;
	clock_t timestamp = 0;
	size_t ip_len;

	// Check if the network is down.
	if (get_host_ip() == 0) {
		return( -ENETDOWN );
	}

	DOWN( &ip_mutex );

	for( ; ; )
	{
		// Get a UDP packet from the queue.
		if( (ip_packet_count != 0) && (timestamp != ip_timestamp) )
		{
			// Update the current timestamp;
			timestamp = ip_timestamp;
			// Look into the ip packets queue.
			queue_for_each( entry, n, ip_packet_queue )
			{
				packet = queue_get_entry( entry );
				if( packet->ip_proto == sk->protocol )
				{
					// IP packet filtering.
					if( ip_packet_filter(packet, from) )
					{
						continue;
					}

					goto found_ip;
				}
			}
		}

		// IP packet not found.
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

found_ip:
	if( (flags & MSG_PEEK) == 0 )
	{
		// Remove the IP packet from the queue.
		queue_del_entry( &ip_packet_queue, entry );

		// Update the ip packets counter.
		ip_packet_count--;
	}

	UP( &ip_mutex );

	// Get the packet length.
	ip_len = ntohs(packet->ip_len) - (packet->ip_hdr_len * sizeof(uint32_t));
	len = MIN( len, ip_len );

	// Copy the content of the packet into the buffer.
	memcpy(buf, (void *)((size_t)packet + packet->ip_hdr_len * sizeof(uint32_t)) , len);

	// Update the sockaddr structure.
	from->sin_family = AF_INET;
	from->sin_port = packet->ip_proto;
	from->sin_addr.s_addr = packet->ip_src;

	// Update the length of the socket structure.
	*fromlen = sizeof(from->sin_family) +
		sizeof(from->sin_port) + sizeof(from->sin_addr);

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
		// Return the real length of the IP packet.
		return( ip_len );
	}
}
