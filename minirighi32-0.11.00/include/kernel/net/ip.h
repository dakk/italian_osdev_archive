/*!	\file include/kernel/net/ip.h
 *	\brief IP (Internet Protocol) layer header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef KIP_H
#define KIP_H

#include <kernel/net/socket.h>
#include <net/ip.h>

// --- Prototypes ----------------------------------------------------- //

uint16_t ip_checksum(const void *buf, size_t hdr_len);
void set_host_ip_net(in_addr_t ip, in_addr_t netmask);
in_addr_t get_host_ip();
in_addr_t get_host_netmask();
in_addr_t get_host_bcast();
in_addr_t get_host_net();
bool ip_packet_filter(ip_t *packet, struct sockaddr_in *from);
void to_ip_layer(ip_t *packet);
int inet_aton(const char *cp, in_addr_t *inp);
int send_ip_packet(uint32_t ip_to, const void *data, size_t len, uint8_t ttl, uint8_t proto);
void ip_clear_queue();
int ip_recvfrom(struct sock *sk, ip_t *buf, size_t len, int flags, struct sockaddr_in *from, socklen_t *fromlen);
int ip_sendto(struct sock *sk, const void *msg, size_t len, int flags, const struct sockaddr_in *to, socklen_t tolen);

#endif
