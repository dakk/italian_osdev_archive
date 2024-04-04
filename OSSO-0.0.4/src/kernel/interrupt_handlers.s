 #
 # OSSO - A Micro Kernel OS
 # Copyright (c) 2000 Alessandro Iurlano.
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation; either version 2, or (at your option)
 # any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program; if not, write to the Free Software
 # Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 #  02111-1307, USA.		
 # 

	
.globl irq0_handler, irq1_handler, irq2_handler, irq3_handler, irq4_handler, irq5_handler, irq6_handler, irq7_handler, irq8_handler, irq9_handler, irqA_handler, irqB_handler, irqC_handler, irqD_handler, irqE_handler, irqF_handler

irq0_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs
	
	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call scheduler

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

	
irq1_handler:
	cli
	pusha	
	push %ds
	push %es
	push %fs
	push %gs
	
	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ1
	
	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	sti
	iret

	
irq2_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ2

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

irq3_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ3

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret
	
irq4_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ4

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret
	
irq5_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ5

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

irq6_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ6

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

irq7_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ7

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret
	
irq8_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ8

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

irq9_handler:
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQ9

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

irqA_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQA

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret


irqB_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQB

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

irqC_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQC

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

irqD_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQD

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret

irqE_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQE

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret
	
irqF_handler:	
	pusha
	push %ds
	push %es
	push %fs
	push %gs

	movl $0x10,%eax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%gs
	movw %ax,%fs
	call IRQF

	pop %gs
	pop %fs
	pop %es
	pop %ds

	popa
	iret
