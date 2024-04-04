/*!	\file network/icmp.c
 *	\brief ICMP (Internet Control Message Protocol) layer.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-09
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <arch/clock.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/kmalloc.h>
#include <kernel/queue.h>
#include <kernel/rtl8139.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <net/network.h>
#include <kernel/net/ip.h>
#include <kernel/net/socket.h>

#include <kernel/net/icmp.h>

static void icmp_fast_echo_reply( icmp_t *packet, int size, in_addr_t ip_dest )
{
	packet->icmp_type = ICMP_ECHOREPLY;
	packet->icmp_chk = 0;

	// Calculate the checksum of the ICMP message.
	packet->icmp_chk = ip_checksum( packet, size );

	// Send the IP packet.
	(void)send_ip_packet(ip_dest, packet, size, IP_DEFAULT_TTL, IPPROTO_ICMP);
}

//! \brief Process an ICMP packet received from the IP layer.
//! \param packet The ICMP packet received.
//! \return 0 if success, otherwise the error code.
int to_icmp_layer( ip_t *packet )
{
	int size = ntohs(packet->ip_len) - (packet->ip_hdr_len) * 4;
	icmp_t *icmp_packet = (icmp_t *)((void *)packet + (packet->ip_hdr_len) * 4);

	// Calculate the icmp header checksum.
	if( ip_checksum(icmp_packet, size) )
	{
		return( -EPROTO );
	}

	// Identify the message type.
	switch( icmp_packet->icmp_type )
	{
		case ICMP_ECHO:
			// Check if we have to send the echo reply.
			if( (packet->ip_dst == get_host_ip()) ||
				(packet->ip_dst == get_host_bcast()) ||
				(packet->ip_dst == INADDR_BROADCAST) ||
				(packet->ip_dst == INADDR_LOOPBACK) )
			{

				icmp_fast_echo_reply(icmp_packet, size, packet->ip_src);
				// Do not store the packet into the ip packet queue!
				// It has been already handled by the system...
				return -1;
			}
		break;
#ifdef ICMP_DEBUG
		case ICMP_ECHOREPLY:
			kprintf( "\necho reply %u.%u.%u.%u > %u.%u.%u.%u id=%u seq=%u ttl=%u\n",

				IP_A(ntohl(packet->ip_src)), IP_B(ntohl(packet->ip_src)),
				IP_C(ntohl(packet->ip_src)), IP_D(ntohl(packet->ip_src)),

				IP_A(ntohl(packet->ip_dst)), IP_B(ntohl(packet->ip_dst)),
				IP_C(ntohl(packet->ip_dst)), IP_D(ntohl(packet->ip_dst)),

				((icmp_ping_t *)icmp_packet)->ping_id,
				((icmp_ping_t *)icmp_packet)->ping_seq,
				packet->ip_ttl );
		break;

		default:
			kprintf( "\nicmp: ??? %u.%u.%u.%u > %u.%u.%u.%u id=%u seq=%u ttl=%u\n",

				IP_A(ntohl(packet->ip_src)), IP_B(ntohl(packet->ip_src)),
				IP_C(ntohl(packet->ip_src)), IP_D(ntohl(packet->ip_src)),

				IP_A(ntohl(packet->ip_dst)), IP_B(ntohl(packet->ip_dst)),
				IP_C(ntohl(packet->ip_dst)), IP_D(ntohl(packet->ip_dst)),

				((icmp_ping_t *)icmp_packet)->ping_id,
				((icmp_ping_t *)icmp_packet)->ping_seq,
				packet->ip_ttl );
		break;
#endif
	}
	return( 0 );
}
