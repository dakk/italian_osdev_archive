/*!	\file include/kernel/net/eth.h
 *	\brief Ethernet driver header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef KETH_H
#define KETH_H

#include <net/eth.h>

// --- Prototypes ----------------------------------------------------- //

void to_eth_layer(ethernet_t *packet, size_t len);
int send_eth_packet(const uint8_t *to, const void *data, size_t len, uint16_t type);
bool is_eth_promisc();
void ifconfig(char *cmd);

#endif
