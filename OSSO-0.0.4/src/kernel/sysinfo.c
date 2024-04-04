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
 * file : kernel/sysinfo.c                                                 *
 * Information gathering functions                                         *
 ***************************************************************************

 ***************************************************************************
 * $Id: sysinfo.c,v 1.1.1.1 2000/05/30 08:11:17 iurlano Exp $
 ***************************************************************************
 *
 * $Log: sysinfo.c,v $
 * Revision 1.1.1.1  2000/05/30 08:11:17  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.3  1999/12/19 23:33:50  iurlano
 * Aggiunto P non bloccante.
 * Aggiunto recmsg bloccante e non.
 * Tolta funzione di rilevazione del memory size da sysinfo e messa in memory.c
 * Aggiunto un semaforo intero che conta i messaggi nella coda
 *
 * Revision 1.2  1999/11/25 11:22:32  iurlano
 * Pulizia e riordino
 *
 *
 ***************************************************************************/

#include <types.h>

#include <kernel.h>
#include <printk.h>

word serial_ports[4];
word parallel_ports[3];
int serial_ports_number;
int parallel_ports_number;
word EBDA_segment;
word BIOS_InstalledHW;

void get_IO_ports_address(void) {
  word * addr;
  int i;

  addr=(word *)0x400;
  i=0;
  while (*addr!=0) {
    serial_ports[i++]=*addr;
    addr++;
  }
  serial_ports_number=i;
  addr=(word *)0x408;
  i=0;
  while (*addr!=0) {
    parallel_ports[i++]=*addr;
    addr++;
  }
  parallel_ports_number=i;

  addr=(word *)0x410;
  BIOS_InstalledHW=*addr;
}


void check_BIOS_32(void) {
  char * index;
  index=(char*)0xE0000;
  while ((int )index < 0x100000) {
    if (*index=='_' && *(index+1)=='3' && *(index+2)=='2' && *(index+3)=='_')
      printk("BIOS32 service found at address : %x\n",index); 
    index+=16;
  }
}






