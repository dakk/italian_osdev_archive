/*!	\file include/kernel/keyboard.h
 *	\brief Low level keyboard driver header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-08
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <arch/interrupt.h>

// --- Keyboard registers --------------------------------------------- //

//! Keyboard base address port.
#define KEYB_PORT		0x60
//! Keyboard acknowledge register.
#define KEYB_ACK		0x61
//! Keyboard status register.
#define KEYB_STATUS		0x64
//! Keyboard LED register.
#define KEYB_LED_CODE		0xED

// --- Keyboard register values --------------------------------------- //

//! The keyboard controller is busy.
#define KEYB_BUSY		0x02
//! Command to set the typematic delay and rate.
#define KEYB_SET_TYPEMATIC	0xF3

// --- Key codes ------------------------------------------------------ //

//! DEL scan code.
#define DEL_SCAN		83

//! CTRL+C ASCII code.
#define CTRL_C			0x2E03
//! CTRL+X ASCII code.
#define CTRL_X			0x2D18
//! CTRL+X ASCII code.
#define CTRL_D			0x2004

//! ALT+F1 ASCII code.
#define ALT_F1			0x6800
//! ALT+F2 ASCII code.
#define ALT_F2			0x6900
//! ALT+F3 ASCII code.
#define ALT_F3			0x6A00
//! ALT+F4 ASCII code.
#define ALT_F4			0x6B00
//! ALT+F5 ASCII code.
#define ALT_F5			0x6C00
//! ALT+F6 ASCII code.
#define ALT_F6			0x6D00
//! ALT+F7 ASCII code.
#define ALT_F7			0x6E00
//! ALT+F8 ASCII code.
#define ALT_F8			0x6F00
//! ALT+F9 ASCII code.
#define ALT_F9			0x7000
//! ALT+F10 ASCII code.
#define ALT_F10			0x7100
//! ALT+F11 ASCII code.
#define ALT_F11			0x8B00
//! ALT+F12 ASCII code.
#define ALT_F12			0x8C00

//! Up.
#define KEYB_UP			0x4800
//! Down.
#define KEYB_DOWN		0x5000
//! Left.
#define KEYB_LEFT		0x4b00
//! Right.
#define KEYB_RIGHT		0x4d00
//! Page up.
#define KEYB_PAGE_UP		0x4900
//! Page down.
#define KEYB_PAGE_DOWN		0x5100
//! Home.
#define KEYB_HOME		0x4700
//! End.
#define KEYB_END		0x4f00

// --- Prototypes ----------------------------------------------------- //

void update_leds();
void keyb_wait_controller();
void init_keyboard();
word scan_key();
void keyboard_handler( irq_context_t *c );
int kgetchar();
int keyb_read();

#endif
