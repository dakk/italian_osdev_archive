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


/*************************** LIB O S S O ***********************************
 * file : lib/messages.c                                                   *
 * Messages Routines                                                       *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: messages.c,v 1.4 2001/04/17 18:04:12 iurlano Exp $
 ***************************************************************************
 *
 * $Log: messages.c,v $
 * Revision 1.4  2001/04/17 18:04:12  iurlano
 * Fixed compilation code
 * Warning: This is a very unstable snapshot
 * There are a lot of strange problems
 *
 * Revision 1.3  2000/10/23 23:33:44  iurlano
 * Remove ebx references from the libOSSO
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:18  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.3  2000/03/20 23:12:56  iurlano
 * Correzione ricezione messaggi
 *
 * Revision 1.2  1999/12/17 22:46:06  iurlano
 *
 * I messaggi sembrano funzionare !!!!
 *
 * Revision 1.1  1999/12/17 17:26:58  iurlano
 * Aggiunte alcune syscall, tipo getpid
 * Lavoro su sistema di messaggi
 * Migliorata libreria
 *
 *
 ***************************************************************************/

#include <syscalls.h>
#include <messages.h>


int send_msg(dword dest,void * buf,dword dim) {
  int ret;
  __asm__ ("pushl %%ebx\n"
	   "movl %%edi,%%ebx\n"
	   "int $0x40\n"
	   "popl %%ebx\n":"=a" (ret):"a" (SYS_SEND_MSG),
	   "D" (dest),"c" (buf),"d" (dim));
  return ret;
}


void * recv_msg(msginfo * ppp) {
  void * ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_RECV_MSG),"c" (ppp));
  return ret;
}

void * getbmsg(void) {
  void * ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_GETB_MSG));
  return ret;
}

int freemsg(void * msg) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_FREE_MSG),"c"((dword)msg));
  return ret;
}
