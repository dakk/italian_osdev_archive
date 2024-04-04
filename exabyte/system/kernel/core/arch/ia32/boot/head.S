/* boot.S -- ia32 arch kernel entry file */
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

#define ASM	1
#include <config.h>
#include <arch-ia32/segment.h>
#include <arch-ia32/multiboot.h>
#include <kernel.h>	/* per le variabili del linker script */

	.text
	.globl	arch_start,_arch_start
	.globl	gdt,idt,gdt_48,idt_48,_gdt,_idt,_gdt_48,_idt_48
	.extern	arch_init,kernel_main

/*
FFFFFFFF	fine memoria		end_mem
C0000000	ebp virtuale immagine	ebp_virt
00100000	ebp fisico immagine	ebp_phys

dati: ebp_virt + relative
factor = ebp_phys - ebp_virt
ebp = ebp_virt + 40100000
*/

/*	
movw	$0x9F45,(0xB8000)	# E
jmp	.		# qui noi non ci dobbiamo finire, init_paging fa il ritorno manuale a paging_enabled
*/

/* entry point del kernel core per architettura ia32 */
arch_start:
_arch_start:
	movl	$0xA0000-4,%esp		# stack temporaneo: 4 byte prima delle rom
	jmp	multiboot_entry
        /* allineamento a 32 bit */
	.align  4

	/* Header Multiboot */
multiboot_header:
        .long   MULTIBOOT_HEADER_MAGIC  /* magic */
        .long   MULTIBOOT_HEADER_FLAGS  /* feature flags */
        .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)/* checksum */

/* queste ce le tengo giusto per sicurezza... */

#if 0 /* non va con questa struttura, non trova _start, _edata, _end... */
#ifndef __ELF__
	/* header_addr */
	.long   multiboot_header
	/* load_addr */
	.long   _start
	/* load_end_addr */
	.long   _edata
	/* bss_end_addr */
	.long   _end
	/* entry_addr */
	.long   multiboot_entry
#endif /* ! __ELF__ */
#endif

multiboot_entry:
	/* inizializza lo stack */
	movl    $(stack + STACK_SIZE - 4),%esp
	/* azzera i flag */
	pushl   $2
	popf
	/* pusho il puntatore alle info multiboot */
	pushl	%ebx
	/* pusho il magic number */
	pushl	%eax
	/* imposta la gdt */
	lgdt	gdt_48
	movw	$KERNEL_DS_SEL,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss
	ljmp	$KERNEL_CS_SEL,$flush
	/* queste per flushare la prefetch :) */
	nop
	nop
flush:
	/* imposta il tss e il task register per lo stack-switching */
	movl	$tss,%eax
	movw	%ax,(ktss + 2)
	shr	$16,%eax
	movb	%al,(ktss + 4)
	movb	%ah,(ktss + 7)
	movw	$KERNEL_TSS_SEL,%ax
	ltr	%ax
	/* imposta una idt iniziale */
	lidt    (idt_48)
	/* resetta il coprocessore (tanto non lo uso :) */
	xorw	%ax, %ax
	outb	%al, $0xf0	# out[0xF0] = 0
	call	delay
	outb	%al, $0xf1	# out[0xF1] = 0
	call	delay
	/* richiamo la funzione di avvio del kernel */
	call	EXT_C(arch_init)
	call	EXT_C(kernel_main)
	/* halt */
loop:   hlt
        jmp     loop

/* area di stack */
.comm	stack, STACK_SIZE
.comm	tss_stack, STACK_SIZE

delay:
	.word   0x00eb      # l' equivalente in intel-asm di jmp $+2
	ret

# eccolo: il tss di turno :S
# non lo usiamo per switchare task, ma ci serve per cambiare lo stack
# privilegiato al momento del richiamo di una system call
tss:
	.word	0, 0			# back link
.globl tss_esp0
tss_esp0:
	.long	tss_stack+STACK_SIZE-4	# ESP0
	.word	KERNEL_DS_SEL, 0	# SS0, reserved

	.long	0			# ESP1
	.word	0, 0			# SS1, reserved

	.long	0			# ESP2
	.word	0, 0			# SS2, reserved

	.long	0			# CR3
	.long	0, 0			# EIP, EFLAGS
	.long	0, 0, 0, 0		# EAX, ECX, EDX, EBX
	.long	0, 0, 0, 0		# ESP, EBP, ESI, EDI
	.word	0, 0			# ES, reserved
	.word	0, 0			# CS, reserved
	.word	0, 0			# SS, reserved
	.word	0, 0			# DS, reserved
	.word	0, 0			# FS, reserved
	.word	0, 0			# GS, reserved
	.word	0, 0			# LDT, reserved
	.word	0, 103			# debug, IO permission bitmap base

gdt:
_gdt:

# Null descriptor
null:	.word	0, 0, 0, 0

# Kernel Code Segment Descriptor
kcode:	.word	0xFFFF	# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0	# base address = 0
	.word	0x9A00	# code read/exec, privilege 0
	.word	0x00CF	# granularity = 4096, 386
			#  (+5th nibble of limit)

# Kernel Data Segment Descriptor
kdata:	.word	0xFFFF	# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0	# base address = 0
	.word	0x9200	# data read/write, privilege 0, expand-up
	.word	0x00CF	# granularity = 4096, 386
			#  (+5th nibble of limit)

# User Code Segment Descriptor
ucode:	.word	0xFFFF	# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0	# base address = 0
	.word	0xFA00	# code read/exec, privilege 3
	.word	0x00CF	# granularity = 4096, 386
			#  (+5th nibble of limit)
	
# User Data Segment Descriptor
udata:	.word	0xFFFF	# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0	# base address = 0
	.word	0xF200	# data read/write, privilege 3, expand-up
	.word	0x00CF	# granularity = 4096, 386
			#  (+5th nibble of limit)

# TSS for Stack Switch
ktss:	.word	103
	.word	0
	.word	0x8900	# ring 0, available, 32-bit
	.word	0

	.fill	250, 8, 0	# riempe per le altre 250 entry
gdt_end:

idt:
_idt:
	.fill	256, 8, 0	# riempe spazio vuoto per 256 entry
idt_end:

idt_48:
_idt_48:
	.word	idt_end-idt-1	# idt limit = 0
	.long	idt		# idt base = 0L

gdt_48:
_gdt_48:
	.word	gdt_end-gdt-1	# gdt limit=[calc], [calc] GDT entries
	/*.word   0x8000		# gdt limit=2048, 256 GDT entries*/
	.long	gdt
