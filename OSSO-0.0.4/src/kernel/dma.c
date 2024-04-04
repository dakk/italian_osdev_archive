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
 * file : kernel/dma.c                                                     *
 * DMA functions                                                           *
 ***************************************************************************

 ***************************************************************************
 * $Id: dma.c,v 1.2 2001/08/18 16:22:46 iurlano Exp $
 ***************************************************************************
 *
 * $Log: dma.c,v $
 * Revision 1.2  2001/08/18 16:22:46  iurlano
 * WORK on fdc and dma. Still no success ...
 *
 * Revision 1.1  2001/08/15 20:06:36  iurlano
 * Added DMA management functions into the kernel and library
 *
 *
 ***************************************************************************/

#include <io.h>
#include <sys.h>
#include <printk.h>
#include <semaphore.h>
#include <process.h>
#include <panic.h>


#define SELECT_CH_0    0
#define SELECT_CH_1    1
#define SELECT_CH_2    2
#define SELECT_CH_3    3

#define CLEAR_MASK     0
#define SET_MASK       4

#define VERIFY_TRANSFER  0
#define WRITE_TRANSFER   4
#define READ_TRANSFER    8

#define AUTOINITIALIZED      0
#define NON_AUTOINITIALIZED  16

#define ADDR_INCR_SEL    32

#define DEMAND_MODE      0
#define SINGLE_MODE     64
#define BLOCK_MODE     128
#define CASCADE_MODE   192


#define DMA_MODE_READ   0x44    /* I/O to memory, no autoinit, 
				   increment, single mode */
#define DMA_MODE_WRITE  0x48    /* memory to I/O, no autoinit, 
				   increment, single mode */
#define DMA_MODE_CASCADE 0xC0   /* pass thru DREQ->HRQ, DACK<-HLDA only */

#define DMA_CHANNELS   8

typedef struct __T_DMA_CHANNEL {
  int used;
  PID pid;
} T_DMA_CHANNEL;

Bool_Semaphore DMA_MUTEX; /* Semaphore for accessing DMA_Channels data */

T_DMA_CHANNEL DMA_CHANNEL[DMA_CHANNELS];

byte MaskReg[8]   = { 0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4 };
byte ModeReg[8]   = { 0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6 };
byte ClearReg[8]  = { 0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8 };

byte PagePort[8]  = { 0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A };
byte AddrPort[8]  = { 0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC };
byte CountPort[8] = { 0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE };


void InitDMA(void) {
  int i;
  for (i=0;i<DMA_CHANNELS;i++){
    DMA_CHANNEL[i].used=0;
    DMA_CHANNEL[i].pid=-1;
  }
  DMA_CHANNEL[4].used=1;
  Init_Bool_Semaphore(&DMA_MUTEX,1);
}

int DMA_Request_Channel(int channel) {
  if ((channel<0) || (channel>=DMA_CHANNELS))
    return -1;
  Bool_P(&DMA_MUTEX);
  if (DMA_CHANNEL[channel].used==0) {
    DMA_CHANNEL[channel].used=1;
    DMA_CHANNEL[channel].pid=Processo_In_Esecuzione->pid;
    Bool_V(&DMA_MUTEX);
    return 0;
  }
  Bool_V(&DMA_MUTEX);
  return -1;
}

int DMA_Free_Channel(int channel) {
  if ((channel<0) || (channel>=DMA_CHANNELS))
    return -1;
  Bool_P(&DMA_MUTEX);
  if ((DMA_CHANNEL[channel].used==1)&&
      (DMA_CHANNEL[channel].pid==Processo_In_Esecuzione->pid)) {
    DMA_CHANNEL[channel].used=0;
    DMA_CHANNEL[channel].pid=-1;
    Bool_V(&DMA_MUTEX);
    return 0;
  }
  Bool_V(&DMA_MUTEX);
  return -1;

}


/* 0 == OK ; -1 == ERROR */
int StartDMA(dword Channel,dword mode,byte * buffer,word length) {

  printk("DMA OP req: Channel %d, mode %x, Buffer %x, length %d\n",Channel,mode,buffer,length);
  Channel&=0xFF;
  mode&=0xFF;
  mode|=Channel;
  length--;
  Bool_P(&DMA_MUTEX);
  if ((DMA_CHANNEL[Channel].used==1)&&
      (DMA_CHANNEL[Channel].pid==Processo_In_Esecuzione->pid)) {
    Bool_V(&DMA_MUTEX);

    if (!(mode&Channel)) panic("ERROR IN MODE!!!!\n");
    else {
      cli();
      outb(Channel|SET_MASK,MaskReg[Channel]);
      outb(0,ClearReg[Channel]);
      outb(mode,ModeReg[Channel]);
      outb(((dword)buffer)&0xFF,AddrPort[Channel]);
      outb((((dword)buffer)>>8)&0xFF,AddrPort[Channel]);
      outb(((dword)buffer>>16)&0xF,PagePort[Channel]);

      outb(length&0xFF,CountPort[Channel]);
      outb(length>>8,CountPort[Channel]);
 
      outb(Channel|CLEAR_MASK,MaskReg[Channel]);

      sti();
    }
    return 0;
  }
  Bool_V(&DMA_MUTEX);
  return -1;
}


