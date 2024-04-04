/* irq.c -- ia32 architecture interrupt request */
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

#include <config.h>		// questo va SEMPRE per primo
#include <arch-ia32/smp.h>
#include <arch-ia32/irq.h>
#include <arch-ia32/pic8259.h>
#include <arch-ia32/ioapic.h>
#include <arch-ia32/segment.h>
#include <arch-ia32/descriptor.h>
#include <stddef.h>
#include <kernel.h>

typedef struct irq_t {
	void (*handlers[IRQ_MAXHANDLERS])(unsigned short irq);
	int enabled;
} irq_t;

irq_t irqs[IRQ_MAXNUM];
unsigned int ia32_irq_max=0;
unsigned int ia32_irq_num=0;

extern void asm_irq_0();
extern void asm_irq_1();
extern void asm_irq_2();
extern void asm_irq_3();
extern void asm_irq_4();
extern void asm_irq_5();
extern void asm_irq_6();
extern void asm_irq_7();
extern void asm_irq_8();
extern void asm_irq_9();
extern void asm_irq_10();
extern void asm_irq_11();
extern void asm_irq_12();
extern void asm_irq_13();
extern void asm_irq_14();
extern void asm_irq_15();
extern void asm_irq_16();
extern void asm_irq_17();
extern void asm_irq_18();
extern void asm_irq_19();
extern void asm_irq_20();
extern void asm_irq_21();
extern void asm_irq_22();
extern void asm_irq_23();
extern void asm_irq_24();
extern void asm_irq_25();
extern void asm_irq_26();
extern void asm_irq_27();
extern void asm_irq_28();
extern void asm_irq_29();
extern void asm_irq_30();
extern void asm_irq_31();

int ia32_irq_init(unsigned char *vectors) {
	kprint(IRQ_INIT);
	if (system.num_ioapic>0) {
		ia32_irq_max=IOAPIC_MAXIRQ;
		ia32_irq_num=IOAPIC_NUMIRQ;
	} else {
		ia32_irq_max=PIC_MAXIRQ;
		ia32_irq_num=PIC_NUMIRQ;
	}
	if (ia32_irq_num>IRQ_MAXNUM) {
		ia32_irq_num=IRQ_MAXNUM;
		ia32_irq_max=IRQ_MAXNUM-1;
	}
	unsigned int i,j;
	for (i=0; i<ia32_irq_num; i++) {
		irqs[i].enabled=0;
		for (j=0; j<IRQ_MAXHANDLERS; j++)
			irqs[i].handlers[j]=NULL;
	}
	ia32_idt_set_desc(vectors[0],KERNEL_CS_SEL,(unsigned)asm_irq_0,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[1],KERNEL_CS_SEL,(unsigned)asm_irq_1,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[2],KERNEL_CS_SEL,(unsigned)asm_irq_2,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[3],KERNEL_CS_SEL,(unsigned)asm_irq_3,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[4],KERNEL_CS_SEL,(unsigned)asm_irq_4,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[5],KERNEL_CS_SEL,(unsigned)asm_irq_5,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[6],KERNEL_CS_SEL,(unsigned)asm_irq_6,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[7],KERNEL_CS_SEL,(unsigned)asm_irq_7,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[8],KERNEL_CS_SEL,(unsigned)asm_irq_8,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[9],KERNEL_CS_SEL,(unsigned)asm_irq_9,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[10],KERNEL_CS_SEL,(unsigned)asm_irq_10,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[11],KERNEL_CS_SEL,(unsigned)asm_irq_11,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[12],KERNEL_CS_SEL,(unsigned)asm_irq_12,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[13],KERNEL_CS_SEL,(unsigned)asm_irq_13,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[14],KERNEL_CS_SEL,(unsigned)asm_irq_14,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[15],KERNEL_CS_SEL,(unsigned)asm_irq_15,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[16],KERNEL_CS_SEL,(unsigned)asm_irq_16,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[17],KERNEL_CS_SEL,(unsigned)asm_irq_17,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[18],KERNEL_CS_SEL,(unsigned)asm_irq_18,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[19],KERNEL_CS_SEL,(unsigned)asm_irq_19,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[20],KERNEL_CS_SEL,(unsigned)asm_irq_20,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[21],KERNEL_CS_SEL,(unsigned)asm_irq_21,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[22],KERNEL_CS_SEL,(unsigned)asm_irq_22,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[23],KERNEL_CS_SEL,(unsigned)asm_irq_23,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[24],KERNEL_CS_SEL,(unsigned)asm_irq_24,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[25],KERNEL_CS_SEL,(unsigned)asm_irq_25,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[26],KERNEL_CS_SEL,(unsigned)asm_irq_26,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[27],KERNEL_CS_SEL,(unsigned)asm_irq_27,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[28],KERNEL_CS_SEL,(unsigned)asm_irq_28,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[29],KERNEL_CS_SEL,(unsigned)asm_irq_29,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[30],KERNEL_CS_SEL,(unsigned)asm_irq_30,GATE_TYPE_32BIT_INTGATE,0);
	ia32_idt_set_desc(vectors[31],KERNEL_CS_SEL,(unsigned)asm_irq_31,GATE_TYPE_32BIT_INTGATE,0);
	kprint(IRQ_OK,ia32_irq_num);
	return 0;
}

int ia32_irq_set_handler(unsigned int irqn,void *handler) {
	if (irqn>ia32_irq_max)
		return -1;
	unsigned int i;
	int first_free=-1;
	for (i=0; i<IRQ_MAXHANDLERS; i++) {
		if (irqs[irqn].handlers[i]==handler)
			return 0;
		if (irqs[irqn].handlers[i]==NULL && first_free==-1)
			first_free=i;
	}
	if (first_free<0)
		return -1;
	irqs[irqn].handlers[first_free]=handler;
	return 0;
}

int ia32_irq_unset_handler(unsigned int irqn,void *handler) {
	if (irqn>ia32_irq_max)
		return -1;
	unsigned int i;
	for (i=0; i<IRQ_MAXHANDLERS; i++) {
		if (irqs[irqn].handlers[i]==handler) {
			irqs[irqn].handlers[i]=NULL;
			return 0;
		}
	}
	return 0;
}

int ia32_irq_enable(unsigned int irqn) {
	if (irqn>ia32_irq_max)
		return -1;
	irqs[irqn].enabled=1;
	return 0;
}

int ia32_irq_disable(unsigned int irqn) {
	if (irqn>ia32_irq_max)
		return -1;
	irqs[irqn].enabled=0;
	return 0;
}

void ia32_irq_eoi(unsigned int irqn,int ioapic) {
	if (irqn>ia32_irq_max)
		return;
	if (system.num_ioapic>0)
		ia32_ioapic_eoi(ioapic,irqn);
	else
		ia32_pic8259_eoi(irqn);
}

void ia32_irq_handler(unsigned int irqn,int ioapic) {
	(*(char *)(0xB809F))++;
	if (irqn>ia32_irq_max) {
		kprint(IRQ_INVALID,irqn);
		return;
	}
	if (irqs[irqn].enabled==0) {
		ia32_irq_eoi(irqn,ioapic);
		return;
	}
	unsigned int i;
	for (i=0; i<IRQ_MAXHANDLERS; i++)
		if (irqs[irqn].handlers[i]!=NULL)
			(*irqs[irqn].handlers[i])(irqn);
	ia32_irq_eoi(irqn,ioapic);
}
