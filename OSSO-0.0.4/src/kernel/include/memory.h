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


/*********************************** OSSO ************************************
 *  File: $Source: /cvsroot/osso/OSSO/src/kernel/include/memory.h,v $
 *  Function and data for paged memory management
 *****************************************************************************/

/*****************************************************************************
 * $Id: memory.h,v 1.3 2001/06/26 21:00:41 iurlano Exp $
 *
 * $Log: memory.h,v $
 * Revision 1.3  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.6  2000/12/07 15:00:10  marcopex
 * *** empty log message ***
 *
 * Revision 1.5  2000/12/07 11:07:43  marcopex
 * *** empty log message ***
 *
 * Revision 1.4  2000/12/05 20:59:47  iurlano
 * Thread work in progress. Yet to make first complete subsystem
 *
 * Revision 1.3  2000/12/05 15:54:19  marcopex
 * Added linear allocations functions for process
 *
 * Revision 1.2  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.1.1.1  2000/05/30 08:11:06  iurlano
 * OSSO : First Public Release
 *
 *****************************************************************************/

#ifndef __MEM_DEFS
#define __MEM_DEFS


#include <types.h>
#include <process.h>
#include <paging.h>
#include <fixedmemory.h>

typedef char page[DIM_PAGE];
typedef dword mem_size;

typedef struct {
  dword numpages;
  dword offset;
}proc_mem;     /* data structure for the process */

typedef struct {
  dword total;
  dword num_pages;
  dword mem_pages;
  dword swap_pages;
  dword free_pages;  
} meminfo;    /* structure for returning memory info */

/*********** Exported Functions ************/

/* Kernel Reserved */

void meminit(void);

page * getpage(void);
PDE  * getpagedirectory(void);
PTE  * getpagetable(void);
void   freepage(page *);

dword Linear_alloc(pid_t processo, size_t dimensione, int livello); 
int   Linear_free (pid_t processo, dword indirizzo, size_t dimensione); 

void check_memory_size(void);
extern dword memory_size;

/* public */
void * mallosso(mem_size dimensione); /* malloc */
void freosso(void *); /* free */
meminfo * getmeminfo(void);

#endif



