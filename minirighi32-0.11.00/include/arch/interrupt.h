/*!	\file include/arch/interrupt.h
 *	\brief Interrupt manager header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-03
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <const.h>
#include <arch/i386.h>

// --- IRQ ------------------------------------------------------------ //

//! Total number of IRQ.
#define IRQ_NR		16

//! Timer IRQ line.
#define TIMER_IRQ	0x00

//! Keyboard IRQ line.
#define KEYBOARD_IRQ	0x01

//! Serial COM1 IRQ line.
#define RS232_1_IRQ	0x04

//! Serial COM2 IRQ line.
#define RS232_2_IRQ	0x03

//! Floppy IRQ line.
#define FLOPPY_IRQ	0x06

// **************** Luca Giovacchini (Ide) ****************
//! IDE primary channel IRQ line.
#define IDEP_IRQ	0x0E

//! IDE secondary channel IRQ line.
#define IDES_IRQ	0x0F
// ********************************************************

// --- Reserved Interrupt --------------------------------------------- //

//! Minirighi interrupt (used for the system calls).
#define MINIRIGHI_INT	0x80
//! DOS interrupt (maybe will be used for DOS emulation...)
#define DOS_INT		0x20

// --- EFLAGS --------------------------------------------------------- //

//! Interrupt enable flag in EFLAGS register.
#define EFLAGS_IF	0x200

//! I/O privilege level 3 (minimum privilege).
//! Everyone can perform I/O operation.
#define EFLAGS_IOPL3	0x3000

//! I/O privilege level 2.
#define EFLAGS_IOPL2	0x2000

//! I/O privilege level 1.
#define EFLAGS_IOPL1	0x1000

//! I/O privilege level 0 (maximum privilege).
//! Only the kernel can perform I/O operation.
#define EFLAGS_IOPL0	0x0000

// --- PIC (Programmable Interrupt Controller) 8259a ------------------ //

//! PIC 8259 master.
#define PORT_8259_M	0x20

//! PIC 8259 slave.
#define PORT_8259_S	0xa0

//! PIC 8259 master (interrupt mask).
#define PORT_INT_MASK_M	0x21

//! PIC 8259 slave (interrupt mask).
#define PORT_INT_MASK_S	0xa1

//! PIC 8259 EOI (End Of Interrupt).
#define EOI		0x20

// --- IDT (Interrupt Descriptor Table) ------------------------------- //

//! Number of entries in the IDT.
#define IDT_DIM 	256

//! IDT entry structure.
typedef struct idt_entry
{
	word offset0_15;
	word selector;
	byte paramcnt;
	byte attribs;
	word offset16_31;
} idt_entry_t;

//! IDT register structure.
typedef struct idt_reg
{
	word limit;
	byte base[sizeof(dword)];
} idt_reg_t;

//! Context after an interrupt.
typedef struct irq_context
{
	//! General purpose register.
	uint32_t	edi, esi, ebp, ebx, edx, ecx, eax;
	//! Segment register.
	uint32_t	es, ds;
	//! The interrupt number.
	uint32_t	IRQ;
	//! Current instruction pointer (a.k.a. program counter).
	uint32_t	eip;
	//! Code segment register.
	uint32_t	cs;
	//! EFLAGS register.
	uint32_t	eflags;
} __attribute__ ((packed)) irq_context_t;

//! Context of a user task after an interrupt.
typedef struct irq_user_context
{
	//! General purpose register.
	uint32_t	edi, esi, ebp, ebx, edx, ecx, eax;
	//! Segment register.
	uint32_t	es, ds;
	//! The interrupt number.
	uint32_t	IRQ;
	//! Current instruction pointer (a.k.a. program counter).
	uint32_t	eip;
	//! Code segment register.
	uint32_t	cs;
	//! EFLAGS register.
	uint32_t	eflags;
	//! User stack pointer register.
	uint32_t	uesp;
	//! User stack segment register.
	uint32_t	uss;
} __attribute__ ((packed)) irq_user_context_t;

//! Context after a system call.
typedef struct syscall_context
{
	uint32_t	__dummy;
	//! General purpose register.
	uint32_t	ebx, ecx, edx, esi, edi, ebp;
	//! Segment register.
	uint32_t	es, ds;
	//! Current instruction pointer (a.k.a. program counter).
	uint32_t	eip;
	//! Code segment register.
	uint32_t	cs;
	//! EFLAGS register.
	uint32_t	eflags;
	//! User stack pointer register.
	uint32_t	uesp;
	//! User stack segment register.
	uint32_t	uss;
} __attribute__ ((packed)) syscall_context_t;

// --- Prototypes ----------------------------------------------------- //

/** \ingroup Kernel
 *  \defgroup KInterrupt Interrupt Manager
 *  The interrupt manager.
 *  @{
 */

void reprogram_PIC();
void install_IDT();

__INLINE__ void end_of_irq(int irq);

void uninstall_handler( uint8_t irq );
void install_intr_handler(uint8_t irq, void *handler);
void install_trap_handler(uint8_t trap, void *handler);

/** @} */ // end of KInterrupt

#endif
