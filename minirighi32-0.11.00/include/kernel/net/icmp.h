/*!	\file include/kernel/net/icmp.h
 *	\brief ICMP (Internet Control Message Protocol) layer header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef KICMP_H
#define KICMP_H

#include <kernel/net/ip.h>
#include <net/icmp.h>

// --- Prototypes ----------------------------------------------------- //

int to_icmp_layer(ip_t *packet);

#endif
