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
 * file : kernel/paging.c                                                  *
 * System code for Paging                                                  *
 ***************************************************************************


 ***************************************************************************
 * $Id: paging.c,v 1.7 2001/04/10 23:10:26 iurlano Exp $
 ***************************************************************************
 *
 * $Log: paging.c,v $
 * Revision 1.7  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.6  2001/03/16 19:22:59  iurlano
 * Added something to the make xconfig prog
 * Now OSSO runs on plex86
 *
 * BIG PROBLEM. OSSO gives random errors about finding INIT and relocating
 *   and doesn't start
 *
 * Revision 1.5  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.4  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.3  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.2  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:12  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.15  1999/12/20 17:33:52  iurlano
 * Added Fork()
 * added some syscalls
 *
 * Revision 1.14  1999/11/25 11:22:31  iurlano
 * Clean in progress
 *
 * Revision 1.13  1999/11/23 12:23:04  iurlano
 * Recreated Makefile
 *
 ***************************************************************************/

#include <paging.h>
#include <printk.h>
#include <fixedmemory.h>
#include <memory.h>
#include <sysinfo.h>

PDE * Kernel_Page_Dir; /* Page Directory Entry */


/*
  This function create a Page Table which maps the physical memory to virtual 
  memory at the same addresses (i.e. the first 32MB are the physical memory)
  if the computer has 32MB of ram
  This could be a problem if the computer has more than 128MB of memory
  because gcc puts its code at these addresses.
  See below.
*/
void init_PDE(PDE * Page_Dir) {
  int i,k;
  int npages; /* Total pages in memory */
  
  PTE * Page_Table;
  npages=memory_size/DIM_PAGE; /* Sets all the physical memory to virtual mem*/

  /* CODE TO BE TESTED: actually it seems to work */
  //  npages=512; /* It doesn't work. Yet to find a solution*/

  /* This sets the pages to be mapped identically from physical to
     virtual memory. Currently it is set with 2 MB limit. */
  /* From the 1st MB to the 2nd MB there is the memory used for managing
     memory (!?!) . Its size depends by the memory size */
  
  //printk("Memory pages =%d\n",npages);
  i=0;
  do {
    Page_Dir[i].Present=1;
    Page_Dir[i].Read_Write=1;
    Page_Dir[i].User=0;
    Page_Dir[i].Write_Through=0;
    Page_Dir[i].Cache_Disable=0;
    Page_Dir[i].Accessed=0;
    Page_Dir[i].Reserved=0;
    Page_Dir[i].Page_4MB=0;
    Page_Dir[i].Global=0;
    Page_Dir[i].Available=0;
    Page_Table=getpagetable();
    //printk("Page Table at : %x\n",Page_Table);
    Page_Dir[i].Page_Table_Base_Addr=((dword)Page_Table)>>12;
    for (k=0;(k<1024) && (npages>0);k++,npages--) {
      Page_Table[k].Present=1;
      Page_Table[k].Read_Write=1;
      Page_Table[k].User=0;
      Page_Table[k].Write_Through=0;
      Page_Table[k].Cache_Disable=0;
      Page_Table[k].Accessed=0;
      Page_Table[k].Reserved=0;
      Page_Table[k].Dirty=0;
      Page_Table[k].Global=0;
      Page_Table[k].Available=0;
      Page_Table[k].Page_Base_Addr=k+i*1024;
    }
   
    if (k<1024) for (;k<1024;k++) {
      Page_Table[k].Present=0;
      Page_Table[k].Read_Write=0;
      Page_Table[k].User=0;
      Page_Table[k].Write_Through=0;
      Page_Table[k].Cache_Disable=0;
      Page_Table[k].Accessed=0;
      Page_Table[k].Reserved=0;
      Page_Table[k].Dirty=0;
      Page_Table[k].Global=0;
      Page_Table[k].Available=0;
      Page_Table[k].Page_Base_Addr=0;
    }
    i++;
  } while (npages>0);
  
  for (;i<1024;i++) {
    Page_Dir[i].Present=0;
    Page_Dir[i].Read_Write=0;
    Page_Dir[i].User=0;
    Page_Dir[i].Write_Through=0;
    Page_Dir[i].Cache_Disable=0;
    Page_Dir[i].Accessed=0;
    Page_Dir[i].Reserved=0;
    Page_Dir[i].Page_4MB=0;
    Page_Dir[i].Global=0;
    Page_Dir[i].Available=0;
    Page_Dir[i].Page_Table_Base_Addr=0;
  }
}

void init_paging(void) {
  dword t;
  Kernel_Page_Dir=(PDE *)KERNEL_Page_Dir_BASE_ADDRESS;
  init_PDE(Kernel_Page_Dir);

  printk("Kernel Page directory Allocated at %d(0x%x)\n",
	 Kernel_Page_Dir,Kernel_Page_Dir);
  __asm__ __volatile__ ("movl %%eax,%%cr3"::"a" (Kernel_Page_Dir));
  /*
    __asm__ ("movl %cr0,%eax\n"
    "orl $0x80000000,%eax\n"
    "movl %eax,%cr0");

  */

  /* Plex86 needed this change */
  __asm__ __volatile__ ("movl %%cr0,%%eax":"=a" (t));
  t|=0x80000000;
  //	   "addl 0x80000000,%eax\n"
  __asm__ __volatile__ ("movl %%eax,%%cr0"::"a" (t));

}

