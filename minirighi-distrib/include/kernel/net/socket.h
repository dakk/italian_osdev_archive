/*!	\file include/kernel/net/socket.h
 *	\brief Socket header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21 Andrea Righi\n
 */

#ifndef KSOCKET_H
#define KSOCKET_H

#include <net/socket.h>

// --- INTERNAL STRUCTURES -------------------------------------------- //

//! Socket internal structure.
struct sock
{
	uint32_t count;
	int domain;
	int type;
	int protocol;

	uint16_t port;
	struct in_addr ipaddr;

	int state;
};

// --- Prototypes ----------------------------------------------------- //

int sys_recvfrom( int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen );
int sys_sendto( int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen );
int sys_bind( int sockfd, struct sockaddr *my_addr, socklen_t addrlen );
int sys_connect( int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen );
int sys_listen( int s, int backlog );
int sys_accept( int s, struct sockaddr *addr, socklen_t *addrlen );
int sys_recv( int s, void *buf, size_t len, int flags );
int sys_send( int s, const void *msg, size_t len, int flags );
int sys_socket( int domain, int type, int protocol );
int sys_closesocket( int sockfd );

uint16_t new_local_port();

#endif
