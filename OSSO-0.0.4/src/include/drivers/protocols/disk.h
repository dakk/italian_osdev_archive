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


/********************** Driver's protocols O S S O *************************
 * file : drivers/lib/include/protocols/disk.h                             *
 * Disk driver protocol                                                    *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: disk.h,v 1.3 2001/08/22 14:19:26 iurlano Exp $
 ***************************************************************************
 *
 * $Log: disk.h,v $
 * Revision 1.3  2001/08/22 14:19:26  iurlano
 * Added partition support to the FAT driver. Tested with hda3 and hda4 and seems working
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2001/04/20 15:30:59  iurlano
 * Changes in to the fat driver
 * creating the filesystem protocol
 *
 * Revision 1.3  2001/03/02 17:56:12  iurlano
 * changed an ld flag in the kernel Makefile. COULD BE A PROBLEM!
 * some reordering
 *
 * Revision 1.2  2000/08/25 13:39:38  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.1.1.1  2000/05/30 08:11:03  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#ifndef __DISK_PROTOCOL
#define __DISK_PROTOCOL

#include <types.h>
//#include "vfs.h"

/* COMMANDS */

#define DSK_CMD_READ_SECTOR       1  /* Logical Sector Read */
#define DSK_CMD_WRITE_SECTOR      2  /* Logical Sector Write */

typedef struct __PartTableRec {
  byte indicator;
  byte start_head;
  byte start_sector;
  byte start_track;
  byte os;
  byte end_head;
  byte end_sector;
  byte end_track;
  dword Preceeding_Sectors;
  dword Length;
} PartTableRec;

typedef struct __DISK_COMMAND {
  dword arg1,arg2; /* arg1 is the device number (currently 0 only) */
  /* arg2 is the absolute sector number from the start of disk */
  byte command;
  byte tag; 
  /* Needed to identify a command if executed out of order */
  /* Currently tag is not used */
  /* it is byte for alignement matters */
  word buffer_dim; /* Buffer follows immediately */
  word next_cmd; /* 0 if last */
} DISK_COMMAND;

#endif
