/*!	\file arch/i386/kernel/doublefault.c
 *	\brief Double fault exception handler.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-31\n
 *	\note Copyright (&copy;) 2003 Andrea Righi
 *
 *	Stack fault exception in kernel mode must be
 *	managed by a task-gate in the IDT. Otherwise
 *	the system will be rebooted because there is no
 *	stack space left to manage the exception!
 *
 *	So we must define a TSS for the double fault
 *	exception. When the double fault task will be
 *	called we'll have a stack fault exception in
 *	kernel-mode.
 *
 *	In user-mode there are no problems, because there
 *	is also the privileged stack. In this case a stack
 *	fault is managed like a normal page fault.
 */

#include <arch/i386.h>
#include <arch/mem.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/kernel_map.h>
#include <kernel/task.h>

//! The size of the stack used by the double fault task.
#define DOUBLE_FAULT_STACK_SIZE		4096

//! The stack of the double fault task.
static unsigned char double_fault_stack[ DOUBLE_FAULT_STACK_SIZE ];

//! The double fault task's stack begins here.
#define DOUBLE_FAULT_STACK_START \
	( ((unsigned long)double_fault_stack) + DOUBLE_FAULT_STACK_SIZE )

static void double_fault_handler();

//! The TSS of the double fault stack.
tss_t double_fault_tss __attribute__ ((__aligned__(16))) =
{
	.esp0 = DOUBLE_FAULT_STACK_START,
	.ss0 = KERNEL_STACK,
	.io_map_addr = sizeof(tss_t),
	.eip = (uint32_t)&double_fault_handler,
	.eflags = 0x00000082,
	.esp = DOUBLE_FAULT_STACK_START,
	.ds = KERNEL_DATA,
	.es = KERNEL_DATA,
	.ss = KERNEL_STACK,
	.cs = KERNEL_CODE,
	.cr3 = K_PDE,
};

//! This is the handler for the double fault exception.
static void double_fault_handler( void )
{
	set_color( LIGHT_RED );
	printk( KERN_EMERG "Kernel Panic: double fault exception in kernel mode!\n");
	printk( KERN_EMERG "System halted!\n");
	// Forever idle...
	for(;;)
	{
		idle();
	}
}
