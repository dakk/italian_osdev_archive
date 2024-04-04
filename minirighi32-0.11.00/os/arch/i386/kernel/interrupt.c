/*!	\file arch/i386/kernel/interrupt.c
 *	\brief Interrupt manager.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-04-03 Andrea Righi:
 *			Rewritten the default irq handler using a
 *			correct acknowledge for the PIC 8259a.\n
 *			Used trap gates instead of interrupt gates
 *			for hardware IRQs.\n
 *			Added a task gate for the double fault
 *			exception.\n
 *		2004-03-18 Andrea Righi:
 *			Reduced context switch overhead between
 *			task <-> kernel switch when an interrupt
 *			occurs.\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <arch/clock.h>
#include <arch/delay.h>
#include <arch/exception.h>
#include <arch/i386.h>
#include <arch/mem.h>
#include <arch/paging.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/floppy.h>
#include <kernel/keyboard.h>
#include <kernel/serial.h>
#include <kernel/speaker.h>
#include <kernel/task.h>

#include <arch/interrupt.h>

//! Interrupt Descriptor Table (IDT).
volatile idt_entry_t idt[ IDT_DIM ];

//! Value of Interrupt Descriptor Table Register.
volatile idt_reg_t idt_ptr;

// --- Extern IRQ entry points ---------------------------------------- //
// (defined inside os/arch/i386/kernel/irq.S)
extern void _irq_00, _irq_01, _irq_02, _irq_03, _irq_04, _irq_05, _irq_06, _irq_07;
extern void _irq_08, _irq_09, _irq_0A, _irq_0B, _irq_0C, _irq_0D, _irq_0E, _irq_0F;
extern void _system_call_entry;
extern void _irq_unhand;

// --- Extern EXC entry points ---------------------------------------- //
// (defined inside os/arch/i386/kernel/exc.S)
extern void _exc_00, _exc_01, _exc_02, _exc_03, _exc_04, _exc_05, _exc_06, _exc_07;
extern void _exc_09, _exc_0A, _exc_0B, _exc_0C, _exc_0D, _exc_0E, _exc_0F;
extern void _exc_unhand;

void unhandled_interrupt( irq_context_t *c );

//! An IRQ handler pointer.
typedef struct irq_desc
{
	void (*handler)( irq_context_t * );
} irq_desc_t;

//! A table for hardware interrupt handlers only.
irq_desc_t irq_handler[ IRQ_NR ] __attribute__ ((aligned(16))) =
{
	[ 0 ... IRQ_NR-1 ] = { .handler = unhandled_interrupt  }
};

//! The PIC irq mask.
uint16_t irq_mask = 0xffff;

//! The PIC irq mask for the master controller.
#define irq_master_mask		( ((unsigned char *)&(irq_mask))[0] )

//! The PIC irq mask for the slave controller.
#define irq_slave_mask		( ((unsigned char *)&(irq_mask))[1] )

//! \brief Enable an IRQ line.
//! \param IRQ The IRQ line to enable.
static __INLINE__ void enable_IRQ( uint8_t IRQ )
{
	unsigned int mask = ~(1 << IRQ);
	uint32_t flags;

	local_irq_save( flags );

	irq_mask &= mask;
	if( IRQ & 8 )
		outb( PORT_INT_MASK_S, irq_slave_mask );
	else
		outb( PORT_INT_MASK_M, irq_master_mask );

	local_irq_restore( flags );
}

//! \brief Disable an IRQ line.
//! \param IRQ The IRQ line to disable.
static __INLINE__ void disable_IRQ( uint8_t IRQ )
{
	unsigned int mask = 1 << IRQ;
	uint32_t flags;

	local_irq_save( flags );

	irq_mask |= mask;
	if( IRQ & 8 )
		outb( PORT_INT_MASK_S, irq_slave_mask );
	else
		outb( PORT_INT_MASK_M, irq_master_mask );

	local_irq_restore( flags );
}

//! \brief Initialize the Programmable Interrupt Controllers (PICs).
void __INIT__ reprogram_PIC()
{
	// Start initialization for master 8259a controller.
	outb( PORT_8259_M, 0x11 );
	// Master base vector [ 0x20 ... 0x27 ].
	outb( PORT_INT_MASK_M, 0x20 );
	// IRQ2 cascade to slave.
	outb( PORT_INT_MASK_M, 1 << 2 );
	// Finish master 8259a initialization (manual EOI).
	outb( PORT_INT_MASK_M, 0x01 );

	// Start initialization for slave 8259a controller.
	outb( PORT_8259_S, 0x11 );
	// Slave base vector [ 0x28 ... 0x2f ].
	outb( PORT_INT_MASK_S, 0x28 );
	// Cascade on IRQ2.
	outb( PORT_INT_MASK_S, 2 );
	// Finish slave 8259a initialization.
	outb( PORT_INT_MASK_S, 0x01 );

	// Initialize the irq mask for master 8259a.
	irq_mask &= ~(1 << 2);
	outb( PORT_INT_MASK_M, irq_master_mask );

	// Initialize the irq mask for master 8259a.
	outb( PORT_INT_MASK_S, irq_slave_mask );
}

//! \brief Uninstall an IRQ handler routine.
//! \param irq The interrupt number.
//! \note
//!	This is valid only for hardware interrupts. You cannot
//!	install a software interrupt handler in this way.
void uninstall_handler( uint8_t irq )
{
	uint32_t flags;

	if ( irq > IRQ_NR )
		return;

	local_irq_save( flags );

	irq_handler[ irq ].handler = NULL;
	disable_IRQ( irq );

	local_irq_restore( flags );
	return;
}

//! \brief Install an IRQ handler routine.
//! \param irq The interrupt number.
//! \param handler A pointer to the handler routine.
//! \note
//!	This is valid only for hardware interrupts. You cannot
//!	install a software interrupt handler in this way.
void install_intr_handler( uint8_t irq, void *handler )
{
	uint32_t flags;

	if ( irq > IRQ_NR )
		return;

	local_irq_save( flags );

	irq_handler[ irq ].handler = handler;
	idt[ 0x20 + irq ].attribs = INT_GATE;
	enable_IRQ( irq );

	local_irq_restore( flags );
	return;
}

//! \brief Install an IRQ handler routine with a trap gate.
//! \param irq The interrupt number.
//! \param handler A pointer to the handler routine.
//! \note
//!	This is valid only for hardware interrupts. You cannot
//!	install a software interrupt handler in this way.
void install_trap_handler( uint8_t irq, void *handler )
{
	uint32_t flags;

	if ( irq > IRQ_NR )
		return;

	local_irq_save( flags );

	irq_handler[ irq ].handler = handler;
	idt[ 0x20 + irq ].attribs = TRAP_GATE;
	enable_IRQ( irq );

	local_irq_restore( flags );
	return;
}

//! \brief Manage an unhandled interrupt.
//! \param c The context of the interrupt.
void unhandled_interrupt( irq_context_t *c )
{
	printk( KERN_ERR
		"%s(): unexpected IRQ number %03u!\n",
		__FUNCTION__, c->IRQ );
}

__INLINE__ void end_of_irq( int irq )
{
	// Simply re-enable the irq line.
	enable_IRQ( irq );
}

//! Acknowledge the irq line.
__INLINE__ void mask_and_ack_irq( int irq )
{
	unsigned int mask = (1 << irq);
	uint32_t flags;

	local_irq_save( flags );

	// Mask the irq line.
	irq_mask |= mask;

	if( irq & 8 )
	{
		// Dummy operation (but required).
		(void)inb( PORT_INT_MASK_S );

		// Mask the irq on the slave.
		outb( PORT_INT_MASK_S, irq_slave_mask );

		// Acknowledge the irq sending a EOI.
		outb( PORT_8259_S, EOI );
		outb( PORT_8259_M, EOI );
	}
	else
	{
		// Dummy operation (but required).
		(void)inb( PORT_INT_MASK_M );

		// Mask the irq on the master.
		outb( PORT_INT_MASK_M, irq_master_mask );

		// Acknowledge the irq sending a EOI.
		outb( PORT_8259_M, EOI );
	}

	local_irq_restore( flags );
}

/*! \ingroup KInterrupt */
//! \brief
//!	This is the default interrupt handler. It is invoked
//!	every time an interrupt occurs.
//! \param c The context of the current task after the interrupt.
//! Send the end of interrupt to the PICs.
void default_handler( irq_context_t *c )
{
	// Mask and acknowledge the irq line.
	mask_and_ack_irq( c->IRQ );

	// Handle the IRQ.
	if( c->IRQ < IRQ_NR )
	{
		// Call the correct interrupt handler.
		irq_handler[ c->IRQ ].handler( c );
	}
	else
	{
		// Unknown interrupt.
		unhandled_interrupt( c );
	}

	// Re-enable the irq line.
	end_of_irq( c->IRQ );
}

//! \brief Set up an IDT entry.
//! \param i The index of the entry.
//! \param selector The selector for the interrupt handler.
//! \param offset The entry point of the handler.
//! \param attribs The descriptor attributes.
//! \param paramcnt
//!	How many items must be copied from the previous stack to the
//!	new privileged stack.
void setup_IDT_entry(byte i, word selector, dword offset, byte attribs, byte paramcnt)
{
	uint32_t flags;

	local_irq_save( flags );

	idt[i].offset0_15 =  offset & 0xFFFF;
	idt[i].offset16_31 = offset >> 16;
	idt[i].selector = selector;
	idt[i].attribs = attribs;
	idt[i].paramcnt = paramcnt;

	local_irq_restore( flags );
}

//! Setup an interrupt gate.
static __INLINE__ void set_intr_gate( unsigned int n, void *addr )
{
	setup_IDT_entry( n, KERNEL_CODE, (uint32_t)addr, INT_GATE, 0 );
}

//! Setup a trap gate.
static __INLINE__ void set_trap_gate( unsigned int n, void *addr )
{
	setup_IDT_entry( n, KERNEL_CODE, (uint32_t)addr, TRAP_GATE, 0 );
}

//! Setup a task gate.
static __INLINE__ void set_task_gate( unsigned int n, unsigned int gdt_entry )
{
	setup_IDT_entry( n, gdt_entry, 0, TASK_GATE, 0 );
}

//! Setup a user-callable gate, for system calls, etc...
static __INLINE__ void set_user_gate( unsigned int n, void *addr )
{
	setup_IDT_entry( n, KERNEL_CODE, (uint32_t)addr, (TRAP_GATE | DPL_3), 0 );
}

//! \brief Initialize the IDT (Interrupt Descriptor Table).
void __INIT__ install_IDT()
{
	unsigned int i;

	// Initialize exception handlers (0x00..0x1F).
	for( i = 0x00; i <= 0x1F; i++ )
		set_intr_gate( i, &_exc_unhand );

	// Setup the IDT entries for exceptions.

	set_trap_gate( 0x00, &_exc_00 ); // divide error.
	set_trap_gate( 0x01, &_exc_01 ); // debug.
	set_intr_gate( 0x02, &_exc_02 ); // nmi.
	set_user_gate( 0x03, &_exc_03 ); // int3.
	set_user_gate( 0x04, &_exc_04 ); // overflow.
	set_user_gate( 0x05, &_exc_05 ); // bounds.
	set_trap_gate( 0x06, &_exc_06 ); // invalid operation.
	set_trap_gate( 0x07, &_exc_07 ); // device not available.
	set_task_gate( 0x08, GDT_DOUBLE_FAULT_ENTRY ); // double fault.
	set_trap_gate( 0x09, &_exc_09 ); // coprocessor segment overrun.
	set_trap_gate( 0x0a, &_exc_0A ); // invalid tss.
	set_trap_gate( 0x0b, &_exc_0B ); // segment not present.
	set_trap_gate( 0x0c, &_exc_0C ); // stack fault.
	set_trap_gate( 0x0d, &_exc_0D ); // general protection.
	set_intr_gate( 0x0e, &_exc_0E ); // page fault.
	set_intr_gate( 0x0f, &_exc_0F ); // spurious interrupt.

	// Initialize interrupt handlers 0x20..0xFF.
	for( i = 0x20; i <= 0xFF; i++ )
		set_intr_gate( i, &_irq_unhand );

	// Setup the IDT entries for IRQs.
	set_trap_gate( 0x20, &_irq_00 );
	set_trap_gate( 0x21, &_irq_01 );
	set_trap_gate( 0x22, &_irq_02 );
	set_trap_gate( 0x23, &_irq_03 );
	set_trap_gate( 0x24, &_irq_04 );
	set_trap_gate( 0x25, &_irq_05 );
	set_trap_gate( 0x26, &_irq_06 );
	set_trap_gate( 0x27, &_irq_07 );
	set_trap_gate( 0x28, &_irq_08 );
	set_trap_gate( 0x29, &_irq_09 );
	set_trap_gate( 0x2a, &_irq_0A );
	set_trap_gate( 0x2b, &_irq_0B );
	set_trap_gate( 0x2c, &_irq_0C );
	set_trap_gate( 0x2d, &_irq_0D );
	set_trap_gate( 0x2e, &_irq_0E );
	set_trap_gate( 0x2f, &_irq_0F );

	// Install Minirighi system calls handler.
	set_user_gate( MINIRIGHI_INT, &_system_call_entry );

	// Set up the IDT pointer.
	idt_ptr.limit = (IDT_DIM * sizeof(idt_entry_t) - 1);
	*(size_t *)idt_ptr.base = ((size_t)&idt);

	// Load info into IDTR register.
	__asm__ __volatile__ ("lidtl (%0)" : : "r"((uint32_t)&idt_ptr));

	// Enable interrupts.
	enable();
}
