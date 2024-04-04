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
 * file : kernel/panic.c                                                   *
 * Definition of panic()                                                   *
 ***************************************************************************

 ***************************************************************************
 * $Id: panic.c,v 1.4 2001/04/18 21:39:43 iurlano Exp $
 ***************************************************************************
 *
 * $Log: panic.c,v $
 * Revision 1.4  2001/04/18 21:39:43  iurlano
 * Fixed a very insidious memory allocation bug in (getbmsg())
 * little change in xconfig
 * fixed stability problems
 * added two programs for testing message passing
 * Added freemsg()
 * no memory leak now on message passing
 * the shell seems to work!!
 * A strange problem: bochs reports freepage page already freed where the real pc and vmware do not!
 *
 * Revision 1.3  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.2  2000/06/12 08:05:26  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:12  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#include <printk.h>
#include <panic.h>
#include <sys.h>
#include <process.h>

void panic(char * text) {
  cli();
  printk("OSSO PaniK : ");
  printk(text);
  printk("\nRunning Process = %d, running thread = %d\n",
	 Processo_In_Esecuzione->pid,Thread_In_Esecuzione);
  while (1) __asm__("hlt");
}
