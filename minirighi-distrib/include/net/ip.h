/*!	\file include/net/ip.h
 *	\brief IP (Internet Protocol) layer header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef IP_H
#define IP_H

#include <net/socket.h>

//! IP version 4.
#define IP_V4			4
//! IP version 6.
#define IP_V6			6

//! Maximum IP frame length.
#define IP_FRAME_LEN		65535
//! Minimum IP header length.
#define IP_HEAD_MIN_LEN		20
//! Default TTL (Time To Live).
#define IP_DEFAULT_TTL		64

//! Generic IP packet type.
#define IPPROTO_IP		0
//! ICMP (Internet Control Message Protocol) packet type.
#define IPPROTO_ICMP		1
//! IGMP (Internet Group Message Protocol) packet type.
#define IPPROTO_IGMP		2
//! TCP (Transmition Control Protocol) packet type.
#define IPPROTO_TCP		6
//! UDP (User Datagram Protocol) packet type.
#define IPPROTO_UDP		17

//! Type of service :: Minimum delay.
#define IP_TOS_MIN_DELAY	0x10
//! Type of service :: Maximum throughput.
#define IP_TOS_MAX_THRU		0x08
//! Type of service :: Maximum rely.
#define IP_TOS_MAX_RELY		0x04
//! Type of service :: Minimum cost.
#define IP_TOS_MIN_COST		0x02

// Fragment flags							//
//! More Fragments.
#define IP_FLAG_MF		0x2000
//! Don't Fragment.
#define IP_FLAG_DF		0x4000
//! The CE flag.
#define IP_FLAG_CE		0x8000
//! The flag mask.
#define IP_FLAG_MASK		0x1FFF

/** \ingroup Network
 *  \defgroup NetIP IP (Internet Protocol) layer
 *  The IP (Internet Protocol) layer.
 *  @{
 */

//! Create an IP address in the binary network format
//! from the notation "a.b.c.d".
#define IP_ADDRESS(a, b, c, d)	((a) | (b) << 8 | (c) << 16 | (d) << 24)
//! Get the 1st most significant byte of a host-format IP address.
#define IP_A(ip)		((uint8_t) ((ip) >> 24))
//! Get the 2nd most significant byte of a host-format IP address.
#define IP_B(ip)		((uint8_t) ((ip) >> 16))
//! Get the 3rd most significant byte of a host-format IP address.
#define IP_C(ip)		((uint8_t) ((ip) >>  8))
//! Get the less significant byte of a host-format IP address.
#define IP_D(ip)		((uint8_t) ((ip) >>  0))

//! Loopback IP address.
#define INADDR_LOOPBACK		IP_ADDRESS(127, 0, 0, 1)
//! Null IP address.
#define INADDR_ANY		IP_ADDRESS(0, 0, 0, 0)
//! Broadcast IP address.
#define INADDR_BROADCAST	IP_ADDRESS(255, 255, 255, 255)

//! The IP packet structure.
typedef struct ip
{
#if __BYTE_ORDER__ == __LITTLE_ENDIAN__
	uint8_t ip_hdr_len:4;	//!< The header length.
	uint8_t ip_version:4;	//!< The IP version.
#else
	uint8_t ip_version:4;	//!< The IP version.
	uint8_t ip_hdr_len:4;	//!< The IP header length.
#endif
	//! Type of Service.
	uint8_t ip_tos;
	//! IP packet length (both data and header).
	uint16_t ip_len;

	//! Identification.
	uint16_t ip_id;
	//! Fragment offset.
	uint16_t ip_off;

	//! Time To Live.
	uint8_t ip_ttl;
	//! The type of the upper-level protocol.
	uint8_t ip_proto;
	//! IP header checksum.
	uint16_t ip_chk;

	//! IP source address (in network format).
	uint32_t ip_src;
	//! IP destination address (in network format).
	uint32_t ip_dst;
} __attribute__ ((packed)) ip_t;

/** @} */ // end of NetIP

#endif
