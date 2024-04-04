/*!	\file drivers/rtl8139/rtl8139.c
 *	\brief RTL8139C(L) single chip fast ethernet controller driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-03
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 *
 *	\n
 *	Based on drivers/net/8139too.c from Linux.
 *	-Andrea Righi.
 */

#include <const.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <arch/clock.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/mem.h>
#include <arch/paging.h>

#include <kernel/dma.h>
#include <kernel/kmalloc.h>
#include <kernel/pci.h>
#include <kernel/semaphore.h>
#include <kernel/video.h>

#include <net/network.h>
#include <kernel/net/arp.h>
#include <kernel/net/eth.h>
#include <kernel/net/ip.h>

#include <kernel/rtl8139.h>

//! Symbolic RTL8139 offsets to registers.
enum RTL8139_registers
{
	MAC0=0,			//!< Ethernet hardware address.
	MAR0=8,			//!< Multicast filter.
	TxStatus0=0x10,		//!< Transmit status: 4 32bit registers.
	TxAddr0=0x20,       	//!< Tx descriptors (also 4 32bit).

	RxBuf=0x30,
	RxEarlyCnt=0x34,
	RxEarlyStatus=0x36,

	ChipCmd=0x37,
	RxBufPtr=0x38,
	RxBufAddr=0x3A,

	IntrMask=0x3C,
	IntrStatus=0x3E,
	TxConfig=0x40,
	RxConfig=0x44,

	Timer=0x48,		//!< General-purpose counter.
	RxMissed=0x4C,		//!< 24 bits valid, write clears.

	Cfg9346=0x50,
	Config0=0x51,
	Config1=0x52,

	TimerIntrReg=0x54, //!< INTR if gp counter reaches this value.
	MediaStatus=0x58,
	Config3=0x59,
	HltClk=0x5B,
	MultiIntr=0x5C,
	RevisionID=0x5E,	//!< Revision of the RTL8139 chip.
	TxSummary=0x60,

	MII_BMCR=0x62,
	MII_BMSR=0x64,

	NWayAdvert=0x66,
	NWayLPAR=0x68,
	NWayExpansion=0x6A,

	DisconnectCnt=0x6C,
	FalseCarrierCnt=0x6E,
	NWayTestReg=0x70,

	RxCnt=0x72,		//!< Packet received counter.
	CSCR=0x74,	//!< Chip status and configuration register.

	PhyParm1=0x78,		//!< Undocumented.
	TwisterParm=0x7c,	//!< Undocumented.
	PhyParm2=0x80,		//!< Undocumented.

	// From 0x84 onwards are a number of power management/wakeup	//
	// frame definitions we will probably never need to know about.	//
};

enum ChipCmdBits
{
	CmdReset=0x10,
	CmdRxEnb=0x08,
	CmdTxEnb=0x04,
	RxBufEmpty=0x01,
};

//! Interrupt Status Register bits.
enum IntrStatusBits
{
	PCIErr=0x8000,
	PCSTimeout=0x4000,
	CableLenChange= 0x2000,
	RxFIFOOver=0x40,
	RxUnderrun=0x20,
	RxOverflow=0x10,
	TxErr=0x08,
	TxOK=0x04,
	RxErr=0x02,
	RxOK=0x01,
	IntrDefault = RxOK | TxOK,
};

//! Transmit Status Register bits.
enum TxStatusBits
{
	TxHostOwns=0x2000,
	TxUnderrun=0x4000,
	TxStatOK=0x8000,
	TxOutOfWindow=0x20000000,
	TxAborted=0x40000000,
	TxCarrierLost=0x80000000,
};

//! Receive Status Register bits.
enum RxStatusBits
{
	RxMulticast=0x8000,
	RxPhysical=0x4000,
	RxBroadcast=0x2000,
	RxBadSymbol=0x0020,
	RxRunt=0x0010,
	RxTooLong=0x0008,
	RxCRCErr=0x0004,
	RxBadAlign=0x0002,
	RxStatusOK=0x0001,
};

//! Media Status Register bits.
enum MediaStatusBits
{
	MSRTxFlowEnable=0x80,
	MSRRxFlowEnable=0x40,
	MSRSpeed10=0x08,
	MSRLinkFail=0x04,
	MSRRxPauseFlag=0x02,
	MSRTxPauseFlag=0x01,
};

enum MIIBMCRBits
{
	BMCRReset=0x8000,
	BMCRSpeed100=0x2000,
	BMCRNWayEnable=0x1000,
	BMCRRestartNWay=0x0200,
	BMCRDuplex=0x0100,
};

enum CSCRBits
{
	CSCR_LinkOKBit=0x0400,
	CSCR_LinkChangeBit=0x0800,
	CSCR_LinkStatusBits=0x0f000,
	CSCR_LinkDownOffCmd=0x003c0,
	CSCR_LinkDownCmd=0x0f3c0,
};

//! Bits in Receive Configuration Register.
enum rx_mode_bits
{
	RxCfgWrap=0x80,
	AcceptErr=0x20,
	AcceptRunt=0x10,
	AcceptBroadcast=0x08,
	AcceptMulticast=0x04,
	AcceptMyPhys=0x02,
	AcceptAllPhys=0x01,
};

//! Pointer to the RTL8139 structure.
rtl8139_t *rtl=NULL;

//! \brief Halt the RTL8139 card and free the logical device.
//! \param rtl
//!	An indirect pointer to the RTL8139 structure. Must be
//!	passed as an indirect pointer because it will be modified
//!	inside this routine!!!
void rtl8139_close(rtl8139_t **rtl)
{
	// Disable interrupts by clearing the interrupt mask		//
	out16((*rtl)->iobase + IntrMask, 0);
	// Stop the chip's Tx and Rx DMA processes			//
	out((*rtl)->iobase + ChipCmd, 0);

	// Put the chip in low-power mode				//
	out((*rtl)->iobase + Cfg9346, 0xC0);
	out((*rtl)->iobase + Config1, 0x03);
	// 'R' would leave the clock running				//
	out((*rtl)->iobase + HltClk, 'H');

	if ( (*rtl) != NULL )
	{
		// Uninstall the handler.
		uninstall_handler( (*rtl)->irq );

		// Deallocate the ring buffers				//
		dma_phys_free((*rtl)->rx_phys, RX_BUF_LEN);
		dma_phys_free((*rtl)->tx_phys, TX_BUF_SIZE*NUM_TX_DESC);

		// Deallocate the logical structure			//
		kfree((*rtl));
		*rtl = NULL;
	}
}

//! \brief Read RTL8139 informations from the EEPROM.
//! \param rtl The RTL8139 structure.
//! \param location The location in the EEPROM to read.
//! \return The value read from the EEPROM.
static unsigned rtl8139_read_eeprom(rtl8139_t *rtl, unsigned location)
{
	int i;
	unsigned int retval = 0;
	long ee_addr = rtl->iobase + Cfg9346;
	int read_cmd = location | EE_READ_CMD;

	out(ee_addr, EE_ENB & ~EE_CS);
	out(ee_addr, EE_ENB);

	// Shift the read command bits out				//
	for (i = 10; i >= 0; i--)
	{
		int dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
		out(ee_addr, EE_ENB | dataval);
		eeprom_delay();
		out(ee_addr, EE_ENB | dataval | EE_SHIFT_CLK);
		eeprom_delay();
	}

	out(ee_addr, EE_ENB);
	eeprom_delay();

	for (i = 16; i > 0; i--)
	{
		out(ee_addr, EE_ENB | EE_SHIFT_CLK);
		eeprom_delay();
		retval = (retval << 1) | ((in(ee_addr) & EE_DATA_READ) ? 1 : 0);
		out(ee_addr, EE_ENB);
		eeprom_delay();
	}

	// Terminate the EEPROM access					//
	out(ee_addr, ~EE_CS);
	return(retval);
}

//! \brief Enable/disable the promiscuous mode for the RTL8139 card.
//! \param rtl The RTL8139 structure.
//! \param on
//!	\li #TRUE enable the promiscuous mode;
//!	\li #FALSE disable the promiscuous mode.
void rtl8139_promisc(rtl8139_t *rtl, bool on)
{
	uint8_t rxc;
	uint32_t flags;

	local_irq_save( flags );

	rxc = in(rtl->iobase + RxConfig);
	if ( on )
		out(rtl->iobase + RxConfig, rxc | AcceptAllPhys);
	else
		out(rtl->iobase + RxConfig, rxc & (~AcceptAllPhys));
	rtl->promisc = on;

	local_irq_restore( flags );
}

//! \brief Reset the RTL8139 ethernet card.
//! \param rtl The RTL8139 structure.
void rtl8139_reset(rtl8139_t *rtl)
{
	unsigned i;
	timeout_t elapsed;
	uint32_t flags;

	save_flags( flags );

	// Send the software-reset command				//
	out(rtl->iobase + ChipCmd, CmdReset);

	// Reset the Rx and Tx rings					//
	rtl->cur_rx = 0;
	rtl->cur_tx = 0;

	// Give the chip 10ms to finish the reset			//
	enable();
	set_timeout(&elapsed, 10);
	while ( (in(rtl->iobase + ChipCmd) & CmdReset) != 0 )
		if ( is_timeout(&elapsed) )
			break;

	restore_flags( flags );

	// Set-up the card MAC address register				//
	for (i=0; i<_countof(rtl->station_address); i++)
        	out(rtl->iobase + MAC0 + i, rtl->station_address[i]);

	// Must enable Tx/Rx before setting transfer thresholds!	//
	out(rtl->iobase + ChipCmd, CmdRxEnb | CmdTxEnb);
	 // Accept no frames yet!					//
	out32(rtl->iobase + RxConfig,
		(RX_FIFO_THRESH<<13) | (RX_BUF_LEN_IDX<<11) | (RX_DMA_BURST<<8));
	out32(rtl->iobase + TxConfig, (TX_DMA_BURST<<8) | 0x03000000);

	// Configure the RX buffer					//
	out32(rtl->iobase + RxBuf, rtl->rx_phys);

	// Init Tx buffer DMA addresses					//
	for (i = 0; i < NUM_TX_DESC; i++)
		out32(rtl->iobase + TxAddr0 + (i * 4), rtl->tx_phys + (TX_BUF_SIZE*i));

	// Reset the missed packets counter				//
	out32(rtl->iobase + RxMissed, 0);

	// Set RX mode							//
	if ( rtl->promisc )
		out(rtl->iobase + RxConfig, AcceptBroadcast | AcceptMyPhys | AcceptAllPhys);
	else
		out(rtl->iobase + RxConfig, AcceptBroadcast | AcceptMyPhys);

	// If we add multicast support, the MAR0 register would have to //
	// be initialized to 0xFFFFFFFF.FFFFFFFF (two 32 bit accesses). //
	// Etherboot only needs broadcast (for ARP/RARP/BOOTP/DHCP) and //
	// unicast.							//

	// Start the chip's Tx and Rx process				//
	out(rtl->iobase + ChipCmd, CmdRxEnb | CmdTxEnb);

	// Disable all known interrupts by setting the interrupt mask	//
	// out16(rtl->iobase + IntrMask, 0x0000);

	// Enable all known interrupts					//
	// out16(rtl->iobase + IntrMask, PCIErr | PCSTimeout | RxUnderrun |
	// 	RxOverflow | RxFIFOOver |
	// 	TxErr | TxOK | RxErr | RxOK);

	// Enable default interrupt mask (RxOK | TxOK)			//
	out16(rtl->iobase + IntrMask, IntrDefault);
}

//! \brief Initialize the RTL8139 driver.
//! \param promisc
//!	\li #TRUE initialize the card and put it in promiscuous mode;
//!	\li #FALSE initialize the card and don't put it in promiscuous
//!	mode.
//! \return
//!	\li 0 if we can successfully enable the RTL8139 ethernet card;
//!	\li -#ENXIO if the device if not present on PCI bus;
//!	\li -#EIO if we cannot read ethernet card informations;
int rtl8139_init(bool promisc)
{
	pci_cfg_t cfg;
	unsigned i;
	uint32_t flags;

	// Search an ethernet device on PCI and enable it!		//
	cfg.base_class = 0x02;	// Ethernet class 			//
	cfg.sub_class  = 0x00;	// Ethernet sub_class			//
	cfg.interface  = 0x00;	// Ethernet interface			//

	if ( !pci_find_cfg(&cfg, TRUE) )
	{
		return(-ENXIO);
	}

	// Close a previous RTL8139 structure if present		//
	if ( rtl != NULL )
	{
		rtl8139_close(&rtl);
	}

	// Allocate the RTL8139 structure				//
	rtl = kmalloc(sizeof(rtl8139_t), GFP_KERNEL);
	if ( !rtl )
		return(-ENOMEM);

	memset(rtl, 0, sizeof(rtl8139_t));

	// Setup the I/O base register					//
	for (i=0; i<6; i++)
		if ( (rtl->iobase=cfg.base[i]) != 0 ) break;

	// Check if the device I/O base address is valid		//
	if ( rtl->iobase == 0 )
	{
		kfree(rtl);
		return(-EIO);
	}

	local_irq_save( flags );

	// Setup the IRQ line						//
	rtl->irq = cfg.irq;
	install_trap_handler( rtl->irq, (void *)&rtl8139_handler );

	// Bring the chip out of low-power mode				//
	out(rtl->iobase + Config1, 0x00);

	// Read the MAC address						//
	if (rtl8139_read_eeprom(rtl, 0) != 0xFFFF)
	{
		// Read from EEPROM					//
        	unsigned short *ap = (unsigned short*)rtl->station_address;
		for (i = 0; i < 3; i++)
		*ap++ = rtl8139_read_eeprom(rtl, i + 7);
	}
	else
	{
		// Read from MAC0 register				//
		unsigned char *ap = (unsigned char*)rtl->station_address;
		for (i = 0; i < 6; i++)
			*ap++ = in(rtl->iobase + MAC0 + i);
	}

	// Read the media informations					//
	rtl->speed10 = (in(rtl->iobase + MediaStatus) & MSRSpeed10) != 0;
	rtl->fullduplex = (in16(rtl->iobase + MII_BMCR) & BMCRDuplex) != 0;

	// Setup the promiscuous mode					//
	rtl->promisc = promisc;

	// Initialize the Rx and Tx rings buffer			//
	if ( !(rtl->rx_ring) )
	{
		rtl->rx_phys = (addr_t)dma_phys_alloc(RX_BUF_LEN);
		rtl->rx_ring = PHYSICAL( (uint8_t *)(rtl->rx_phys) );
	}
	if ( !(rtl->tx_ring) )
	{
		rtl->tx_phys = (addr_t)dma_phys_alloc(TX_BUF_SIZE*NUM_TX_DESC);
		rtl->tx_ring = PHYSICAL( (uint8_t *)(rtl->tx_phys) );
	}

	// Initialize the RTL8139 mutex semaphore			//
	init_MUTEX(&rtl->mutex);

	// Hardware reset of the RTL8139				//
	rtl8139_reset(rtl);

	// Well done!							//
	local_irq_restore( flags );
	return(0);
}

//! \brief Get the MAC hardware address of the RTL8139 ethernet card.
//! \return A pointer to the MAC address value.
//! \exception NULL if a RTL8139 card has not been initialized.
uint8_t *get_eth_mac_addr()
{
	if (rtl)
		return(rtl->station_address);
	else
		return(NULL);
}

//! \brief Get the pointer of the RTL8139 device structure.
//! \return
//!	\li a pointer of the RTL8139 structure if it has been
//!	initialized;
//!	\li 0 if the RTL8139 card has not been initialized.
rtl8139_t *get_rtl8139_device()
{
	return( rtl );
}

//! \brief Send an ethernet packet to the physiscal layer.
//! \param rtl The RTL8139 structure.
//! \param data The data to be sent.
//! \param len The size of the data to be sent in bytes.
//! \return The number of bytes sent, or -1 if an error occurred.
/**
 *	\todo Set up a timeout for the transmitted packets.
 */
int send_rtl8139_packet(rtl8139_t *rtl, const void *data, size_t len)
{
	uint32_t flags;

	// Check if the RTL8139 card has been configured		//
	if ( !rtl ) return(-1);

	// Adapt the packet length to the Tx buffer size		//
	// This should be done by the ethernet layer...			//
	len = MIN(len, TX_BUF_SIZE);

	// Enter critical region					//
	// DOWN(&rtl->mutex);
	local_irq_save(flags);

	if ( in32(rtl->iobase + TxStatus0 + (rtl->cur_tx * 4)) & TxHostOwns )
	{
		// Found a free buffer!				//

		// Copy the raw data to the Tx ring		//
		memcpy(rtl->tx_ring + (TX_BUF_SIZE*rtl->cur_tx), data, len);

		// NOTE: RTL8139 doesn't auto-pad! Be sure that //
		// the ethernet layer has done it correctly	//
		while (len < ETH_MIN_LEN)
			(rtl->tx_ring + (TX_BUF_SIZE*rtl->cur_tx))[len++] = '\0';

		// Move the data from the Tx buffer to the	//
		// internal transmit FIFO in PCI bus master	//
		// mode						//
		out32(rtl->iobase + TxAddr0 + rtl->cur_tx * 4,
			rtl->tx_phys + (TX_BUF_SIZE * rtl->cur_tx));
		out32(rtl->iobase + TxStatus0 + rtl->cur_tx * 4,
			((TX_FIFO_THRESH<<11) & 0x003F0000) | len);

		// Select new descriptor for transmition	//
		rtl->cur_tx = (rtl->cur_tx + 1) % NUM_TX_DESC;

		// End of critical region			//
		// UP(&rtl->mutex);
		local_irq_restore(flags);

		// Return the byte transmitted				//
		return(len);
	}
	// End of critical region					//
	// UP(&rtl->mutex);
	local_irq_restore(flags);

	// Buffer not available!					//
	return(-1);
}

//! \brief Handle a transmitted packet event.
//! \param rtl The RTL8139 structure.
void rtl8139_handle_tx(rtl8139_t *rtl)
{
	unsigned int i, status;

	for(i=0; i<NUM_TX_DESC; i++)
	{
		// We must read the status of every descriptor when	//
		// a Tx interrupt occurs				//
		status = in32(rtl->iobase + TxStatus0 + (i * 4));
#ifdef RTL8139_DEBUG
		kprintf("\nbuffer %u status %#010x", i, status);
#endif
	}
}

//! \brief Handle a received packet event.
//! \param rtl The RTL8139 structure.
void rtl8139_handle_rx(rtl8139_t *rtl)
{
	uint32_t ring_offs, rx_size, rx_status;
	rxpacket_t *packet;

	while ((in(rtl->iobase + ChipCmd) & RxBufEmpty) == 0)
	{
		ring_offs = rtl->cur_rx % RX_BUF_LEN;

		// Read status+size of the next frame from DMA		//
		// ring buffer						//
		rx_status = *(uint32_t *)(rtl->rx_ring + ring_offs);
		rx_size = rx_status >> 16;
		rx_status &= 0xFFFF;

		if (
			(rx_status & (RxBadSymbol | RxRunt | RxTooLong | RxCRCErr | RxBadAlign)) ||
			(rx_size < ETH_MIN_LEN) ||
			(rx_size > ETH_FRAME_LEN + 4)
		)
		{
#ifdef RTL8139_DEBUG
			kprintf("\nRX error! status=%#010x rx_size=%#010x\n", rx_status, rx_size);
#endif
			rtl8139_reset(rtl);
			return;
		}

		// Allocate a buffer to store the packet		//
		packet = kmalloc(sizeof(rxpacket_t) - 1 + rx_size - 4, GFP_ATOMIC);
		if (packet == NULL)
			return;

		// No one cares about the FCS				//
		packet->length = rx_size - 4;

		// Received a good packet				//
		if (ring_offs + 4 + rx_size - 4 > RX_BUF_LEN)
		{
        		int semi_count = RX_BUF_LEN - ring_offs - 4;

			memcpy(packet->data, rtl->rx_ring + ring_offs + 4, semi_count);
			memcpy(packet->data + semi_count, rtl->rx_ring, rx_size - 4 - semi_count);
#ifdef RTL8139_DEBUG
			kprintf("\nRX packet (%u+%u bytes)", semi_count, rx_size - 4 - semi_count);
#endif
		}
		else
		{
			memcpy(packet->data, rtl->rx_ring + ring_offs + 4, packet->length);
#ifdef RTL8139_DEBUG
			kprintf("\nRX packet (%u bytes)", rx_size-4);
#endif
		}
		#if 0
		// Dump the received packet content			//
		kprintf("\n");
		for (i=0; i<packet->length; i++)
			kprintf("%02X ", packet->data[i]);
		#endif
		rtl->cur_rx = (rtl->cur_rx + rx_size + 4 + 3) & ~3;
		out16(rtl->iobase + RxBufPtr, rtl->cur_rx - 16);

		// Send the packet to the upper layer			//
		to_eth_layer((ethernet_t *)packet->data, packet->length);

		// Free the packet sturcture				//
		kfree(packet);
	}
	return;
}

/** \ingroup Handlers */
//! \brief
//!	The RTL8139 interrupt handler routine. It is invoked every
//!	time that an ethernet packet is received or when a packet
//!	has been transmitted.
void rtl8139_handler( irq_context_t *c )
{
	uint16_t status;

	out16(rtl->iobase + IntrMask, 0);

	status = in16(rtl->iobase + IntrStatus);
	// Acknowledge all known interrupts				//
	out16(rtl->iobase + IntrStatus, status & ~(RxFIFOOver | RxOverflow | RxOK));

	if (status & (RxOK | RxErr))
		rtl8139_handle_rx(rtl);
	else
		if (status & (TxOK | TxErr))
			rtl8139_handle_tx(rtl);
#ifdef RTL8139_DEBUG
		else
			printk(KERN_WARNING
				"%s(): unknown interrupt! isr = %#010x\n",
				__FUNCTION__, status);
#endif

	// Re-enable interrupts from the RTL8139 card			//
	out16(rtl->iobase + IntrStatus, status & (RxFIFOOver | RxOverflow | RxOK));
	out16(rtl->iobase + IntrMask, IntrDefault);
}

//! \brief
//!	Print the current RTL8139 card configuration on the console.
//! \param rtl The RTL8139 structure.
void rtl8139_dump_info(rtl8139_t *rtl)
{
	in_addr_t ip, netmask, ip_bcast;

	if ( !rtl )
	{
		// The card is not configured				//
		kprintf("\nUsage: ifconfig [up|down]");
		kprintf("\nRTL8139: ethernet card not configured!\n");
		return;
	}

	// Dump the device informations					//
	kprintf("\nRTL8139: card configured @ I/O base=%04X IRQ=%u", rtl->iobase, rtl->irq);
	// Dump the MAC address						//
	kprintf("\nRTL8139: MAC address %02x:%02x:%02x:%02x:%02x:%02x",
		rtl->station_address[0], rtl->station_address[1], rtl->station_address[2],
		rtl->station_address[3], rtl->station_address[4], rtl->station_address[5]
	);
	// Dump the media informations					//
	kprintf("\nRTL8139: %sMbps %s-duplex",
		rtl->speed10 ? "10" : "100",
		rtl->fullduplex ? "full" : "half"
	);
	// Print the promiscuous mode flag				//
	kprintf("\nRTL8139: promiscuous mode %s", (rtl->promisc) ? "ENABLED" : "DISABLED");
	// Print virtual memory rings pointers				//
	kprintf("\nRTL8139: rx_ring=%p<->%p, tx_ring=%p<->%p",
		rtl->rx_ring, rtl->rx_ring + RX_BUF_LEN,
		rtl->tx_ring, rtl->tx_ring + PAGE_ALIGN_UP(TX_BUF_SIZE*NUM_TX_DESC)
	);
	// Well done!							//
	kprintf("\nRTL8139: OK, ethernet card is ready!");

	ip = ntohl( get_host_ip() );
	netmask = ntohl( get_host_netmask() );
	ip_bcast = (~netmask) | (ip & netmask);
	kprintf("\n\nhost ip=%u.%u.%u.%u netmask=%u.%u.%u.%u broadcast=%u.%u.%u.%u\n",
		IP_A(ip), IP_B(ip), IP_C(ip), IP_D(ip),
		IP_A(netmask), IP_B(netmask), IP_C(netmask), IP_D(netmask),
		IP_A(ip_bcast), IP_B(ip_bcast), IP_C(ip_bcast), IP_D(ip_bcast)
	);
}
