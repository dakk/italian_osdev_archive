/* physical.h */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#ifndef MM_PHYSICAL_H
#define MM_PHYSICAL_H

void mm_physical_init();
void mm_physical_reserve(unsigned long start,unsigned long size);
unsigned long mm_physical_alloc_page();
void mm_physical_free_page(unsigned long pointer);

unsigned long mm_physical_get_free_mem();
unsigned long mm_physical_get_used_mem();
unsigned long mm_physical_get_free_dma_mem();
unsigned long mm_physical_get_used_dma_mem();

#endif	/* MM_PHYSICAL_H */
