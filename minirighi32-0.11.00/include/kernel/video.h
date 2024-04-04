/*!	\file include/kernel/video.h
 *	\brief Low level video controller 6845 driver header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-08
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef VIDEO_H
#define VIDEO_H

#include <const.h>
#include <kernel/console.h>

//! The blank character.
#define BLANK	(0x20 | (DEFAULT_COLOR << 8))

//! The physical address of the video memory buffer.
#define VIDEO_MEM_ADDRESS	PHYSICAL(0xB8000)

//! The video CRT port.
#define VIDEO_CRT_PORT		0x03d4

// --- Prototypes ----------------------------------------------------- //

int k_get_cur_pos(console_t *console);
bool k_set_cur_pos(console_t *console, word pos);
int k_get_color(console_t *console);
void k_set_color(console_t *console, byte attrib);
void k_scroll_up(console_t *console);
void k_gotoxy(console_t *console, int x, int y);
// *************** Giovacchini Luca *********************
int k_get_crt_height();
int k_get_crt_width();
// ******************************************************

void video_clrscr(console_t *console);
void video_putchar(console_t *console, byte c);
void init_video();

#endif
