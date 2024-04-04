/*
 * util.c
 * 
 * Assorted IRQ/DMA utility functions for DJGPP 2.01
 * 
 * Copyright (C) 1998  Fabian Nunez
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * The author can be reached by email at: fabian@cs.uct.ac.za
 * 
 * or by airmail at: Fabian Nunez
 *                   10 Eastbrooke
 *                   Highstead Road
 *                   Rondebosch 7700
 *                   South Africa
 */

//#include <assert.h>
//#include <go32.h>
//#include <dos.h>     /* disable,enable */
#include "util.h"

/* definition of DMA channels */
const static DmaChannel dmainfo[] = {
   { 0x87, 0x00, 0x01 },
   { 0x83, 0x02, 0x03 },
   { 0x81, 0x04, 0x05 },
   { 0x82, 0x06, 0x07 }
};

/* if handler != NULL then set new one and save old, else restore old */
void set_irq_handler(int irq,void (*handler)(void),IrqSave *irqsave)
{
   int picbase,irqstate;
   
   if (irq == 0x1c) {
      /* kludge to allow use of int 1ch in DOS */
      picbase = 0;
   } else {
      /* get INT base of IRQ's */
      if (irq < 8) {
	 picbase = _go32_info_block.master_interrupt_controller_base;
      } else {
	 irq -= 8;
	 picbase = _go32_info_block.slave_interrupt_controller_base;
      }
   }
   
   /* disable interrupts */
   irqstate = __dpmi_get_and_disable_virtual_interrupt_state();
   
   if (handler) {
      /* setting new handler */
      _go32_dpmi_get_protected_mode_interrupt_vector(picbase + irq,
						     &irqsave->oldhdlr);
      irqsave->newhdlr.pm_offset = (unsigned long)handler;
      irqsave->newhdlr.pm_selector = _go32_my_cs();
      _go32_dpmi_allocate_iret_wrapper(&irqsave->newhdlr);
      _go32_dpmi_set_protected_mode_interrupt_vector(picbase + irq,
						     &irqsave->newhdlr);
   } else {
      /* restoring old handler */
      _go32_dpmi_set_protected_mode_interrupt_vector(picbase + irq,
						     &irqsave->oldhdlr);
      _go32_dpmi_free_iret_wrapper(&irqsave->newhdlr);
   }

   /* re-enable interrupts if necessary */
   __dpmi_get_and_set_virtual_interrupt_state(irqstate);
}

/* 
 * this allocates a buffer in the < 1M range, maps it and returns the linear
 * address, also setting the physical in the integer pointed at
 */
long alloc_dma_buffer(int size)
{
   _go32_dpmi_seginfo seginfo;
   long begin,end;

   /* loop until allocated mem does not cross a 64K boundary */
   do {
      seginfo.size = (size + 15) / 16;  /* translate to paragraphs */
      assert(_go32_dpmi_allocate_dos_memory(&seginfo) == 0);
      begin = seginfo.rm_segment << 4;
      end = begin + seginfo.size;
   } while((end & 0xffff) < (begin & 0xffff));
   
   return begin;
}

/*
 * this sets up a DMA trasfer between a device and memory.  Pass the DMA
 * channel number (0..3), the physical address of the buffer and transfer
 * length.  If 'read' is TRUE, then transfer will be from memory to device,
 * else from the device to memory.
 */
void dma_xfer(int channel,long physaddr,int length,BOOL read)
{
   long page,offset;
   
   assert(channel < 4);
   
   /* calculate dma page and offset */
   page = physaddr >> 16;
   offset = physaddr & 0xffff;
   length -= 1;  /* with dma, if you want k bytes, you ask for k - 1 */

   disable();  /* disable irq's */
   
   /* set the mask bit for the channel */
   outb(0x0a,channel | 4);
   
   /* clear flipflop */
   outb(0x0c,0);
   
   /* set DMA mode (write+single+r/w) */
   outb(0x0b,(read ? 0x48 : 0x44) + channel);
   
   /* set DMA page */
   outb(dmainfo[channel].page,page);
   
   /* set DMA offset */
   outb(dmainfo[channel].offset,offset & 0xff);  /* low byte */
   outb(dmainfo[channel].offset,offset >> 8);    /* high byte */
   
   /* set DMA length */
   outb(dmainfo[channel].length,length & 0xff);  /* low byte */
   outb(dmainfo[channel].length,length >> 8);    /* high byte */
   
   /* clear DMA mask bit */
   outb(0x0a,channel);
   
   enable();  /* enable irq's */
}
