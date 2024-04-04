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


/*************************** LIB O S S O ***********************************
 * file : lib/include/memory.h                                             *
 * headers for memory.c                                                    *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: memory.h,v 1.11 2001/06/26 21:00:41 iurlano Exp $
 ***************************************************************************
 *
 * $Log: memory.h,v $
 * Revision 1.11  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.10  2001/05/27 20:22:53  iurlano
 * Added experimental driver config feature
 *
 * Revision 1.9  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.8  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2001/04/26 22:09:09  iurlano
 * fat work (again)
 * working snapshot ?
 *
 * Revision 1.3  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:20  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.2  2000/03/07 21:37:52  iurlano
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.1  1999/12/25 19:59:42  iurlano
 * *** empty log message ***
 *
 * Revision 1.1  1999/12/17 22:46:06  iurlano
 *
 ***************************************************************************/


#include <types.h>

void * mallosso(unsigned long dimensione);
void freeosso(void * ptr);

typedef struct {
  dword total;
  dword num_pages;
  dword mem_pages;
  dword swap_pages;
  dword free_pages;  
} meminfo;    /* structure for returning memory info */

int get_free_pages(void);
meminfo * get_mem_info(void);

void bzero(byte *buf,unsigned long int count);
void *memset(void *s, int c, unsigned int n);




