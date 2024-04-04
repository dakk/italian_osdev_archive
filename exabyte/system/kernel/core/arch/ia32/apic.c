/* apic.c -- Advanced Programmable Interrupt Controllers support */
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
#include <arch-ia32/msr.h>
#include <arch-ia32/apic.h>
#include <arch-ia32/smp.h>
#include <arch-ia32/string.h>
#include <arch-ia32/arch.h>
#include <arch-ia32/io.h>
#include <kernel.h>

#define APIC_ID(b)				(*(unsigned long *)(b+0x0020))
#define APIC_VERSION(b)			(*(unsigned long *)(b+0x0030))
#define APIC_TPR(b)				(*(unsigned long *)(b+0x0080))
#define APIC_EOI(b)				(*(unsigned long *)(b+0x00B0))
#define APIC_LDR(b)				(*(unsigned long *)(b+0x00D0))
#define APIC_DFR(b)				(*(unsigned long *)(b+0x00E0))
#define APIC_SVR(b)				(*(unsigned long *)(b+0x00F0))
#define APIC_ICR0(b)			(*(unsigned long *)(b+0x0300))
#define APIC_ICR1(b)			(*(unsigned long *)(b+0x0310))
#define APIC_LVT_TIMER(b)		(*(unsigned long *)(b+0x0320))
#define APIC_LVT_THERMAL(b)		(*(unsigned long *)(b+0x0330))
#define APIC_LVT_PERF(b)		(*(unsigned long *)(b+0x0340))
#define APIC_LVT_LINT0(b)		(*(unsigned long *)(b+0x0350))
#define APIC_LVT_LINT1(b)		(*(unsigned long *)(b+0x0360))
#define APIC_LVT_ERROR(b)		(*(unsigned long *)(b+0x0370))
#define APIC_TIMER_ICR(b)		(*(unsigned long *)(b+0x0380))
#define APIC_TIMER_CCR(b)		(*(unsigned long *)(b+0x0390))
#define APIC_TIMER_DCR(b)		(*(unsigned long *)(b+0x03E0))

#define LVT_FIXED				0x00000000
#define LVT_SMI					0x00000200
#define LVT_NMI					0x00000400
#define LVT_INIT				0x00000500
#define LVT_EXTINT				0x00000700
#define LVT_TRIGGER_EDGE		0x00000000
#define LVT_TRIGGER_LEVEL		0x00008000
#define LVT_MASKED				0x00010000
#define LVT_ONESHOT				0x00000000
#define LVT_PERIODIC			0x00020000

#define SVR_ENABLE				0x00000100

#define DFR_FLAT				0xFFFFFFFF
#define DFR_CLUSTER				0x0FFFFFFF

#define ICR_FIXED						0x00000000
#define ICR_LOWPRIORITY					0x00000100
#define ICR_SMI							0x00000200
#define ICR_NMI							0x00000400
#define ICR_INIT						0x00000500
#define ICR_START						0x00000600
#define ICR_DEST_LOGICAL				0x00000800
#define ICR_STATUS_IDLE					0x00000000
#define ICR_STATUS_PENDING				0x00001000
#define ICR_DEASSERT					0x00000000
#define ICR_ASSERT						0x00004000
#define ICR_TRIGGER_EDGE				0x00000000
#define ICR_TRIGGER_LEVEL				0x00008000
#define ICR_DESTINATION_SELF			0x00040000
#define ICR_DESTINATION_ALL_WITH_SELF	0x00080000
#define ICR_DESTINATION_ALL_NOT_SELF	0x000C0000

#define DCR_DIVIDE_1					0x0000000B
#define DCR_DIVIDE_2					0x00000000
#define DCR_DIVIDE_4					0x00000001
#define DCR_DIVIDE_8					0x00000002
#define DCR_DIVIDE_16					0x00000003
#define DCR_DIVIDE_32					0x00000008
#define DCR_DIVIDE_64					0x00000009
#define DCR_DIVIDE_128					0x0000000A

void ia32_micro_delay(unsigned long n) { // TEMPORANEEEEOOOOO! (SPARATO A CASO)
	unsigned long i,j;
	for (i=0; i<n; i++)
		for (j=0; j<1000; j++);
}

void ia32_apic_init(unsigned int cpu) {
	unsigned long vers=APIC_VERSION(cpus[cpu].apic_base);
	cpus[cpu].apic_version=vers&0xFF;
	cpus[cpu].apic_lvts=(vers>>16)&0xFF;
	kprint(APIC_INFO,cpus[cpu].apic_id,cpus[cpu].apic_version);
	if (cpus[cpu].apic_version<0x10)
		kprint(APIC_EXTERNAL);
	else if (cpus[cpu].apic_version==0x14)
		kprint(APIC_P4);
	else if (cpus[cpu].apic_version<0x20)
		kprint(APIC_LAPIC);
	else
		kprint(APIC_UNKNOWN);
	kprint(APIC_LVTS,cpus[cpu].apic_lvts);
	// inizializzo l'apic
	APIC_TPR(cpus[cpu].apic_base)=0x20;
	APIC_LVT_TIMER(cpus[cpu].apic_base)=LVT_MASKED|SMP_TIMER_INTERRUPT;
	APIC_LVT_LINT0(cpus[cpu].apic_base)=LVT_EXTINT|LVT_TRIGGER_LEVEL|SMP_LINT0_INTERRUPT;
	APIC_LVT_LINT1(cpus[cpu].apic_base)=LVT_NMI|SMP_LINT1_INTERRUPT;
	APIC_LVT_ERROR(cpus[cpu].apic_base)=LVT_MASKED|SMP_ERROR_INTERRUPT;
	APIC_LVT_PERF(cpus[cpu].apic_base)=LVT_MASKED|SMP_PERF_INTERRUPT;
	APIC_LVT_THERMAL(cpus[cpu].apic_base)=SMP_THERMAL_INTERRUPT; // lo abilito, non si sa mai :)
	APIC_SVR(cpus[cpu].apic_base)=SVR_ENABLE|0x0F;
	// ripeto i lint per sicurezza
	APIC_LVT_LINT0(cpus[cpu].apic_base)=LVT_EXTINT|LVT_TRIGGER_LEVEL|SMP_LINT0_INTERRUPT;
	APIC_LVT_LINT1(cpus[cpu].apic_base)=LVT_NMI|SMP_LINT1_INTERRUPT;
	APIC_DFR(cpus[cpu].apic_base)=DFR_FLAT;
	kprint(APIC_INITOK,cpu);
}

void ia32_apic_boot_ap(unsigned int cpu,unsigned char boot_page) {
	kprint(APIC_BOOTING,cpu);
	outportb(0x70,0x0F);
	outportb(0x71,0x0A);
	*(unsigned short *)(0x0467)=(boot_page*0x1000); // ??? non sono per niente sicuro
	*(unsigned short *)(0x0469)=0;
	// sendo un init
	APIC_ICR1(cpus[system.bsp_cpu_num].apic_base)=cpus[cpu].apic_id<<24;
	APIC_ICR0(cpus[system.bsp_cpu_num].apic_base)=ICR_INIT|ICR_ASSERT;
	ia32_micro_delay(100);
	// deasserto l'init
	APIC_ICR1(cpus[system.bsp_cpu_num].apic_base)=cpus[cpu].apic_id<<24;
	APIC_ICR0(cpus[system.bsp_cpu_num].apic_base)=ICR_INIT;
	ia32_micro_delay(100);
	// sendo lo start
	APIC_ICR1(cpus[system.bsp_cpu_num].apic_base)=cpus[cpu].apic_id<<24;
	APIC_ICR0(cpus[system.bsp_cpu_num].apic_base)=ICR_START|ICR_ASSERT|(boot_page&0xFF);
	ia32_micro_delay(100);
}

void ia32_apic_setup_timer(unsigned int cpu,unsigned long ticks) {
	APIC_LVT_TIMER(cpus[cpu].apic_base)=LVT_MASKED;
	APIC_TIMER_DCR(cpus[cpu].apic_base)=DCR_DIVIDE_1;
	APIC_TIMER_ICR(cpus[cpu].apic_base)=ticks;
	APIC_LVT_TIMER(cpus[cpu].apic_base)=LVT_PERIODIC|SMP_TIMER_INTERRUPT;
}

inline unsigned long ia32_apic_get_timer(unsigned int cpu) {
	return APIC_TIMER_CCR(cpus[cpu].apic_base);
}

void ia32_apic_eoi(unsigned int cpu) {
	APIC_EOI(cpus[cpu].apic_base)=0;
}
