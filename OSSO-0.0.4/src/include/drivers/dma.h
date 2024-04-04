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


/****************************** O S S O  ***********************************
 * file : drivers/lib/include/dma.h
 * Definitions of dma functions
 ***************************************************************************

 ***************************************************************************
 * $Id: dma.h,v 1.2 2001/08/18 16:22:46 iurlano Exp $
 ***************************************************************************
 *
 * $Log: dma.h,v $
 * Revision 1.2  2001/08/18 16:22:46  iurlano
 * WORK on fdc and dma. Still no success ...
 *
 * Revision 1.1  2001/08/15 20:06:36  iurlano
 * Added DMA management functions into the kernel and library
 *
 *
 ***************************************************************************/

#ifndef __DMA_DEFS_DRV_LIB
#define __DMA_DEFS_DRV_LIB


#include <types.h>



#define DMA_MODE_READ   0x44    /* I/O to memory, no autoinit, 
				   increment, single mode */
#define DMA_MODE_WRITE  0x48    /* memory to I/O, no autoinit, 
				   increment, single mode */
#define DMA_MODE_CASCADE 0xC0   /* pass thru DREQ->HRQ, DACK<-HLDA only */



int DMA_Op(dword Channel,dword mode,byte * buffer,word length);
int DMA_Request_Channel(int channel);
int DMA_Free_Channel(int channel);

#endif
