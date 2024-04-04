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
 * file : drivers/lib/io.c                                                 *
 * I/O functions, 8 & 16 bits sized                                        *
 ***************************************************************************

 ***************************************************************************
 * $Id: io.c,v 1.3 2000/10/17 22:56:38 iurlano Exp $
 ***************************************************************************
 *
 * $Log: io.c,v $
 * Revision 1.3  2000/10/17 22:56:38  iurlano
 * io changes (added outd & ind)
 * printc expansion (added %ld & %lx in format)
 * work on rtl8139
 *
 * Revision 1.2  2000/08/25 13:39:37  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.1.1.1  2000/05/30 08:11:02  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#include <types.h>

byte inb(word port) { 
  byte _v; 
  __asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); 
  return _v;
}

word inw(word port) { 
  word _v; 
  __asm__ volatile ("inw %%dx,%%ax":"=a" (_v):"d" (port)); 
  return _v;
}

word ind(word port) { 
  dword _v; 
  __asm__ volatile ("inl %%dx,%%eax":"=a" (_v):"d" (port)); 
  return _v;
}

void outb(byte value, word port) {
  __asm__ ("outb %%al,%%dx"::"a" (value),"d" (port));
}

void outw(word value, word port) {
  __asm__ ("outw %%ax,%%dx"::"a" (value),"d" (port));
  return ;
}

void outd(dword value, word port) {
  __asm__("outl %%eax,%%dx"::"a" (value),"d" (port));
  return ;
}
