/*!	\file include/net/socket.h
 *	\brief Socket header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21\n
 */

#ifndef SOCKET_H
#define SOCKET_H

//! Internet address.
struct in_addr
{
	uint32_t s_addr;
};

//! Internet address type.
typedef uint32_t in_addr_t;

//! Address family type (AF_xxx).
typedef unsigned short sa_family_t;

//! Socket structure.
struct sockaddr
{
	//! Address family (AF_xxx).
	sa_family_t	sa_family;
	//! 14 bytes of protocol address.
	char		sa_data[ 14 ];
};

//! Detailed socket structure.
struct sockaddr_in
{
	//! Address family.
	sa_family_t		sin_family;
	//! Port number.
	unsigned short int	sin_port;
	//! Internet address.
	struct in_addr		sin_addr;
	//! Pad to size of struct sockaddr.
	unsigned char __pad[ sizeof(struct sockaddr) - sizeof(sa_family_t) -
				sizeof(unsigned short int) - sizeof(struct in_addr) ];
};

//! Internet IP protocol address family.
#define AF_INET		2

//! Internet IP protocol protocol family.
#define PF_INET		AF_INET

//! Stream (connection) socket.
#define SOCK_STREAM	1
//! Datagram (connectionless) socket.
#define SOCK_DGRAM	2
//! Raw socket (network protocol access).
#define SOCK_RAW	3

// --- Flags for send and recv ---------------------------------------- //

//! Out-of-band data.
#define MSG_OOB		1
//! Read data from the queue without removing it.
#define MSG_PEEK	2
//! Don't use a gateway to send out the packet.
#define MSG_DONTROUTE	4
//! Return the real length of the packet, even when it was longer than
//! the passed buffer.
#define MSG_TRUNC	0x20
//! Nonblocking I/O.
#define MSG_DONTWAIT	0x40
//! End of record.
#define MSG_EOR		0x80
//! Wait for a full request.
#define MSG_WAITALL	0x100

#endif
