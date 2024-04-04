/* descriptor.h -- ia32 architecture descriptors definitions */
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

#ifndef IA32_DESCRIPTOR_H
#define IA32_DESCRIPTOR_H

#include <arch-ia32/config.h>

#ifdef LANG_US
#define IDT_SPURIOUS	"spurious interrupt handler"
#elif defined LANG_IT
#define IDT_SPURIOUS	"interrupt handler non impostato"
#endif

#define GDT_ENTRIES				256 // non so quanto mettere qui, perchè la gdt tiene 8192 descrittori, ma sarebbe uno spreco di memoria... (ci basteranno 256 non ti preoccupare :)
#define IDT_ENTRIES				256

#define SELECTOR_GDT				0x0000
#define SELECTOR_LDT				0x0004

#define SEGMENT_TYPE_DATA			0x00000000
#define SEGMENT_TYPE_CODE			0x00000800
#define SEGMENT_TYPE_ACCESSED			0x00000100
#define SEGMENT_TYPE_READABLE			0x00000200
#define SEGMENT_TYPE_WRITABLE			0x00000200
#define SEGMENT_TYPE_EXPANDDOWN			0x00000400
#define SEGMENT_TYPE_CONFORMING			0x00000400

#define GATE_TYPE_LDT				0x00000200
#define GATE_TYPE_16BIT_TSS			0x00000100
#define GATE_TYPE_16BIT_TSS_BUSY		0x00000300
#define GATE_TYPE_16BIT_CALLGATE		0x00000400
#define GATE_TYPE_16BIT_TASKGATE		0x00000500
#define GATE_TYPE_16BIT_INTGATE			0x00000600
#define GATE_TYPE_16BIT_TRAPGATE		0x00000700
#define GATE_TYPE_32BIT_TSS			0x00000900
#define GATE_TYPE_32BIT_TSS_BUSY		0x00000B00
#define GATE_TYPE_32BIT_CALLGATE		0x00000C00
#define GATE_TYPE_32BIT_INTGATE			0x00000E00
#define GATE_TYPE_32BIT_TRAPGATE		0x00000F00
#define GATE_TYPE_64BIT_TSS			GATE_TYPE_32BIT_TSS
#define GATE_TYPE_64BIT_TSS_BUSY		GATE_TYPE_32BIT_TSS_BUSY
#define GATE_TYPE_64BIT_CALLGATE		GATE_TYPE_32BIT_CALLGATE
#define GATE_TYPE_64BIT_INTGATE			GATE_TYPE_32BIT_INTGATE
#define GATE_TYPE_64BIT_TRAPGATE		GATE_TYPE_32BIT_TRAPGATE

#define DESCRIPTOR_TYPE_GATE			0x00000000
#define DESCRIPTOR_TYPE_SEGMENT			0x00001000

#define DESCRIPTOR_PRESENT			0x00008000

#define DESCRIPTOR_64BIT			0x00200000
#define DESCRIPTOR_32BIT			0x00400000
#define DESCRIPTOR_16BIT			0x00000000

#define DESCRIPTOR_GRANULAR			0x00800000

typedef unsigned long long ia32_desc_t;
typedef ia32_desc_t ia32_desc_table[256];

int ia32_gdt_set_desc(unsigned short entry,unsigned long base,unsigned long limit,unsigned int flags,unsigned int rpl);
int ia32_gdt_raw_set_desc(ia32_desc_t *desc,unsigned long base,unsigned long limit,unsigned int flags,unsigned int rpl);
void ia32_gdt_free_desc(unsigned short entry);
unsigned short ia32_gdt_get_free_desc();
int ia32_gdt_init();

int ia32_idt_set_desc(unsigned char num,unsigned short segment,unsigned long offset,unsigned int flags, unsigned int rpl);
unsigned long ia32_idt_get_offset(unsigned char num);
int ia32_idt_raw_set_desc(ia32_desc_t *desc,unsigned char num,unsigned short segment,unsigned long offset,unsigned int flags, unsigned int rpl);
void ia32_idt_free_desc(unsigned char num);
int ia32_idt_init();

/* definite in head.S */
extern ia32_desc_table idt;
extern ia32_desc_table gdt;

#ifdef CONFIG_ARCH_FASTASM
/* funzioni veloci per sbrigarsi */
#define _set_gate(gate_addr,sel,type,dpl,addr) \
__asm__ ("movw %%dx,%%ax\n\t" \
	"movw %0,%%dx\n\t" \
	"movl %%eax,%1\n\t" \
	"movl %%edx,%2" \
	 : \
	 : "i" ((short) (0x8000+(dpl<<13)+(type<<8))), \
	 "o" (*((char *) (gate_addr))), \
	 "o" (*(4+(char *) (gate_addr))), \
	 "d" ((char *) (addr)),"a" (sel << 16))

#define set_intr_gate(n,sel,addr) \
    _set_gate(&idt[n],sel,14,0,addr)    // 14 (1110) = interrupt disabilitati

#define set_system_gate(n,sel,addr) \
    _set_gate(&idt[n],sel,15,3,addr)    // 15 (1111) = interrupt abilitati

#define set_trap_gate(n,sel,addr) \
    _set_gate(&idt[n],sel,15,0,addr)    // 15 (1111) = interrupt abilitati

#define set_call_gate(a,sel,addr) \
	_set_gate(a,sel,12,3,addr)
#endif	/* CONFIG_ARCH_FASTASM */

#endif	/* IA32_DESCRIPTOR_H */
