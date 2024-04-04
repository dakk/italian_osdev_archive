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
 * file : kernel/parttable.c                                               *
 * Functions for collecting partition table's data                         *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: parttable.c,v 1.1.1.1 2000/05/30 08:11:12 iurlano Exp $
 ***************************************************************************
 *
 * $Log: parttable.c,v $
 * Revision 1.1.1.1  2000/05/30 08:11:12  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.3  1999/11/25 11:22:31  iurlano
 * Pulizia e riordino
 *
 * Revision 1.2  1999/10/21 15:39:02  iurlano
 * Aggiustamento commenti
 * Aggiunta header e .c per la paginazione
 *
 ***************************************************************************/

#include <types.h>
#include <kernel.h>
#include <parttable.h>

#include <printk.h>

Partition Partitions[4];

void read_partition_table(byte * settore) {
  byte * temp;
  int i;
  temp=settore+0x1BE;
  for (i=0;i<4;i++) {
    Partitions[i].bootflag=*temp++;
    Partitions[i].start_head=*temp++;
    Partitions[i].start_sec=*temp & 63; 
    Partitions[i].start_cyl=((*temp++ << 2) & 0x300) |*temp++;
    Partitions[i].system=*temp++; 
    Partitions[i].end_head=*temp++;
    Partitions[i].end_sec=*temp & 63; 
    Partitions[i].end_cyl=((*temp++ << 2) & 0x300) |*temp++;
    Partitions[i].relative_start_sector=*((dword * )temp);
    temp+=4;
    Partitions[i].sector_number=*((dword * )temp);
    temp+=4;
  }
}

void print_partition_table(void) {
  int i;
  printk("BOOT SYSTEM START C:H:S END C:H:S SEC_START SEC_LENGTH\n");
  for (i=0;i<4;i++) {
    printk("%u %u %u:%u:%u %u:%u:%u %u %u\n",Partitions[i].bootflag,
	   Partitions[i].system,Partitions[i].start_cyl,
	   Partitions[i].start_head,Partitions[i].start_sec,
	   Partitions[i].end_cyl,Partitions[i].end_head,Partitions[i].end_sec,
	   Partitions[i].relative_start_sector,Partitions[i].sector_number);
  }
}

