/* ioapic.c -- IA32 I/O APIC support */
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
#include <arch-ia32/ioapic.h>
#include <arch-ia32/io.h>
#include <kernel.h>

#define IOAPIC_REGSEL(v)			*(unsigned long *)(ioapics[ioapic].address)=v
#define IOAPIC_WINDOW				(*(unsigned long *)(ioapics[ioapic].address+0x10))
#define IOREDTBL0					0x10

#define IOAPIC_MASKED					0x00010000
#define IOAPIC_TRIGGER_LEVEL			0x00008000
#define IOAPIC_TRIGGER_EDGE				0x00000000
#define IOAPIC_POLARITY_HIGH			0x00000000
#define IOAPIC_POLARITY_LOW				0x00002000
#define IOAPIC_DESTINATION_PHYSICAL		0x00000000
#define IOAPIC_DESTINATION_LOGICAL		0x00000800
#define IOAPIC_FIXED					0x00000000
#define IOAPIC_LOWEST					0x00000100
#define IOAPIC_SMI						0x00000200
#define IOAPIC_NMI						0x00000400
#define IOAPIC_INIT						0x00000500
#define IOAPIC_EXTINT					0x00000700

#define IODELAY() {inportb(0x80); inportb(0x80); inportb(0x80); inportb(0x80); } // sparato totalmente a caso

int ia32_ioapic_init(int ioapic) {
	kprint(IOAPIC_LINIT,ioapic);
	ia32_ioapic_disable_all_irqs(ioapic);
	if (system.use_imcr) {
		outportb(0x22,0x70);
		IODELAY();
		outportb(0x23,0x01);
		IODELAY();
	}
	kprint(IOAPIC_OK);
	return 0;
}

int ia32_ioapic_enable_irq(int ioapic,unsigned int irqn,unsigned char vector,int cpu_dest) {
	if (ioapic<0 || (unsigned int)ioapic>=system.num_ioapic)
		return -1;
	if (irqn>IOAPIC_MAXIRQ)
		return -1;
	IOAPIC_REGSEL(IOREDTBL0+irqn*2); IOAPIC_WINDOW=IOAPIC_MASKED;
	IOAPIC_REGSEL(IOREDTBL0+irqn*2+1); IOAPIC_WINDOW=(cpus[cpu_dest].apic_id<<24);
	IOAPIC_REGSEL(IOREDTBL0+irqn*2);
	if (irqn<16)
		IOAPIC_WINDOW=IOAPIC_TRIGGER_EDGE|IOAPIC_POLARITY_HIGH|IOAPIC_DESTINATION_PHYSICAL|IOAPIC_LOWEST|(vector);
	else
		IOAPIC_WINDOW=IOAPIC_TRIGGER_LEVEL|IOAPIC_POLARITY_LOW|IOAPIC_DESTINATION_PHYSICAL|IOAPIC_LOWEST|(vector);
	return 0;
}

int ia32_ioapic_disable_irq(int ioapic,unsigned int irqn) {
	if (ioapic<0 || (unsigned int)ioapic>=system.num_ioapic)
		return -1;
	if (irqn>IOAPIC_MAXIRQ)
		return -1;
	IOAPIC_REGSEL(IOREDTBL0+irqn*2); IOAPIC_WINDOW=IOAPIC_MASKED;
	return 0;
}

int ia32_ioapic_disable_all_irqs(int ioapic) {
	unsigned int i;
	for (i=0; i<IOAPIC_NUMIRQ; i++)
		ia32_ioapic_disable_irq(ioapic,i);
	return 0;
}

int ia32_ioapic_enable_all_irqs(int ioapic,unsigned char *vectors,int *cpu_dests) {
	unsigned int i;
	for (i=0; i<IOAPIC_NUMIRQ; i++)
		ia32_ioapic_enable_irq(ioapic,i,vectors[i],cpu_dests[i]);
	return 0;
}

int ia32_ioapic_eoi(int ioapic,unsigned int irqn)
{
	// boh...!
	/* TODO lol :D */
	return 0;
}
