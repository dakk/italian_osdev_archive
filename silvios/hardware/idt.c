/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <8259.h>
#include <gdt.h>
#include <idt.h>
#include <stdio.h>
#include <video.h>
#include <task.h>

extern void int_null();

desc_table(IDT, 256)
{
};

struct {
	unsigned short limit __attribute__ ((packed));
	union DT_entry *idt __attribute__ ((packed));
} loadidt = {
(256 * sizeof(union DT_entry) - 1), IDT};


void InitIDT()
{
	int count = 0;

	for (count = 16; count < 256; count++)
		set_vector(int_null, count, D_PRESENT + D_INT + D_DPL3);

	set_vector(_int0, 0, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int1, 1, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int2, 2, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int3, 3, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int4, 4, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int5, 5, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int6, 6, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int7, 7, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int8, 8, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int9, 9, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int10, 10, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int12, 12, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int13, 13, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int14, 14, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int15, 15, D_PRESENT + D_INT + D_DPL3);
	set_vector(_int16, 16, D_PRESENT + D_INT + D_DPL3);

	debug("\nvettori settati");

	asm("lidt (%0)                 \n"	/* Load the IDT                */
	    "pushfl                    \n"	/* Clear the NT flag           */
      "andl $0xffffbfff,(%%esp)  \n" "popfl                     \n":
      :    "r"((char *) &loadidt)
		);
	asm("sti");
}


void set_vector(void *handler, unsigned char interrupt,
		unsigned short control_major)
{
	unsigned short codesegment = 0x08;
	asm volatile ("movw %%cs,%0":"=g" (codesegment));

	IDT[interrupt].gate.offset_low =
		(unsigned short) (((unsigned long) handler) & 0xffff);
	IDT[interrupt].gate.selector = codesegment;
	IDT[interrupt].gate.access = control_major;
	IDT[interrupt].gate.offset_high =
		(unsigned short) (((unsigned long) handler) >> 16);
}

asm(".globl int_null        \n" "int_null:              \n" "   iret                \n"	/* Exit interrupt                   */
	);

void panic(void)
{
	set_text_color(ROSSO + LAMPEGGIO);
	kprint("\nKERNEL PANIC:   ");
	return;
}

void _int0()
{
	panic();
	kprint("int0: Divisione per 0\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int1()
{
	panic();
	kprint("int1: Eccezione di debug\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int2()
{
	panic();
	kprint("int2: errore sconosciuto\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int3()
{
	panic();
	kprint("int3: Breakpoint\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int4()
{
	panic();
	kprint("int4: Overflow\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int5()
{
	panic();
	kprint("int 5: Bounds check\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int6()
{
	panic();
	kprint("int6: opcode invalido\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int7()
{
	panic();
	kprint("int7: Copprocessore non avviabile\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int8()
{
	panic();
	kprint("int8: doppio fallimento :°°(\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int9()
{
	panic();
	kprint("int9: Coprocessor Segment overrun\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int10()
{
	panic();
	kprint("int10: Invalid TSS\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int11()
{
	panic();
	kprint("int11: Segment not present\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int12()
{
	panic();
	kprint("int12: Stack exception\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int13()
{
	panic();
	kprint("int13: General Protection\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int14()
{
	unsigned int addr;
	panic();
	__asm__ volatile ("movl  %%cr2, %0":"=r" (addr));
	kprint("int14: un accesso all'indirizzo %xh (%bB)ha causato un Page fault\n", addr, addr);
	getch();
	get_curtask_status();

	while (1);
}

void _int15()
{
	panic();
	kprint("int15: errore sconosciuto\n");
	getch();
	get_curtask_status();
	while (1);
}

void _int16()
{
	panic();
	kprint("int16: Coprocessor error\n");
	getch();
	get_curtask_status();
	while (1);
}
