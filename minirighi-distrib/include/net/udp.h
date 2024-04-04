/*!	\file include/net/udp.h
 *	\brief UDP (User Datagram Protocol) layer header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef UDP_H
#define UDP_H

#include <net/socket.h>

/** \ingroup Network
 *  \defgroup NetUDP UDP (User Datagram Protocol) layer
 *  The UDP (User Datagram Protocol) layer.
 *  @{
 */

//! The UDP packet structure.
typedef struct udp
{
	//! The source port.
	uint16_t udp_src;
	//! The destination port.
	uint16_t udp_dst;

	//! The packet length.
	uint16_t udp_len;
	//! The UDP checksum.
	uint16_t udp_chk;

} __attribute__ ((packed)) udp_t;

/** @} */ // end of NetUDP

#endif
