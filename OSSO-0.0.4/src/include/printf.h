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


/*************************** lib O S S O  ***********************************
 * file : include/lib/printf.h
 * Header for printf
 ***************************************************************************

 ***************************************************************************
 * $Id: printf.h,v 1.5 2001/06/13 16:45:28 steros Exp $
 ***************************************************************************
 *
 * $Log: printf.h,v $
 * Revision 1.5  2001/06/13 16:45:28  steros
 * FAT/SHELL Restructuration, bug fixes and flush function to avoid line
 * buffering.
 *
 * Revision 1.4  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.3  2001/05/29 17:41:17  steros
 * Modified printc for line buffered functionality.
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.3  2000/10/25 19:59:39  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:20  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.1  1996/09/12 05:04:44  iurlano
 * Riordino directory test e lib
 * adesso test compila con lib
 * prove di gestione di segnale a livello utente
 *
 *
 ***************************************************************************/

#include "../Config_Global.h"

#ifndef __PRINTF_LIB
#define __PRINTF_LIB

/* If __PRINT_MODULE_NAME is defined, then print it before printing
   anything. */

#ifdef __PRINT_MODULE_NAME__
#define printf(x...) {_printf("%s : ",__PRINT_MODULE_NAME__);_printf(x);}
#else
#define printf(x...) _printf(x)
#endif

#ifdef LINE_BUFFERED_OUTPUT
  extern int LineBuffered;

  void printc_flush(void);
#endif

int _printf(char * stringa, ...);

#endif
