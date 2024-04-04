/*!	\file include/net/eth.h
 *	\brief Ethernet driver header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-21
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef ETH_H
#define ETH_H

#include <kernel/semaphore.h>

//! IP packet type.
#define ETH_FRAME_IP		0x0800
//! ARP packet type.
#define ETH_FRAME_ARP		0x0806
//! Size of an ethernet address (a.k.a. MAC).
#define ETH_ADDR_LEN		6
//! Size of the ethernet header.
#define ETH_HEAD_LEN		14
//! Minimum ethernet packet size.
#define ETH_MIN_LEN		60
//! Maximum ethernet packet size.
#define ETH_FRAME_LEN		1514
//! Ethernet MTU (Maximum transfer unit).
#define ETH_MTU			(ETH_FRAME_LEN - ETH_HEAD_LEN)

//! Send and receive buffers size.
#define ETH_RECV_BUF_DIM	10

//! Physical packet structure.
typedef struct phys_packet
{
	//! The packet size.
	size_t length;
	//! The packet content.
	uint8_t data[ETH_FRAME_LEN];
} phys_packet_t;

//! The ethernet buffer structure.
typedef struct eth_buf
{
	//! The packets pointers.
	phys_packet_t packet[ETH_RECV_BUF_DIM];
	//! The position of the next packet received.
	int read;
	//! The position of the next packet to be sent.
	int write;
	//! The amount of packets in the buffer.
	int count;
} eth_buf_t;

/** \ingroup Network
 *  \defgroup NetEthernet Ethernet Layer
 *  The ethernet layer.
 *  @{
 */

//! An ethernet packet structure.
typedef struct ethernet
{
	//! Destination MAC address.
	uint8_t  dst[ETH_ADDR_LEN];
	//! Source MAC address.
	uint8_t  src[ETH_ADDR_LEN];
	//! The packet type.
	uint16_t type;
	//! The packet content.
	uint8_t data[1];
}  __attribute__ ((packed)) ethernet_t;

/** @} */ // end of NetEthernet

#endif
