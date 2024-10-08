/*!	\file arch/i386/kernel/irq.S
 *	\brief IRQ handler routines.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-26
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

.file	"irq.S"

; //! Include common assembler header.
#include <arch/asm.h>

.text

; //! Macro to build a generic interrupt handler routine.
#define INTERRUPT(name, irq) \
	.global _##name ; \
	ALIGN ; \
_##name: \
	pushl	##irq ; \
	jmp	COMMON_ISR ; \

; //! Common interrupt procedure.
; //! Every INTs must call this procedure.
.extern	 default_handler
ALIGN
COMMON_ISR:
	cld
	; // Saving segment registers; get rid of
	; // %gs and %fs registers because the kernel
	; // does not use them.
	; // For the user tasks registers %fs, %gs are
	; // correctly stored into their TSS, so we
	; // have no trouble during a task switch.
	push	%ds
	push	%es
	; // Saving general purpose registers.
	pushl	%eax
	pushl	%ecx
	pushl	%edx
	pushl	%ebx
	pushl	%ebp
	pushl	%esi
	pushl	%edi
	; // Update segment registers with the kernel
	; // data segment.
	movw	$KERNEL_DATA_SEL, %bx
	movw	%bx, %ds
	movw	%bx, %es
	; // Call the default handler;
	; //     void default_handler(irq_context_t *c);
	pushl	%esp
	call	default_handler
.global ret_from_isr
ret_from_isr:
	popl	%esp
	; // Restore general purpose registers.
	popl	%edi
	popl	%esi
	popl	%ebp
	popl	%ebx
	popl	%edx
	popl	%ecx
	popl	%eax
	; // Restore segment registers.
	pop	%es
	pop	%ds
	; // Skip the IRQ number.
	add	$4, %esp
	; // Return from interrupt; restores %eip, %cs and
	; // eflags registers for 0-privileged tasks;
	; // for other tasks also %esp and %ss registers
	; // are restored.
	iretl

; // --- Interrupt Service Routines Declaration ------------------------//

INTERRUPT(irq_00, $0x00)	; //!< Timer
INTERRUPT(irq_01, $0x01)	; //!< Keyboard
INTERRUPT(irq_02, $0x02)	; //!< PIC Slave
INTERRUPT(irq_03, $0x03)	; //!< RS232-(COM2) or Available
INTERRUPT(irq_04, $0x04)	; //!< RS232-(COM1)
INTERRUPT(irq_05, $0x05)	; //!< Available
INTERRUPT(irq_06, $0x06)	; //!< Floppy Disk
INTERRUPT(irq_07, $0x07)	; //!< LPT1
INTERRUPT(irq_08, $0x08)	; //!< CMOS Real Time Clock
INTERRUPT(irq_09, $0x09)	; //!< PIC Master
INTERRUPT(irq_0A, $0x0A)	; //!< Available
INTERRUPT(irq_0B, $0x0B)	; //!< Available
INTERRUPT(irq_0C, $0x0C)	; //!< Mouse PS/2 or Available
INTERRUPT(irq_0D, $0x0D)	; //!< Numeric Coprocessor
INTERRUPT(irq_0E, $0x0E)	; //!< IDE Channel 0 (Master)
INTERRUPT(irq_0F, $0x0F)	; //!< IDE Channel 1 (Slave)

INTERRUPT(irq_unhand, $0xFF)	; //!< Unhandled interrupt
