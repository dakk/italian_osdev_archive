/*!	\file include/net/network.h
 *	\brief Header for the network driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-09
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef NETWORK_H
#define NETWORK_H

#if __BYTE_ORDER__==__LITTLE_ENDIAN__
	//! Network to host conversion for a word.
	#define ntohs(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )
	//! Host to network conversion for a word.
	#define htons(n) ( (((n) & 0xFF00) >> 8) | (((n) & 0x00FF) << 8) )
	//! Network to host conversion for a double word.
	#define ntohl(n) ( (((n) & 0xFF000000) >> 24) | (((n) & 0x00FF0000) >> 8) \
		| (((n) & 0x0000FF00) << 8) | (((n) & 0x000000FF) << 24) )
	//! Host to network conversion for a double word.
	#define htonl(n) ( (((n) & 0xFF000000) >> 24) | (((n) & 0x00FF0000) >> 8) \
		| (((n) & 0x0000FF00) << 8) | (((n) & 0x000000FF) << 24) )
#else
	//! Network to host conversion for a word.
	#define ntohs(n) (n)
	//! Host to network conversion for a word.
	#define htons(n) (n)
	//! Network to host conversion for a double word.
	#define ntohl(n) (n)
	//! Host to network conversion for a double word.
	#define htonl(n) (n)
#endif

#endif
