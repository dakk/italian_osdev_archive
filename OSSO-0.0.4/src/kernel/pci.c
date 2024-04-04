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


/************************************ OSSO ***********************************
 *  File: kernel/pci.c                                                       *
 *  PCI Functions                                                            *
 *****************************************************************************

 *****************************************************************************
 * $Id: pci.c,v 1.2 2000/11/21 23:03:08 iurlano Exp $
 *****************************************************************************
 *
 * $Log: pci.c,v $
 * Revision 1.2  2000/11/21 23:03:08  iurlano
 * Added pci.h with prototypes for pci
 * Added prototype of boot loader wrote with bcc
 * Added bootfiles version modified by steros
 *
 * Revision 1.1  2000/11/09 14:30:32  iurlano
 * Added pci.c module for accessing pci with configuration mechanism 1
 *    without BIOS
 *
 *
 *****************************************************************************/

// This should NOT be part of the kernel. It's here for testing purpose

#include <PnP.h>
#include <string.h>
#include <printk.h>
#include <constants.h>
#include <io.h>

#define PCI_CONF_PORT_INDEX     0xCF8
#define PCI_CONF_PORT_DATA      0xCFC


dword PCI_Read_Dword(dword coords,dword regnum) {
  outd((0x80000000 |(coords<<8)|(regnum & ~3)),PCI_CONF_PORT_INDEX);

  return ind(PCI_CONF_PORT_DATA);
}

word PCI_Read_Word(dword coords,dword regnum) {
  outd((0x80000000 |(coords<<8)|(regnum & ~3)), PCI_CONF_PORT_INDEX);

  return inw(PCI_CONF_PORT_DATA + (regnum & 2));
}

byte PCI_Read_Byte(dword coords,dword regnum) {
  outd((0x80000000 |(coords<<8)|(regnum & ~3)), PCI_CONF_PORT_INDEX);

  return inb(PCI_CONF_PORT_DATA + (regnum & 3));
}


int PCI_Write_Dword(dword coords,dword regnum,dword value) {
  outd((0x80000000 |(coords<<8)|(regnum & ~3)),PCI_CONF_PORT_INDEX);
  outd(value,ind(PCI_CONF_PORT_DATA));
  return 0;
}

int PCI_Write_Word(dword coords,dword regnum,word value) {
  outd((0x80000000 |(coords<<8)|(regnum & ~3)), PCI_CONF_PORT_INDEX);
  outw(value,inw(PCI_CONF_PORT_DATA + (regnum & 2)));
  return 0;
}

int PCI_Write_Byte(dword coords,dword regnum,byte value) {
  outd((0x80000000 |(coords<<8)|(regnum & ~3)), PCI_CONF_PORT_INDEX);
  outb(value,inb(PCI_CONF_PORT_DATA + (regnum & 3)));
  return 0;
}

