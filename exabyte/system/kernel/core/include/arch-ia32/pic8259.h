/* pic8259.h -- ia32 architecture pic8259 irq chip definitions */
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

#ifndef PIC8259_H
#define PIC8259_H

#define PIC_MAXIRQ	15		// irq massimo: 15
#define PIC_NUMIRQ	PIC_MAXIRQ + 1	// numero di irq massimi :/

#define PIC_MASTER	0x20	// porta del pic master (IRQ0-IRQ7)
#define PIC_SLAVE	0xA0	// porta del pic slave (IRQ8-IRQ15)

#define PIC_EOI		0x20	// End Of Interrupt
#define PIC_ICW1	0x11	// 10001 - ICW4 will be sent, cascade mode, vector size: 8 bytes, edge triggered mode, sending ICW1
#define PIC_ICW4	0x01	// 00001 - 8086/8088 mode

int ia32_pic8259_init(void);
int ia32_pic8259_enable_irq(unsigned int nr);
int ia32_pic8259_disable_irq(unsigned int nr);
int ia32_pic8259_disable_all_irqs();
int ia32_pic8259_enable_all_irqs();
int ia32_pic8259_eoi(unsigned int irqn);

#endif
