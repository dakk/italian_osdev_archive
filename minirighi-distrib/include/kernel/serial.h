/*!	\file include/kernel/serial.h
 *	\brief Serial/RS232 port driver header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-08
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <kernel/keyboard.h>

//! Serial port base address (for COM1).
#define PORT1	0x3F8
//! Serial port base address (for COM2).
#define PORT2	0x2F8
//! Serial port base address (for COM3).
#define PORT3	0x3E8
//! Serial port base address (for COM4).
#define PORT4	0x2E8

//! COM1 port address.
#define COM1	PORT1
//! COM2 port address.
#define COM2	PORT2
//! COM3 port address.
#define COM3	PORT3
//! COM4 port address.
#define COM4	PORT4

// --- Baud rates ----------------------------------------------------- //

#define BPS_115200	0x01	//!< 115,200 BPS.
#define BPS_56700	0x02	//!< 56,700  BPS.
#define BPS_38400	0x03	//!< 38,400  BPS.
#define BPS_19200	0x06	//!< 19,200  BPS.
#define BPS_9600	0x0C	//!< 9,600   BPS.
#define BPS_4800	0x18	//!< 4,800   BPS.
#define BPS_2400	0x30	//!< 2,400   BPS.

//! RS232 buffer size.
#define RS232_BUF_DIM	256

// --- Prototypes ----------------------------------------------------- //

void init_rs232(word port, byte divisor);
void close_rs232(word port);
byte rs232_getchar();
void rs232_putchar(byte c);
void rs232_chat();

#endif
