/*!	\file network/eth.c
 *	\brief Ethernet driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-04-16 Andrea Righi:
 *			Fixed a possible deadlock in the
 *			ehternet receive buffer accesses.\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <arch/clock.h>
#include <arch/i386.h>
#include <arch/mem.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/kmalloc.h>
#include <kernel/rtl8139.h>
#include <kernel/sched.h>
#include <net/network.h>
#include <kernel/net/arp.h>
#include <kernel/net/ip.h>
#include <kernel/net/tcp.h>

#include <kernel/net/eth.h>

extern rtl8139_t *rtl;
extern task_t *arp_recv_thread;

//! Ethernet receive buffer.
eth_buf_t *eth_recv_buf = NULL;

//! Ethernet daemon.
task_t *eth_recv_thread = NULL;

// --- Initialization ------------------------------------------------- //

//! \brief Initialize the ethernet receive buffer.
int eth_init_buffer()
{
	// Create the receive buffer					//
	eth_recv_buf = kmalloc(sizeof(eth_buf_t), GFP_KERNEL);
	if( eth_recv_buf == NULL )
		return( -ENOMEM );

	// Reset the receive buffer					//
	memset(eth_recv_buf, 0, sizeof(eth_buf_t));
	return( 0 );
}

// --- RECV routines -------------------------------------------------- //

//! \brief Receive data from the ethernet layer.
//! \param buf
//!	A buffer where the ethernet received packet will be copied.
//! \param len
//!	The max size of the buffer \p buf.
//! \param flags
//!	Flags.
//! \return The length in bytes of the packet received.
/**
 *	\todo \p flags parameter implementation.
 */
int recv_eth_packet(ethernet_t *buf, size_t len, int flags)
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
			local_irq_restore(irqflags);
			sleep_task( curr_task );
			continue;
		}

		// Look into the buffer if there is an
		// IP packet.
		packet.raw = eth_recv_buf->packet[ eth_recv_buf->read ].data;
		if(ntohs(packet.eth->type) == ETH_FRAME_IP) break;

		local_irq_restore(irqflags);
		schedule();
	}

	// Get the packet length.
	len = MIN( len,
		eth_recv_buf->packet[ eth_recv_buf->read ].length );

	// Get the packet from the recv buffer and put it into the
	// requested pointer.
	memcpy( buf, packet.eth, len );

	// Update the read pointer and the recv buffer counter.
	eth_recv_buf->read = (eth_recv_buf->read + 1) % ETH_RECV_BUF_DIM;
	(eth_recv_buf->count)--;

	local_irq_restore( irqflags );

	// Return the length of the received packet.
	return( len );
}

//! \brief
//! 	This daemon permanently reads packets from the recv buffer and
//!	send them to the opportune layer.
void eth_recv_daemon( int argc, char **argv )
{
	union
	{
		uint8_t raw[ ETH_FRAME_LEN ];
		ethernet_t eth;
	} packet;
	int len;

	for(;;)
	{
		// Get an ethernet packet.
		len = recv_eth_packet( &packet.eth, sizeof(packet.raw), 0 );
		if( len < 0 )
		{
			continue;
		}
		to_ip_layer( (ip_t *)(packet.eth.data) );
	}
}

static __INLINE__ void dump_eth_packet(ethernet_t *packet, size_t len) {
	kprintf("MAC DEST   = %02x:%02x:%02x:%02x:%02x:%02x\n",
		packet->dst[0], packet->dst[1], packet->dst[2],
		packet->dst[3], packet->dst[4], packet->dst[5]);
	kprintf("MAC SOURCE = %02x:%02x:%02x:%02x:%02x:%02x\n",
		packet->src[0], packet->src[1], packet->src[2],
		packet->src[3], packet->src[4], packet->src[5]);
	kprintf("PACK TYPE  = %04x\n", ntohs(packet->type));
	kprintf("TOT LEN = %u bytes\n", len);
}

static __INLINE__ void store_eth_packet(ethernet_t *packet, size_t len) {
	// Store the packet data into the send buffer.
	memcpy( eth_recv_buf->packet[eth_recv_buf->write].data,
		packet,
		len = MIN(len, ETH_FRAME_LEN));

	// Store the packet length.
	eth_recv_buf->packet[eth_recv_buf->write].length = len;

	// Update the recv buffer write pointer.
	eth_recv_buf->write = (eth_recv_buf->write + 1) % ETH_RECV_BUF_DIM;

	// Update the packet counter.
	if (eth_recv_buf->count <= ETH_RECV_BUF_DIM) {
		(eth_recv_buf->count)++;
	} else {
		// The oldest packet has been overwritten, so update.
		// also the read pointer
		eth_recv_buf->read = (eth_recv_buf->read + 1) % ETH_RECV_BUF_DIM;
	}
}

//! \brief
//!	Process an ethernet packet received from the physical layer.
//! \param packet The ethernet packet received.
//! \param len The size of the packet.
void to_eth_layer(ethernet_t *packet, size_t len)
{
#ifdef ETH_DEBUG
	dump_eth_packet(packet, len);
#endif
	// Wakeup the receive daemons.
	switch( ntohs(packet->type) )
	{
		case ETH_FRAME_IP:
			store_eth_packet(packet, len);
			wakeup_task( eth_recv_thread );
		break;

		case ETH_FRAME_ARP:
			store_eth_packet(packet, len);
			wakeup_task( arp_recv_thread );
		break;

		default:
			printk(KERN_NOTICE
				"%s(): unknown protocol type %u received\n",
				__FUNCTION__, ntohs(packet->type));
		break;
	}
}

// --- SEND routines -------------------------------------------------- //

//! \brief Send an ethernet packet to the physiscal layer.
//! \param to The destination ethernet address (MAC).
//! \param data The buffer of data to be sent.
//! \param len The size of the buffer of data to be sent.
//! \param type The packet type.
//! \return
//!	\li The number of bytes sent in case of success.
//!	\li -#ENOMEM cannot allocate the packet structure.
//!	\li -#ENXIO no ethernet device found.
int send_eth_packet(const uint8_t *to, const void *data, size_t len, uint16_t type)
{
	uint8_t *packet;
	uint8_t *mac_addr;

	// Analyze the packet length (must be less than ETH_MTU)	//
	// TODO: if the packet length if great than ETH_MTU		//
	// perform a packet fragmentation.				//
	len = MIN(len, ETH_MTU);

	// Create the ethernet packet					//
	packet = kmalloc( MAX(len+ETH_HEAD_LEN, ETH_MIN_LEN), GFP_KERNEL );
	if (!packet)
		return(-ENOMEM);

	// Get the local mac address					//
	if ( (mac_addr = get_eth_mac_addr()) == NULL )
		// No such device or address!				//
		return(-ENXIO);

	// Add the ethernet header to the packet			//
	memcpy(packet, to, ETH_ADDR_LEN);
	memcpy(packet + ETH_ADDR_LEN, mac_addr, ETH_ADDR_LEN);
	memcpy(packet + 2 * ETH_ADDR_LEN, &type, sizeof(uint16_t));

	// Copy the data into the packet				//
	memcpy(packet + ETH_HEAD_LEN, data, len);

	// Adjust the packet length including the size of the header	//
	len += ETH_HEAD_LEN;

	// Auto-pad! Send a minimum payload (another 4 bytes are	//
	// sent automatically for the FCS, totalling to 64 bytes)	//
	// It is the minimum length of an ethernet packet.		//
	while (len < ETH_MIN_LEN)
		packet[len++] = '\0';

	// Go to the physical layer					//
	len = send_rtl8139_packet(get_rtl8139_device(), packet, len);

	// Free the memory of the packet				//
	kfree(packet);

	// Return the bytes transmitted at this level			//
	return(len);
}

// --- Interface routines --------------------------------------------- //

//! \brief
//!	Check if the ethernet device has been configured in
//!	promiscuous mode.
//! \return
//!	\li #TRUE the ethernet card is in promiscuous mode;
//!	\li #FALSE the ethernet card is not in promiscuous mode.
bool is_eth_promisc()
{
	return( get_rtl8139_device()->promisc );
}

//! Default IP address for this host.
#define DEFAULT_IP_ADDR		( IP_ADDRESS(10, 0, 0, 2) )
//! Default IP netmask for this host.
#define DEFAULT_NETMASK_ADDR	( IP_ADDRESS(255, 0, 0, 0) )
//! \brief
//!	This is the "ifconfig" command for the shell.
//! \param cmd A string of command parameters.
void ifconfig(char *cmd)
{
	uint8_t ip_dot[STR_MAX_LENGTH];
	in_addr_t ip, netmask;

	// Skip initial spaces into the parameters string		//
	while (*cmd == ' ') cmd++;
	if (*cmd == '\0')
	{
		// Simply print current ethernet configuration		//
		rtl8139_dump_info(rtl);
		return;
	}
	if ( strstr(cmd, "up") )
	{
		if ( rtl != NULL )
		{
			kprintf("\n\rRTL8139 is already up!");
			if ( strstr(cmd, "promisc") )
				rtl8139_promisc(get_rtl8139_device(), TRUE);
			else
				rtl8139_promisc(get_rtl8139_device(), FALSE);
			kprintf("\n\rRTL8139: promiscuous mode %s\n\r",
				(get_rtl8139_device()->promisc) ? "ENABLED" : "DISABLED");
			return;
		}

		// Get the host IP and netmask				//
		kprintf("\n\rIP address: ");
		gets(ip_dot);
		if ( !inet_aton(ip_dot, &ip) )
		{
			// Set the IP to default value			//
			ip = DEFAULT_IP_ADDR;
		}
		kprintf("\rNetmask   : ");
		gets(ip_dot);
		if ( !inet_aton(ip_dot, &netmask) )
		{
			// Set the netmask to default value		//
			netmask = DEFAULT_NETMASK_ADDR;
		}

		// Initialize the ethernet buffer			//
		if( eth_init_buffer() < 0 )
		{
			printk( KERN_ERR
				"%s(): cannot create the ethernet buffer!\n",
				__FUNCTION__ );
			return;
		}
		// Set the host ip and netmask addresses		//
		set_host_ip_net(ip, netmask);

		// Initialize the ARP manager.
		arp_init();

		// Open the TCP module					//
		tcp_module_init();

		// Initialize the RTL8139 device			//
		switch( rtl8139_init(strstr(cmd, "promisc") ? TRUE : FALSE) )
		{
			case ( 0 ):
			// --- SUCCESS ---				//
			// Dump the ethernet card informations		//
			rtl8139_dump_info(rtl);
			// Create the ethernet receiver thread		//
			eth_recv_thread =
				create_thread(
					&eth_recv_daemon,
					0, NULL,
					"eth_recvd" );
			break;

			case ( -ENXIO ):
			// --- DEVICE NOT FOUND ---			//
			// Null the host ip and netmask			//
			set_host_ip_net(0, 0);
			// Destroy the ethernet buffer			//
			kfree( eth_recv_buf );
			// Print the error message			//
			kprintf("\n\rRTL8139: cannot find an ethernet card!\n\r");
			break;

			case ( -EIO ):
			// --- I/O ERROR ---				//
			// Null the host ip and netmask			//
			set_host_ip_net(0, 0);
			// Destroy the ethernet buffer			//
			kfree( eth_recv_buf );
			// Print the error message			//
			kprintf("\n\rRTL8139: I/O error!\n\r");
			break;
		}
		return;
	}
	if ( strstr(cmd, "down") )
	{
		kprintf("\n\rRTL8139: shutting down eth0 device...");

		// Close the TCP module					//
		tcp_module_close();

		// Empty the ip packet queue.
		ip_clear_queue();

		// Destroy the ethernet buffer				//
		kfree( eth_recv_buf );

		// Kill the ethernet receiver thread			//
		if( eth_recv_thread != NULL )
		{
			kill(eth_recv_thread->pid);
		}

		// Shutdown the ARP manager.
		arp_shutdown();
		// Clear the host ip and netmask addresses		//
		set_host_ip_net(0, 0);
		// Close the RTL8139 interface				//
		rtl8139_close(&rtl);
		kprintf("\n\r");
		return;
	}

	// Command unknown!						//
	kprintf("\n\rerror: %s unknown!\n\r", cmd);
}
