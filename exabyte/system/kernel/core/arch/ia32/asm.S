/* asm.S -- ia32 interrupts assembly functions */
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

/* FUNZIONI PER IRQ E SYSCALL */

#define ASM	1
#include <config.h>
#include <arch-ia32/segment.h>
#include <kernel.h>

#define ENTRY(name) \
	.global name; \
    name:

#define SAVE_ALL	\
	cld;          \
	pushl	%es;  \
	pushl	%ds;  \
	pushl	%eax; \
	pushl	%ebp; \
	pushl	%edi; \
	pushl	%esi; \
	pushl	%edx; \
	pushl	%ecx; \
	pushl	%ebx; \

	pushl	%edx	/* salva edx */
	movl	$KERNEL_DS_SEL,%edx; \
	movl	%edx,%ds; \
	movl	%edx,%es; \
	movl	$USER_DS_SEL,%edx; \
	movl	%edx, %fs;
	popl	%edx	/* ripristina edx */

#define RESTORE_ALL \
    popl %ebx;  \
    popl %ecx;  \
    popl %edx;  \
    popl %esi;  \
    popl %edi;  \
    popl %ebp;  \
    popl %eax;  \
    popl %ds;   \
    popl %es;   \
    iret;

/*
#define IRQ_DEBUG(n) \
	.globl _asm_irq_##n;			\
		_asm_irq_##n:			\
	.globl asm_irq_##n			;\
		asm_irq_##n:			;\
		cli					;\
		hlt					;\
		_loopzzz##n:			;\
		jmp	_loopzzz##n			;
*/

#define IRQ(n) \
ENTRY(_asm_irq_##n)				\
ENTRY(asm_irq_##n)				\
    	SAVE_ALL;				\
	/* TODO: maschera IRQ */		\
	pushl	$0;				\
	pushl	$##n;				\
	call	EXT_C(ia32_irq_handler);	\
	popl %eax;					\
	popl %eax;					\
	RESTORE_ALL;

IRQ(0)
IRQ(1)
IRQ(2)
IRQ(3)
IRQ(4)
IRQ(5)
IRQ(6)
IRQ(7)
IRQ(8)
IRQ(9)
IRQ(10)
IRQ(11)
IRQ(12)
IRQ(13)
IRQ(14)
IRQ(15)
IRQ(16)
IRQ(17)
IRQ(18)
IRQ(19)
IRQ(20)
IRQ(21)
IRQ(22)
IRQ(23)
IRQ(24)
IRQ(25)
IRQ(26)
IRQ(27)
IRQ(28)
IRQ(29)
IRQ(30)
IRQ(31)

.globl _asm_timer_test;
	_asm_timer_test:
.globl asm_timer_test;
	asm_timer_test:
	cld;
	pushl %eax;
	pushl %edx;
	rdtsc;
	pushl	%es;
	pushl	%ds;
	pushl	%ebp;
	pushl	%edi;
	pushl	%esi;
	pushl	%ecx;
	pushl	%ebx;
	pushl	%edx;
	movl	$KERNEL_DS_SEL,%edx;
	movl	%edx,%ds;
	movl	%edx,%es;
	movl	%edx,%fs;
	movl	%edx,%gs;
	pushl	%eax;
	call	EXT_C(ia32_timer_test);
	popl	%eax;
	popl	%eax;
	popl	%ebx;
	popl	%ecx;
	popl	%esi;
	popl	%edi;
	popl	%ebp;
	popl	%ds;
	popl	%es;
	popl	%edx;
	popl	%eax;
	iret;

ENTRY(_asm_smp_timer_handler)
ENTRY(asm_smp_timer_handler)
    	SAVE_ALL;
	call	EXT_C(ia32_smp_timer_handler);
	RESTORE_ALL;

ENTRY(_asm_smp_lint0_handler)
ENTRY(asm_smp_lint0_handler)
    	SAVE_ALL;
	call	EXT_C(ia32_smp_lint0_handler);
	RESTORE_ALL;

ENTRY(_asm_smp_lint1_handler)
ENTRY(asm_smp_lint1_handler)
    	SAVE_ALL;
	call	EXT_C(ia32_smp_lint1_handler);
	RESTORE_ALL;

ENTRY(_asm_smp_error_handler)
ENTRY(asm_smp_error_handler)
    	SAVE_ALL;
	call	EXT_C(ia32_smp_error_handler);
	RESTORE_ALL;

ENTRY(_asm_smp_perf_handler)
ENTRY(asm_smp_perf_handler)
    	SAVE_ALL;
	call	EXT_C(ia32_smp_perf_handler);
	RESTORE_ALL;

ENTRY(_asm_smp_thermal_handler)
ENTRY(asm_smp_thermal_handler)
    	SAVE_ALL;
	call	EXT_C(ia32_smp_thermal_handler);
	RESTORE_ALL;
