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
 * file : kernel/gdt.c                                                     *
 * GDT Management functions                                                *
 ***************************************************************************

 ***************************************************************************
 * $Id: gdt.c,v 1.4 2000/11/30 22:47:15 iurlano Exp $
 ***************************************************************************
 *
 * $Log: gdt.c,v $
 * Revision 1.4  2000/11/30 22:47:15  iurlano
 * Changed gdt handling.
 * Removed LDT for each task and added a global LDT for USER LEVEL tasks
 *
 * Revision 1.3  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.2  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:08  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.7  2000/01/04 00:56:50  iurlano
 * bug fixes
 *
 * Revision 1.6  1999/11/30 11:38:07  iurlano
 * Aggiunto semaforo a gdt.c
 * Corrected initialization of the semaphore of memory.c
 *
 ***************************************************************************/



#include <types.h>
#include <sys.h>
#include <gdt.h>
#include <fixedmemory.h>
#include <semaphore.h>
#include <panic.h>

SegmentDes * GDT;

static int gdt_count; /* free entries in table */
static int gdt_size;  /* Total entries in table */

const SegmentDes NULLDES={0,0,0,0,0}; /* Null Descriptor */

static Bool_Semaphore Gdt_Mutex; /* Mutual Exclusion for gdt data */

void free_gdt_entry(dword idx) {
  Bool_P(&Gdt_Mutex);
  GDT[idx].access&=0xEF; /* Cleared SD_OSAvail flag */
  gdt_count++;
  Bool_V(&Gdt_Mutex);
}


int get_free_gdt_entries(void) {
  return gdt_count;
}


dword prenote_gdt_entry(void) { /* 0 == NOT FOUND FREE ENTRY */
  int i;
  Bool_P(&Gdt_Mutex);
  for (i=LAST_USED_GDT_ENTRY+1;i<gdt_size;i++) {
    if ((GDT[i].access&SD_OSAvail)==0) { // SD_OSAvail flag = 0 -> free entry
      GDT[i].access|=SD_OSAvail;
      gdt_count--;
      Bool_V(&Gdt_Mutex);
      return i;
    }
  }  
  Bool_V(&Gdt_Mutex);
  panic("gdt free entries exausted!");
  return 0;
}

SegmentDes GLOBAL_LDT[2];

void init_gdt(int numdes) {
  GDT=(SegmentDes * )GDT_BASE_ADDRESS;
  /* Documentation reccomends the first descriptor to be null */
  GDT[NULL_SEGMENT_DES]=PackDescriptor(0,0,0,0,0,0);
  GDT[KERNEL_CODE_SEGMENT_DES]=PackDescriptor(0,0xFFFFF,SD_Type_Exec_Read,0,
					      SD_App|SD_Present,
					      SD_Size_32|SD_4k_Size);

  GDT[KERNEL_DATA_SEGMENT_DES]=PackDescriptor(0,0xFFFFF,SD_Type_Data_RW,0,
					      SD_App|SD_Present,
					      SD_Size_32|SD_4k_Size);
  GDT[NULL_SEGMENT_DES].access|=SD_OSAvail;
  GDT[KERNEL_CODE_SEGMENT_DES].access|=SD_OSAvail;
  GDT[KERNEL_DATA_SEGMENT_DES].access|=SD_OSAvail;

  GLOBAL_LDT[USER_CODE_SEGMENT_DES]=PackDescriptor(0,0xFFFFF,SD_Type_Exec_Read,3,
			       SD_App|SD_Present,SD_Size_32|SD_4k_Size);

  GLOBAL_LDT[USER_DATA_SEGMENT_DES]=PackDescriptor(0,0xFFFFF,SD_Type_Data_RW,3,
			       SD_App|SD_Present,SD_Size_32|SD_4k_Size);

  /*
    GDT[SUPERVISOR_LDT_SEGMENT_DES]=PackDescriptor((dword) (&GLOBAL_LDT),
    sizeof(SegmentDes),
    SD_Type_LDT,0,SD_Present,SD_Size_32);
  */
  GDT[USER_LDT_SEGMENT_DES]=PackDescriptor((dword) (&GLOBAL_LDT),
					   2*sizeof(SegmentDes),
					   SD_Type_LDT,3,SD_Present,SD_Size_32);

  //  GDT[SUPERVISOR_LDT_SEGMENT_DES].access|=SD_OSAvail;
  GDT[USER_LDT_SEGMENT_DES].access|=SD_OSAvail;


  load_gdt((char *)GDT,numdes);
  gdt_count=numdes-6;
  gdt_size=numdes;
  Init_Bool_Semaphore(&Gdt_Mutex,1);
}



