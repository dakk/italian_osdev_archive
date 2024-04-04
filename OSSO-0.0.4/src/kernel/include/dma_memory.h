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
 *  File: include/dma_memory.h
 *  Function and data for DMA memory management (under 1st MB)
 *****************************************************************************/

/*****************************************************************************
 * $Id: dma_memory.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 *
 * $Log: dma_memory.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.1  2000/11/24 18:25:54  iurlano
 * Added first try of a DMA memory management. Yet to be tested
 *
 *
 *****************************************************************************/

#ifndef __DMA_MEM_DEFS
#define __DMA_MEM_DEFS

#include <types.h>

#define DMA_Memory_Start 0x10000
#define DMA_Memory_End   0x20000
#define DMA_Memory_Size  (DMA_Memory_End-DMA_Memory_Start)
#define DMA_Memory_Block_Size   0x1000 /* 4 K bytes (a page) */
#define DMA_Memory_Block_Number (DMA_Memory_Size/DMA_Memory_Block_Size)


int DMA_Memory_Init(void);
void * DMA_Memory_Get_Block(dword*);
int DMA_Memory_Free_Block(void * address);

#endif


