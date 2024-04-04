/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


/******************************* O S S O ***********************************
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/printk.h,v $
 * Prototypes of functions defined in printk.c
 ***************************************************************************/

/***************************************************************************/
/*
 * $Id: printk.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 */
/***************************************************************************/
/*
 * $Log: printk.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.7  2001/03/01 12:56:05  iurlano
 * Added some functions
 *
 * Revision 1.6  2000/12/12 22:28:59  iurlano
 * Threadified fork(). To be tested but seems to be working
 * Added support for coloured printk() in kernel.
 *
 * Revision 1.5  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.4  2000/12/05 16:36:01  iurlano
 * Thread managementwork in progress
 *
 * Revision 1.3  2000/11/26 23:34:59  iurlano
 * work in progress on DMA memory
 *
 * Revision 1.2  2000/10/25 19:59:39  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.4  1999/11/25 13:08:57  pecciant
 * Standardizzati gli header dei .h
 *
 * Revision 1.3  1999/10/27 21:57:43  iurlano
 * Aggiunto header per Pnp (e' vuoto per ora)
 *
 * Revision 1.2  1999/10/21 15:38:41  iurlano
 * Aggiustamento commenti
 * Aggiunta header per la paginazione
 *
 */
/***************************************************************************/


#ifndef __PRINTK_DEFS
#define __PRINTK_DEFS

#include <types.h>
#define Default_Color 0x7

#define Black          0
#define Blue           1
#define Green          2
#define Cyan           3
#define Red            4
#define Magenta        5
#define Brown          6
#define LightGray      7

#define DarkGray       8
#define LightBlue      9
#define LightGreen    10
#define LightCyan     11
#define LightRed      12
#define LightMagenta  13
#define Yellow        14
#define White         15


/* If __PRINT_MODULE_NAME__ is defined, then print it before printing
   anything. */

#ifdef __PRINT_MODULE_NAME__
//#define printk(x...) {_color_printk(Red,"%s : ",__PRINT_MODULE_NAME__);_printk(x);}
#define printk(x...) {_color_printk(Red,"%s : ",__FILE__);_printk(x);}
#else 
#define printk(x...) _printk(x)
#endif

void get_initial_cursor_pos(void);

void set_videomem_start_offset(word offset);
word get_videomem_start_offset(void);

#define _printk(x...) _color_printk(0x7,x)

int _color_printk(char attribute,char * stringa, ...);

#define PRINTC(x...) _PRINTC(Default_Color,x)
#define PRINTS(x...) _PRINTS(Default_Color,x)
#define PRINTD(x...) _PRINTD(Default_Color,x)
#define PRINTU(x...) _PRINTU(Default_Color,x)
#define PRINTX(x...) _PRINTX(Default_Color,x)

void _PRINTC(char attribute,const char ch);
void _PRINTS(char attribute,const char * srt);
void _PRINTD(char attribute,signed int d);
void _PRINTU(char attribute,unsigned int d);
void _PRINTX(char attribute,unsigned int d);

#endif
