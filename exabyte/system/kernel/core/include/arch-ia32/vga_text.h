/* vga_text.h -- ia32 text video management definitions */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#ifndef IA32_VGA_TEXT_H
#define IA32_VGA_TEXT_H

#include <arch-ia32/config.h>

#ifdef VGA_TEXT_INTERNALS
/* DICHIARAZIONI PRIVATE PER IA32 */

#ifdef LANG_US
#define VIDEO_NORMAL1	"videotext: "
#define VIDEO_NORMAL2	"text console 80x25\n"
#define VIDEO_COLOR	"colour "
#define VIDEO_MONO	"monochrome "
#elif defined LANG_IT
#define VIDEO_NORMAL1	"videotext: console testuale "
#define VIDEO_NORMAL2	"80x25\n"
#define VIDEO_COLOR	"a colori "
#define VIDEO_MONO	"monocromatica "
#endif

#define TAB_SIZE	8

void scroll_up(void);
void clear_line(int n);
void new_line(void);

#endif

/* DICHIARAZIONI PUBBLICHE PER LA KPRINT DEL KERNEL */

/* attributi di testo */
#define VDTXT_TEXT_LIGHTGRAY		0x07

/* attributi di sfondo */
#define VDTXT_BACK_BLACK		0x00

#define	VDTXT_DEFAULT_TEXT	VDTXT_TEXT_LIGHTGRAY
#define	VDTXT_DEFAULT_BACK	VDTXT_BACK_BLACK
#define	VDTXT_DEFAULT_ATTR	(VDTXT_DEFAULT_TEXT | VDTXT_DEFAULT_BACK)

int videotext_init(unsigned int max_x,unsigned int max_y,unsigned int last_y);
int videotext_putc(char c,unsigned char attr);
int videotext_clear(unsigned char attr);

#endif	/* IA32_VGA_TEXT_H */
