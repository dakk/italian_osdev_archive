/*!	\file include/kernel/net/udp.h
 *	\brief UDP (User Datagram Protocol) layer header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef KUDP_H
#define KUDP_H

#include <net/udp.h>

// --- Prototypes ----------------------------------------------------- //

int to_udp_layer(udp_t *packet, in_addr_t ip_src, in_addr_t ip_dst);
int udp_recvfrom(struct sock *sk, void *buf, size_t len, int flags, struct sockaddr_in *from, socklen_t *fromlen);
int udp_sendto(struct sock *sk, const void *msg, size_t len, int flags, const struct sockaddr_in *to, socklen_t tolen);

#endif
