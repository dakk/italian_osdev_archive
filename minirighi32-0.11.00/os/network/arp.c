/*!	\file network/arp.c
 *	\brief ARP (Address Resolution Protocol) layer.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-04-16 Andrea Righi:
 *			Added the ARP daemon.\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <string.h>

#include <arch/clock.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/kmalloc.h>
#include <kernel/rtl8139.h>
#include <kernel/semaphore.h>
#include <kernel/sched.h>
#include <kernel/task.h>
#include <net/network.h>
#include <kernel/net/eth.h>
#include <kernel/net/ip.h>
#include <kernel/net/arp.h>

extern eth_buf_t *eth_recv_buf;

//! Broadcast ethernet address.
static uint8_t eth_bcast[ETH_ADDR_LEN] =
{
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

//! The ARP cache maximum entries.
#define ARP_TABLE_DIM	10

//! ARP cache (here are stored all addresses resolutions IP<->MAC).
static struct
{
	//! IP address.
	in_addr_t ip;
	//! Ethernet address.
	uint8_t mac[ ETH_ADDR_LEN ];
	//! Timestamp.
	uint32_t tickstamp;
} arp_table[ ARP_TABLE_DIM ];

//! ARP cache mutex semaphore.
semaphore_t arp_mutex;

//! ARP daemon.
task_t *arp_recv_thread = NULL;

//! \brief Compare two ethernet addresses.
//! \param m1 A pointer to the first address to compare.
//! \param m2 A pointer to the second address to compare.
//! \return #TRUE if they match, otherwise return #FALSE.
bool mac_addr_cmp(uint8_t *m1, uint8_t *m2)
{
	if ( memcmp(m1, m2, ETH_ADDR_LEN)==0 )
		return(TRUE);
	else
		return(FALSE);
}

//! \brief Insert an address resolution into the ARP cache.
//! \param ip The IP address (in network format).
//! \param mac The ethernet address.
void arp_add_cache(in_addr_t ip, uint8_t *mac)
{
	uint32_t age_max=0, pos_max=0, age, ticks;
	int i;

	DOWN( &arp_mutex );

	// Check if the address is already present			//
	for(i=0; i<ARP_TABLE_DIM; i++)
	{
		if ( arp_table[i].ip == ip )
		{
			// Address already present => refresh cache	//
			memcpy(arp_table[i].mac, mac, ETH_ADDR_LEN);
			arp_table[i].tickstamp = sys_get_ticks();

			UP( &arp_mutex );
			return;
		}
	}
	// Find an empty entry in the table				//
	for(i=0; i<ARP_TABLE_DIM; i++)
	{
		if ( arp_table[i].ip == 0 )
		{
			// Store the address into the ARP table		//
			arp_table[i].ip = ip;
			memcpy(arp_table[i].mac, mac, ETH_ADDR_LEN);
			arp_table[i].tickstamp = sys_get_ticks();

			UP( &arp_mutex );
			return;
		}
		// Calculate the age of this entry			//
		ticks = sys_get_ticks();
		age = ticks - arp_table[i].tickstamp;
		if ( age >= age_max )
		{
			age_max = age;
			pos_max = i;
		}
	}
	// The ARP table is full => overwrite the oldest entry		//
	arp_table[pos_max].ip = ip;
	memcpy(arp_table[pos_max].mac, mac, ETH_ADDR_LEN);
	arp_table[pos_max].tickstamp = sys_get_ticks();

	UP( &arp_mutex );
}

//! \brief Remove an entry from the ARP cache.
//! \param ip The IP address to remove (in network format).
//! \return #TRUE if the entry has been found, #FALSE otherwise.
bool arp_remove_cache(in_addr_t ip)
{
	int i;

	DOWN(&arp_mutex);

	for(i=0; i<ARP_TABLE_DIM; i++)
		if ( arp_table[i].ip == ip )
		{
			arp_table[i].ip = 0;
			UP(&arp_mutex);
			return(TRUE);
		}

	// The entry has not been found					//
	UP(&arp_mutex);
	return(FALSE);
}

//! \brief Translate from IP address to ethernet address.
//! \param eth_addr A pointer to the ethernet address.
//! \param ip_addr The IP address to translate (in network format).
//! \return #TRUE if the translation can be performed, #FALSE otherwise.
bool arp_ip_to_eth(uint8_t *eth_addr, in_addr_t ip_addr)
{
	int i;
	timeout_t time1, time2;

	if ( get_eth_mac_addr() == NULL )
		// Ethernet device is down!				//
		return(FALSE);

	// --- Special addresses ---					//
	if( 	(ip_addr==INADDR_ANY) ||
		(ip_addr==get_host_ip()) ||
		(ip_addr==INADDR_LOOPBACK) )
	{
		// Maybe we're asking our MAC address (???)		//
		memcpy(eth_addr, get_eth_mac_addr(), ETH_ADDR_LEN);
		return(TRUE);
	}

	// Check for a broadcast request				//
	if ( (ip_addr==get_host_bcast()) || (ip_addr==INADDR_BROADCAST) )
	{
		// Broadcast in the current LAN				//
		memcpy(eth_addr, eth_bcast, ETH_ADDR_LEN);
		return(TRUE);
	}
	// --- End of special addresses ---				//

	// Search the address into the ARP cache		//
	DOWN( &arp_mutex );
	for(i=0; i<ARP_TABLE_DIM; i++)
	{
		if ( arp_table[i].ip == ip_addr)
		{
			// Resolution is found in the cache	//
			memcpy(eth_addr, arp_table[i].mac, ETH_ADDR_LEN);

			UP( &arp_mutex );
			return(TRUE);
		}
	}
	UP( &arp_mutex );

	// Initialize a timeout to ask who has the ip address		//
	set_timeout(&time1, 4000);
	set_timeout(&time2, 0);

	for(;;)
	{
		// Search the address into the ARP cache		//
 		DOWN( &arp_mutex );
		for(i=0; i<ARP_TABLE_DIM; i++)
		{
			if ( arp_table[i].ip == ip_addr)
			{
				// Resolution is found in the cache	//
				memcpy(eth_addr, arp_table[i].mac, ETH_ADDR_LEN);

				UP( &arp_mutex );
				return(TRUE);
			}
		}
		UP( &arp_mutex );

		// Resolution not found into the cache			//
		if ( is_timeout(&time1) )
		{
			// In this LAN none has this IP			//
			return(FALSE);
		}

		if ( is_timeout(&time2) )
		{
#ifdef ARP_DEBUG
			kprintf("\narp who-has %u.%u.%u.%u tell %u.%u.%u.%u"
				" (%02x:%02x:%02x:%02x:%02x:%02x)",

				IP_A(ntohl(ip_addr)),
				IP_B(ntohl(ip_addr)),
				IP_C(ntohl(ip_addr)),
				IP_D(ntohl(ip_addr)),

				IP_A(ntohl(get_host_ip())),
				IP_B(ntohl(get_host_ip())),
				IP_C(ntohl(get_host_ip())),
				IP_D(ntohl(get_host_ip())),

				get_eth_mac_addr()[0],
				get_eth_mac_addr()[1],
				get_eth_mac_addr()[2],
				get_eth_mac_addr()[3],
				get_eth_mac_addr()[4],
				get_eth_mac_addr()[5]
			);
#endif
			// Ask in broadcast who has the ip		//
			send_arp_packet(ip_addr, eth_bcast, ARP_OP_REQUEST);
			// Wait 1sec before re-sending the request		//
			set_timeout(&time2, 1000);
		}
		schedule();
	}
}

//! \brief Get an ARP packet from the ethernet layer.
//! \param packet The ARP packet.
void to_arp_layer(arp_t *packet)
{
	// Add the address resolution into the ARP cache.
	arp_add_cache(packet->arp_ip_source, packet->arp_eth_source);

	// Identify the ARP operation.
	switch( ntohs(packet->arp_op) )
	{
		case ARP_OP_REQUEST:
			// Check if we must reply our address to the sender.
			if (packet->arp_ip_dest == get_host_ip()) {
				// Send our address resolution.
				send_arp_packet(packet->arp_ip_source,
					packet->arp_eth_source, ARP_OP_REPLY);
			}
		break;

		case ARP_OP_REPLY:
			// Arp daemon handles this event...
		break;

		default:
			printk(KERN_WARNING
				"%s(): arp message %i unknown!\n",
				ntohs(packet->arp_op), __FUNCTION__);
		break;
	}
}

//! \brief Send an ARP packet to the ethernet layer.
//! \param ip_to The wanted IP destination address in network format.
//! \param eth_to The ethernet destination address.
//! \param arp_op The ARP operation.
//! \return
//!	\li The number of bytes sent in case of success;
//!	\li a negative value if an error occurs.
int send_arp_packet(in_addr_t ip_to, const uint8_t *eth_to, uint16_t arp_op)
{
	arp_t *packet;
	int tot_len;
	uint8_t *mac_addr;

	packet = kmalloc(sizeof(arp_t), GFP_KERNEL);
	if (packet==NULL)
		return(-ENOMEM);

	// Create the ARP header					//
	packet->arp_hard_type = htons(ARPHRD_ETHER);
	packet->arp_proto_type = htons(ETH_FRAME_IP);
	packet->arp_hard_size = ETH_ADDR_LEN;
	packet->arp_proto_size = sizeof(in_addr_t);
	packet->arp_op = htons(arp_op);

	// Copy the MAC address of this host				//
	if ( (mac_addr = get_eth_mac_addr()) == NULL )
		// No such device or address!				//
		return(-ENXIO);

	memcpy(packet->arp_eth_source, mac_addr, ETH_ADDR_LEN);
	// Copy the IP address of this host				//
	packet->arp_ip_source = get_host_ip();

	// Set the destination MAC address				//
	memcpy(packet->arp_eth_dest, eth_to, ETH_ADDR_LEN);
	// Set the destination IP					//
	packet->arp_ip_dest = ip_to;

	// Go to the ethernet layer...					//
	tot_len = send_eth_packet(eth_to, packet, sizeof(arp_t), htons(ETH_FRAME_ARP));

	// Free the memory of the packet				//
	kfree(packet);

	if ( tot_len < 0 )
		// Something wrong from at the ethernet layer		//
		return(tot_len);

	// Well done!							//
	return(0);
}

//! \brief Receive an ARP packet.
//! \param buf The buffer where the received packet will be copied.
//! \param len The maximum size of the buffer.
//! \return The size of the packet received in bytes.
int recv_arp_packet(arp_t *buf, size_t len)
{
	uint32_t irqflags;
	union
	{
		ethernet_t *eth;
		uint8_t *raw;
	} packet;

	for(;;)
	{
		local_irq_save(irqflags);

		if( eth_recv_buf->count == 0 )
		{
			local_irq_restore( irqflags );
			sleep_task( curr_task );
			continue;
		}

		// Look into the buffer if there is an
		// ARP packet.
		packet.raw = eth_recv_buf->packet[ eth_recv_buf->read ].data;
		if( ntohs(packet.eth->type) == ETH_FRAME_ARP ) break;

		local_irq_restore(irqflags);
		schedule();
	}

	// Get the ARP packet length.
	len = MIN( len,
		eth_recv_buf->packet[ eth_recv_buf->read ].length -
		sizeof(ethernet_t) + 1 );

	// Copy the packet into the buffer.
	memcpy( buf, packet.eth->data, len );

	// Update the read pointer and the recv buffer counter.
	eth_recv_buf->read = (eth_recv_buf->read + 1) % ETH_RECV_BUF_DIM;
	(eth_recv_buf->count)--;

	local_irq_restore( irqflags );

	// Return the length of the received packet.
	return( len );
}

//! The ARP daemon implementation.
void arp_recv_daemon( int argc, char **argv )
{
	arp_t buf;
	int len;

	for(;;) {
		len = recv_arp_packet(&buf, sizeof(arp_t));
		if (len < 0) {
			continue;
		}
		to_arp_layer(&buf);
	}
}

//! \brief Initialize the ARP manager.
int arp_init()
{
	// Initialize the ARP cache.
	init_MUTEX( &arp_mutex );
	memset( arp_table, 0, sizeof(arp_table) );

	// Create the ARP daemon.
	arp_recv_thread = create_thread( arp_recv_daemon, 0, NULL, "arp_recvd" );

	return( 0 );
}

//! \brief Close the ARP manager.
void arp_shutdown()
{
	DOWN( &arp_mutex );

	// Clear the ARP table.
	memset( arp_table, 0, sizeof(arp_table) );

	UP( &arp_mutex );

	// Kill the ARP thread.
	if( arp_recv_thread != NULL )
	{
		kill( arp_recv_thread->pid );
	}
}

//! \brief Send an ARP request.
//! \param ip_dot
//!	The wanted IP address in numbers-and-dot notation.
//!	If \p ip_dot is not a valid ip address print the arp table
//!	to stdout.
void arp_request(char *ip_dot)
{
	uint8_t eth[ETH_ADDR_LEN];
	in_addr_t ip;
	int i;

	// Translate the IP address from numbers-and-dot notation into	//
	// binary network order byte number.				//
	if ( !inet_aton(ip_dot, &ip) )
	{
		// Not a valid address => dump the ARP cache to stdout	//
		// TODO: return if the ethernet card is not initialized	//
		kprintf("\nIP address      MAC address");
		for(i=0; i<ARP_TABLE_DIM; i++)
		{
			if ( arp_table[i].ip != 0 )
			{
				kprintf("\n%u.%u.%u.%u",
					IP_A(ntohl(arp_table[i].ip)),
					IP_B(ntohl(arp_table[i].ip)),
					IP_C(ntohl(arp_table[i].ip)),
					IP_D(ntohl(arp_table[i].ip))
				);
				gotoxy(16, -1);
				kprintf("%02x:%02x:%02x:%02x:%02x:%02x",
					arp_table[i].mac[0],
					arp_table[i].mac[1],
					arp_table[i].mac[2],
					arp_table[i].mac[3],
					arp_table[i].mac[4],
					arp_table[i].mac[5]
				);
			}
		}
		kprintf("\n");
		return;
	}

	// If a valid address is passed perform the address resolution	//
	if ( arp_ip_to_eth(eth, ip) )
		kprintf("\narp reply %u.%u.%u.%u is-at %02x:%02x:%02x:%02x:%02x:%02x\n",

			IP_A(ntohl(ip)), IP_B(ntohl(ip)),
			IP_C(ntohl(ip)), IP_D(ntohl(ip)),

			eth[0], eth[1], eth[2],
			eth[3], eth[4], eth[5]
		);
	else
		kprintf("\n\rhost unreachable!\n\r");
}
