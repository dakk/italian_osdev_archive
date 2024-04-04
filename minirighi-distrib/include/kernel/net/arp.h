/*!	\file include/kernel/net/arp.h
 *	\brief ARP (Address Resolution Protocol) layer header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef KARP_H
#define KARP_H

#include <net/arp.h>

// --- Prototypes ----------------------------------------------------- //

void to_arp_layer(arp_t *packet);
void arp_add_cache(in_addr_t ip, uint8_t *mac);
bool arp_remove_cache(in_addr_t ip);
bool arp_ip_to_eth(uint8_t *eth_addr, in_addr_t ip_addr);
int arp_ip_to_eth(uint8_t *eth_addr, uint32_t ip_addr);
int send_arp_packet(in_addr_t ip_to, const uint8_t *eth_to, uint16_t arp_op);
void arp_request(char *ip_dot);
int arp_init();
void arp_shutdown();

#endif
