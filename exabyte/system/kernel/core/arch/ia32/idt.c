/* idt.c -- ia32 architecture interrupt descriptor table */
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
#include <arch-ia32/descriptor.h>
#include <arch-ia32/segment.h>
#include <arch-ia32/config.h>
#include <panic.h>

/* setta una entry nella idt */
int ia32_idt_set_desc(unsigned char num,unsigned short segment,unsigned long offset,unsigned int flags, unsigned int rpl)
{
	if ((!segment) || (rpl > 3)) return -1;
	unsigned long *low_dword=(unsigned long *)&idt[num];
	unsigned long *hi_dword=(unsigned long *)((unsigned)&idt[num]+sizeof(unsigned long));
	*low_dword=(offset&0xFFFF)+(segment<<16);
	*hi_dword=(flags|DESCRIPTOR_PRESENT|DESCRIPTOR_TYPE_GATE)+((rpl&0x03)<<13)+(offset&0xFFFF0000);
	return 0;
}

unsigned long ia32_idt_get_offset(unsigned char num) {
	if (num>=IDT_ENTRIES)
		return 0;
	unsigned long *low_dword=(unsigned long *)&idt[num];
	unsigned long *hi_dword=(unsigned long *)((unsigned)&idt[num]+sizeof(unsigned long));
	unsigned long offset=((*low_dword)&0xFFFF)|((*hi_dword)&0xFFFF0000);
	return offset;
}

int ia32_idt_raw_set_desc(ia32_desc_t *desc,unsigned char num,unsigned short segment,unsigned long offset,unsigned int flags, unsigned int rpl) {
	unsigned long *low_dword=(unsigned long *)desc;
	unsigned long *hi_dword=(unsigned long *)((unsigned)desc+sizeof(unsigned long));
	*low_dword=(offset&0xFFFF)+(segment<<16);
	*hi_dword=(flags|DESCRIPTOR_PRESENT|DESCRIPTOR_TYPE_GATE)+((rpl&0x03)<<13)+(offset&0xFFFF0000);
	return 0;
}

/* cancella una entry nella idt */
void ia32_idt_free_desc(unsigned char num) {
	idt[num]=0;
}

void ia32_idt_spurious_handler() {
	panic(IDT_SPURIOUS);
}

/* inizializzazione idt */
int ia32_idt_init() {
#ifndef CONFIG_ARCH_HEAD_TABLES
	unsigned int i;
	// cancello l'idt
	for (i=0; i<IDT_ENTRIES; i++)
		idt[i]=0;
	// setto l'idtr
	unsigned int address[2];
	address[0]=0xFFFFFFFF;
	address[1]=(unsigned)&idt;
	asm volatile ("lidt (%0)"::"g"((void *)address+2));
#endif
	unsigned int j;
	for (j=0x20; j<IDT_ENTRIES; j++)
		ia32_idt_set_desc(j,KERNEL_CS_SEL,(unsigned)ia32_idt_spurious_handler,GATE_TYPE_32BIT_INTGATE,0);
	return 0;
}
