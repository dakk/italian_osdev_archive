/*!	\file include/net/tcp.h
 *	\brief Header for the TCP (Transmition Control Protocol).
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date 2004-08-21
 *	\note Copyright (C) 2003-2004 Andrea Righi
 */

#ifndef TCP_H
#define TCP_H

/** \ingroup Network
 *  \defgroup NetTCP TCP (Transmition Control Protocol) layer
 *  The TCP (Transmition Control Protocol) layer.
 *  @{
 */

//! TCP connection closed state.
#define TCP_CLOSED	0
//! TCP SYN received state.
#define TCP_SYN_RCVD	1
//! TCP SYN sent state.
#define TCP_SYN_SENT	2
//! TCP connection established state.
#define TCP_ESTABLISHED	3
//! TCP closing state.
#define TCP_CLOSING	4

//! A socket structure.
typedef struct
{
	//! IP source address (in network format).
	in_addr_t ip_src;
	//! Source port.
	uint16_t port_src;
	//! IP destination address (in network format).
	in_addr_t ip_dst;
	//! Destination port.
	uint16_t port_dst;
} socket_t;

//! A TCP connection states machine structure.
typedef struct
{
	// The state of the connection.
	int state;
	//! The socket used in the connection.
	socket_t socket;
	//! The current sequence number.
	int seq_num;
} tcp_state_t;

//! TCP packet structure.
typedef struct tcp
{
	//! Source port.
	uint16_t tcp_src;
	//! Destination port.
	uint16_t tcp_dst;

	//! Sequence number.
	uint32_t tcp_seq_num;
	//! ACK number.
	uint32_t tcp_ack_num;

#if __BYTE_ORDER__ == __LITTLE_ENDIAN__
	//! Reserved (bit 0..3).
	uint8_t tcp_res1:4;
	//! Header length.
	uint8_t tcp_hdr_len:4;
	//! FIN flag.
	uint8_t tcp_fin:1;
	//! SYN flag.
	uint8_t tcp_syn:1;
	//! RST flag.
	uint8_t tcp_rst:1;
	//! PSH flag.
	uint8_t tcp_psh:1;
	//! ACK flag.
	uint8_t tcp_ack:1;
	//! URG flag.
	uint8_t tcp_urg:1;
	//! Reserved (bit 4..6).
	uint8_t tcp_res2:2;
#else
	//! Header length.
	uint8_t tcp_hdr_len:4;
	//! Reserved.
	uint8_t tcp_res:6;
	//! URG flag.
	uint8_t tcp_urg:1;
	//! ACK flag.
	uint8_t tcp_ack:1;
	//! PSH flag.
	uint8_t tcp_psh:1;
	//! RST flag.
	uint8_t tcp_rst:1;
	//! SYN flag.
	uint8_t tcp_syn:1;
	//! FIN flag.
	uint8_t tcp_fin:1;
#endif
	//! Window size.
	uint16_t tcp_win_size;
	//! TCP checksum.
	uint16_t tcp_chk;
	//! Urgent pointer.
	uint16_t tcp_urg_ptr;

} __attribute__ ((packed)) tcp_t;

/** @} */ // end of NetTCP

#endif
