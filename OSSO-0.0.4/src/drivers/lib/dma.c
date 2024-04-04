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
 * file : lib/dma.c                                                        *
 * DMA Routines                                                            *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: dma.c,v 1.1 2001/08/15 20:06:36 iurlano Exp $
 ***************************************************************************
 *
 * $Log: dma.c,v $
 * Revision 1.1  2001/08/15 20:06:36  iurlano
 * Added DMA management functions into the kernel and library
 *
 *
 ***************************************************************************/

#include <syscalls.h>
#include <messages.h>


int DMA_Op(dword Channel,dword mode,byte * buffer,word length) {
  int ret;
  __asm__ ("pushl %%ebx\n"
	   "movl %%esi,%%ebx\n"
	   "int $0x40\n"
	   "popl %%ebx\n":"=a" (ret):"a" (SYS_START_DMA_OP),
	   "S" (Channel),"c" (mode),"d" ((dword)buffer),"D" ((dword)length));
  return ret;
}

int DMA_Request_Channel(int channel) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_REQ_DMA_CHANNEL),"c"(channel));
  return ret;
}
int DMA_Free_Channel(int channel) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_FREE_DMA_CHANNEL),"c"(channel));
  return ret;
}

