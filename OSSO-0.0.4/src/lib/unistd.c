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


/****************************** O S S O  ***********************************
 * file : $Source: /cvsroot/osso/OSSO/src/lib/unistd.c,v $
 * READ and WRITE definitions
 ***************************************************************************

 ***************************************************************************
 * $Id: unistd.c,v 1.3 2000/11/02 11:59:37 iurlano Exp $
 ***************************************************************************
 *
 * $Log: unistd.c,v $
 * Revision 1.3  2000/11/02 11:59:37  iurlano
 * ELF dynamic linking work in progress
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:19  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.1  1999/12/21 13:55:49  pecciant
 * Aggiunti dispositivi a caratteri
 *
 *
 ***************************************************************************/

#include <syscalls.h>
typedef unsigned long int dword;
typedef unsigned long int ssize_t;

ssize_t read(int fd, void *buf, ssize_t count){
  int ret;
  __asm__ ("push %%ebx\n"
	   "movl %%edi,%%ebx\n"
	   "int $0x40\n"
	   "popl %%ebx\n":"=a" (ret):"a" (SYS_READ),"D" (fd),"c" (buf),"d" (count));
  return ret;
}

ssize_t write(int fd, const void *buf, ssize_t count){
  int ret;
  __asm__ ("push %%ebx\n"
	   "movl %%edi,%%ebx\n"
	   "int $0x40\n"
	   "popl %%ebx\n":"=a" (ret):"a" (SYS_WRITE),"D" (fd),"c" (buf),"d" (count));
  return ret;
}



