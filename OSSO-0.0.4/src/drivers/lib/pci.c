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


/************************* Driver's LIB O S S O ****************************
 * file : drivers/lib/pci.c                                                *
 * Routines for pci access                                                 *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: pci.c,v 1.7 2001/05/27 20:22:53 iurlano Exp $
 ***************************************************************************
 *
 * $Log: pci.c,v $
 * Revision 1.7  2001/05/27 20:22:53  iurlano
 * Added experimental driver config feature
 *
 * Revision 1.6  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.5  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2000/10/23 23:33:44  iurlano
 * Remove ebx references from the libOSSO
 *
 * Revision 1.3  2000/08/25 13:39:37  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:02  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#include <syscalls.h>
#include <types.h>
#include <semaphores.h>
#include <drivers/drivers.h>
#include <io.h>

#define PCI_CONF_PORT_INDEX     0xCF8
#define PCI_CONF_PORT_DATA      0xCFC


byte PCI_Read_Byte(int bus,int device,int function,dword regnum) {
  byte b;
  __asm__("int $0x40":"=a" (b):"a" (PCI_READ_BYTE),
	  "c" ((bus<<8)|(device<<3)|(function)),"d" (regnum));
  return b;
}

word PCI_Read_Word(int bus,int device,int function,dword regnum) {
  word w;
  __asm__("int $0x40":"=a" (w):"a" (PCI_READ_WORD),
	  "c" ((bus<<8)|(device<<3)|(function)),"d" (regnum));
  return w;
}
dword PCI_Read_Dword(int bus,int device,int function,dword regnum) {
  dword d;
  __asm__("int $0x40":"=a" (d):"a" (PCI_READ_DWORD),
	  "c" ((bus<<8)|(device<<3)|(function)),"d" (regnum));
  return d;

}


int PCI_Write_Dword(int bus,int device,int function,dword regnum,dword value) {
  dword coords;
  coords=(bus<<8)|(device<<3)|(function);
  outd((0x80000000 |(coords<<8)|(regnum & ~3)),PCI_CONF_PORT_INDEX);
  outd(value,ind(PCI_CONF_PORT_DATA));
  return 0;
}

int PCI_Write_Word(int bus,int device,int function,dword regnum,word value) {
  dword coords;
  coords=(bus<<8)|(device<<3)|(function);
  outd((0x80000000 |(coords<<8)|(regnum & ~3)), PCI_CONF_PORT_INDEX);
  outw(value,inw(PCI_CONF_PORT_DATA + (regnum & 2)));
  return 0;
}

int PCI_Write_Byte(int bus,int device,int function,dword regnum,byte value) {
  dword coords;
  coords=(bus<<8)|(device<<3)|(function);
  outd((0x80000000 |(coords<<8)|(regnum & ~3)), PCI_CONF_PORT_INDEX);
  outb(value,inb(PCI_CONF_PORT_DATA + (regnum & 3)));
  return 0;
}
