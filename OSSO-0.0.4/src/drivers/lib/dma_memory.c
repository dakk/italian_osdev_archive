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
 * file : drivers/lib/dma_memory.c
 * Routines for DMA memory access
 ***************************************************************************
 
 ***************************************************************************
 * $Id: dma_memory.c,v 1.4 2001/04/28 12:15:52 steros Exp $
 ***************************************************************************
 *
 * $Log: dma_memory.c,v $
 * Revision 1.4  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.3  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.2  2000/11/26 23:34:59  iurlano
 * work in progress on DMA memory
 *
 * Revision 1.1  2000/11/24 18:25:54  iurlano
 * Added first try of a DMA memory management. Yet to be tested
 *
 *
 ***************************************************************************/

#include <syscalls.h>
#include <types.h>
#include <drivers/drivers.h>

void * DMA_Memory_Get_Block(dword * physical_address) {
  dword d;
  __asm__("int $0x40":"=a" (d):"a" (SYS_GET_DMA_MEM), "c" (physical_address));
  return (void*)d;
}

int DMA_Memory_Free_Block(void * address) {
  dword d;
  __asm__("int $0x40":"=a" (d):"a" (SYS_FREE_DMA_MEM),"c"((dword)address));
  return d;
}
