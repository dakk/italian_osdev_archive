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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/interrupts.h,v $
 * Descrizione: Prototipi e definizioni per la gestione delle interruzioni
 ***************************************************************************

 ***************************************************************************
 * $Id: interrupts.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: interrupts.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:06  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:40  pecciant
 *
 *
 * Revision 1.2  1999/11/25 13:08:55  pecciant
 * Standardizzati gli header dei .h
 *
 *
 ***************************************************************************/

#ifndef __INTERRUPTS_DEFS
#define __INTERRUPTS_DEFS

#include <types.h>

#define IRQ_0 (word) (1)
#define IRQ_1 ((word) 1 << 1)
#define IRQ_2 ((word) 1 << 2)
#define IRQ_3 ((word) 1 << 3)
#define IRQ_4 ((word) 1 << 4)
#define IRQ_5 ((word) 1 << 5)
#define IRQ_6 ((word) 1 << 6)
#define IRQ_7 ((word) 1 << 7)
#define IRQ_8 ((word) 1 << 8)
#define IRQ_9 ((word) 1 << 9)
#define IRQ_A ((word) 1 << 10)
#define IRQ_B ((word) 1 << 11)
#define IRQ_C ((word) 1 << 12)
#define IRQ_D ((word) 1 << 13)
#define IRQ_E ((word) 1 << 14)
#define IRQ_F ((word) 1 << 15)

#define ALL_IRQS (0xFFFF)

void init_IDT_table(void);
void set_IRQ_mask(word mask);
word get_IRQ_mask(void);
void unmask_IRQ(word irq);
void mask_IRQ(word irq);


#endif



