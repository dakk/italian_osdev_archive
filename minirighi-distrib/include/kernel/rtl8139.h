/*!	\file include/kernel/rtl8139.h
 *	\brief
 *		RTL8139C(L) single chip fast ethernet controller
 *		driver header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 *
 *	\image html rtl8139.jpg "How the RTL8139C controller works"
 */

#ifndef RTL8139_H
#define RTL8139_H

#include <arch/interrupt.h>
#include <kernel/semaphore.h>

//! Threshold is bytes transferred to chip before transmission starts
//! (rounded down to 32 byte units).
#define TX_FIFO_THRESH		256
//! Rx buffer level before first PCI xfer.
#define RX_FIFO_THRESH		4
//! Maximum PCI burst, '4' is 256 bytes.
#define RX_DMA_BURST		4
//! Calculate as 16<<val.
#define TX_DMA_BURST		4
//! Number of Tx descriptor registers.
#define NUM_TX_DESC		4
//! Size of the Tx bounce buffers -- must be at least (dev->mtu+14+4).
#define TX_BUF_SIZE	1536
//! The receive buffer length code: 0, 1, 2 is allowed - 8, 16, 32KB.
#define RX_BUF_LEN_IDX      0
//! The receive buffer length in bytes.
#define RX_BUF_LEN          (8192 << RX_BUF_LEN_IDX)

// Serial EEPROM section						//
#define EE_SHIFT_CLK    0x04	//!< EEPROM shift clock.
#define EE_CS           0x08	//!< EEPROM chip select.
#define EE_DATA_WRITE   0x02	//!< EEPROM chip data in.
#define EE_WRITE_0      0x00	//!< EEPROM write 0.
#define EE_WRITE_1      0x02	//!< EEPROM write 1.
#define EE_DATA_READ    0x01	//!< EEPROM chip data out.
#define EE_ENB          (0x80 | EE_CS)

//! Delay between EEPROM clock transitions.
//! No extra delay is needed with 33Mhz PCI,
//! but 66Mhz may change this.
#define eeprom_delay()  in32(ee_addr)

//! EEPROM write command.
#define EE_WRITE_CMD    (5 << 6)
//! EEPROM read command.
#define EE_READ_CMD     (6 << 6)
//! EEPROM erase command.
#define EE_ERASE_CMD    (7 << 6)

/** \ingroup Drivers
 *  \defgroup RTL8139Driver RTL8139C(L) Fast Ethernet Controller
 *  The RTL8139C(L) sinle chip fast ethernt controller driver.
 *  @{
 */

//! A receive packet structure.
typedef struct rxpacket
{
	size_t length;		//!< The packet size.
	uint16_t type;		//!< The packet type.
	uint8_t data[1];	//!< The packet contents.
} rxpacket_t;

//! A transmitted packet structure.
typedef struct txpacket
{
	//! The transmition buffer number.
	int buffer;
	//! A timeout for the transmitted packet.
	unsigned timeout;
} txpacket_t;

//! The RTL8139 device structure.
typedef struct rtl8139
{
	//! I/O port base address.
	uint16_t iobase;
	//! The IRQ line.
	uint8_t irq;
	//! The MAC address.
	uint8_t station_address[6];
	//! 1=10Mbps / 0=100Mbps.
	bool speed10;
	//! 1=full-duplex / 0=half-duplex.
	bool fullduplex;
	//! 1=promiscuous mode ON / 0=promiscuous mode OFF.
	bool promisc;
	//! Current Tx ring.
	unsigned cur_tx;
	//! Current Rx ring.
	unsigned cur_rx;

	//! Physical address of the Rx ring.
	addr_t	rx_phys,
	//! Physical address of the Tx ring.
		tx_phys;
	//! The Rx ring buffer (virtual address).
	uint8_t	*rx_ring,
	//! The Tx ring buffer (virtual address).
		*tx_ring;
	//! A mutual exclusion semaphore for the RTL8139 device.
	semaphore_t mutex;
} rtl8139_t;

// --- Prototypes ----------------------------------------------------- //

uint8_t *get_eth_mac_addr();
rtl8139_t *get_rtl8139_device();
int send_rtl8139_packet(rtl8139_t *rtl, const void *data, size_t len);
int rtl8139_init(bool promisc);
void rtl8139_close(rtl8139_t **rtl);
void rtl8139_promisc(rtl8139_t *rtl, bool on);
void rtl8139_dump_info(rtl8139_t *rtl);
void rtl8139_handler( irq_context_t *c );

/** @} */ // end of RTL8139Driver

#endif
