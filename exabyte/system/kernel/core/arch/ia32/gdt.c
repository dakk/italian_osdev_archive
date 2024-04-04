/* gdt.c -- ia32 architecture global descriptor table */
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

#include <arch-ia32/descriptor.h>
#include <arch-ia32/segment.h>
#include <arch-ia32/config.h>

int ia32_gdt_set_desc(unsigned short entry,unsigned long base,unsigned long limit,unsigned int flags,unsigned int rpl) {
	unsigned int num=entry>>3;
	unsigned long *low_dword=(unsigned long *)&gdt[num];
	unsigned long *hi_dword=(unsigned long *)((unsigned)&gdt[num]+sizeof(unsigned long));
	if(rpl > 3) return -1;
	limit>>=12;
	*low_dword=(limit&0xFFFF)+((base&0xFFFF)<<16);
	*hi_dword=((base>>16)&0xFF)+(flags|DESCRIPTOR_TYPE_SEGMENT|DESCRIPTOR_PRESENT)+((rpl&0x03)<<13)+(limit&0xF0000)+(base&0xFF000000);
	return 0;
}

int ia32_gdt_raw_set_desc(ia32_desc_t *desc,unsigned long base,unsigned long limit,unsigned int flags,unsigned int rpl) {
	unsigned long *low_dword=(unsigned long *)desc;
	unsigned long *hi_dword=(unsigned long *)((unsigned)desc+sizeof(unsigned long));
	if(rpl > 3) return -1;
	limit>>=12;
	*low_dword=(limit&0xFFFF)+((base&0xFFFF)<<16);
	*hi_dword=((base>>16)&0xFF)+(flags|DESCRIPTOR_TYPE_SEGMENT|DESCRIPTOR_PRESENT)+((rpl&0x03)<<13)+(limit&0xF0000)+(base&0xFF000000);
	return 0;
}

void ia32_gdt_free_desc(unsigned short entry) {
	unsigned int num=entry>>3;
	gdt[num]=0;
}

unsigned short ia32_gdt_get_free_desc() {
	unsigned int i;
	for (i=1; i<GDT_ENTRIES; i++) {
		unsigned long *hi_dword=(unsigned long *)((unsigned)&gdt[i]+sizeof(unsigned long));
		if (!((*hi_dword)&DESCRIPTOR_PRESENT))
			return (i<<3);
	}
	return 0;
}

int ia32_gdt_init() {
#ifndef CONFIG_ARCH_HEAD_TABLES
	unsigned int i;
	// cancello la gdt
	for (i=0; i<GDT_ENTRIES; i++)
		gdt[i]=0;
	// setuppo i segmenti di default
	ia32_gdt_set_desc(KERNEL_CS,0,0xFFFFFFFF,SEGMENT_TYPE_CODE|SEGMENT_TYPE_READABLE|DESCRIPTOR_32BIT|DESCRIPTOR_GRANULAR,0);
	ia32_gdt_set_desc(KERNEL_DS,0,0xFFFFFFFF,SEGMENT_TYPE_DATA|SEGMENT_TYPE_WRITABLE|DESCRIPTOR_32BIT|DESCRIPTOR_GRANULAR,0);
	ia32_gdt_set_desc(USER_CS,0,0xFFFFFFFF,SEGMENT_TYPE_CODE|SEGMENT_TYPE_READABLE|SEGMENT_TYPE_CONFORMING|DESCRIPTOR_32BIT|DESCRIPTOR_GRANULAR,3);
	ia32_gdt_set_desc(USER_DS,0,0xFFFFFFFF,SEGMENT_TYPE_DATA|SEGMENT_TYPE_WRITABLE|DESCRIPTOR_32BIT|DESCRIPTOR_GRANULAR,3);
	// setto il gdtr
	unsigned int address[2];
	address[0]=0xFFFFFFFF;
	address[1]=(unsigned)&gdt;
	asm volatile ("lgdt (%0)"::"g"((void *)address+2));
	// updato i selettori
	asm volatile (
		"pushl %0\n"
		"pushl $new_segments\n"
		"lret\n"
		"new_segments:\n"
		"mov %1,%%eax\n"
		"mov %%ax,%%ds\n"
		"mov %%ax,%%es\n"
		"mov %%ax,%%fs\n"
		"mov %%ax,%%gs\n"
		"mov %%ax,%%ss\n"
		:: "g"(KERNEL_CS),"g"(KERNEL_DS));
#endif
	return 0;
}
