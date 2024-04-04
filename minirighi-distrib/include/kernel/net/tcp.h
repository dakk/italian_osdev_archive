/*!	\file include/kernel/net/tcp.h
 *	\brief Header for the TCP (Transmition Control Protocol).
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (C) 2003-2004 Andrea Righi
 */

#ifndef KTCP_H
#define KTCP_H

#include <net/tcp.h>

// --- Prototypes ----------------------------------------------------- //

int to_tcp_layer(tcp_t *packet, size_t len, in_addr_t ip_src, in_addr_t ip_dst);
int tcp_module_init();
int tcp_module_close();

#endif
