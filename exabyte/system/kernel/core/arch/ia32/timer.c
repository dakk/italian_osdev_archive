/* timer.c */
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

#include <config.h>
#include <arch-ia32/arch.h>
#include <arch-ia32/timer.h>
#include <arch-ia32/io.h>
#include <arch-ia32/cpuid.h>
#include <arch-ia32/smp.h>
#include <arch-ia32/descriptor.h>
#include <arch-ia32/segment.h>
#include <arch-ia32/spinlock.h>
#include <arch-ia32/apic.h>
#include <arch-ia32/irq.h>
#include <kernel.h>

extern void asm_timer_test();

static unsigned long timer_spinlock=0;
static unsigned long prev_ticks=0;
static unsigned long avg_ticks=0;
static unsigned long num_ticks=0;
static unsigned int num_tests=0;
static int measure_bus;
static unsigned long apic_ticks_1=0;
static unsigned long apic_ticks_2=0;

// scandaloso, senza questa funzione non va... :|

unsigned long div(unsigned long op1,unsigned long op2) {
	unsigned long i=0;
	long tmp=(long)op1;
	if (op2>op1)
		return 0;
	while (tmp>0) {
		tmp-=op2;
		i++;
	}
	return i;
}

// in questa funz uso gli spinlock in modo un po' strano... :)

void ia32_timer_init() {
	unsigned long eax,ebx,ecx,edx;
	ia32_cpuid(1,&eax,&ebx,&ecx,&edx);
	if (!(edx&(1<<4))) {
		system.use_tsc=0;
		kprint("ia32: time stamp counter not implemented\n");
		return;
	}
	system.use_tsc=1;
	kprint("ia32: measuring clock frequency... ");
	unsigned char timer_int=ia32_smp_get_int_by_irq(0);
	static unsigned long old_offset=0;
	old_offset=ia32_idt_get_offset(timer_int);
	//ia32_pit_set_frequency(100);
	ia32_idt_set_desc(timer_int,KERNEL_CS_SEL,(unsigned)asm_timer_test,GATE_TYPE_32BIT_INTGATE,0);
	ia32_spinlock_acquire(&timer_spinlock);
	num_tests=9; // = 0.5 sec
	measure_bus=0;
	sti();
	ia32_spinlock_acquire(&timer_spinlock);
	cli();
	cpus[system.bsp_cpu_num].speed=(avg_ticks/num_ticks)/54925;
	kprint("%u mhz\n",cpus[system.bsp_cpu_num].speed);
	if (system.use_apic) {
		kprint("ia32: measuring bus speed... ");
		ia32_spinlock_release(&timer_spinlock);
		ia32_spinlock_acquire(&timer_spinlock);
		measure_bus=1;
		num_tests=0;
		ia32_apic_setup_timer(system.bsp_cpu_num,0xFFFFFFFF);
		sti();
		ia32_spinlock_acquire(&timer_spinlock);
		cli();
		measure_bus=0; // temp
		unsigned long bus_speed=(apic_ticks_2-apic_ticks_1);
		system.bus_ticks=bus_speed;
		system.bus_speed=div(bus_speed,54925L);
		kprint("%u mhz\n",system.bus_speed);
		kprint("ia32: initializing apic timer... ");
		ia32_apic_setup_timer(system.bsp_cpu_num,system.bus_ticks);
		kprint("ok\n");
	} else
		system.bus_speed=0;
	ia32_idt_set_desc(timer_int,KERNEL_CS_SEL,old_offset,GATE_TYPE_32BIT_INTGATE,0);
}

void ia32_timer_test(unsigned long eax,unsigned long edx) {
	if (measure_bus) {
		unsigned long ticks=ia32_apic_get_timer(system.bsp_cpu_num);
		ia32_irq_eoi(0,0);
		switch (num_tests) {
		case 0:
			apic_ticks_1=0xFFFFFFFF-ticks;
			break;
		case 1:
			apic_ticks_2=0xFFFFFFFF-ticks;
			ia32_spinlock_release(&timer_spinlock);
			break;
		};
		num_tests++;
	} else {
		ia32_irq_eoi(0,0);
		num_tests--;
		unsigned long ticks=eax-prev_ticks;
		if (prev_ticks!=0) {
			unsigned long old_avg_ticks=avg_ticks;
			avg_ticks+=ticks;
			if (old_avg_ticks>avg_ticks) {
				avg_ticks=ticks;
				num_ticks=1;
			} else
				num_ticks++;
		}
		if (num_tests<=0)
			ia32_spinlock_release(&timer_spinlock);
		prev_ticks=eax;
	}
}
