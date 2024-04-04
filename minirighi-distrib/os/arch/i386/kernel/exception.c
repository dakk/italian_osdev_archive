/*!	\file arch/i386/kernel/exception.c
 *	\brief Exceptions manager.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-03-18 Andrea Righi:
 *			Reduced context switch overhead between
 *			task <-> kernel switch when an exception
 *			occurs.
 *	\note Copyright (&copy;) 2003 Andrea Righi
 *
 * \n<b>From: &lt;INTEL 80386 PROGRAMMER'S REFERENCE MANUAL 1986&gt;</b>
 * <table>
 * <tr><td><b>Identifier</b></td><td><b>Description</b></td></tr>
 *
 * <tr><td align="center">0</td><td>Divide error</td>
 * <tr><td align="center">1</td><td>Debug exception</td>
 * <tr><td align="center">2</td><td>Nonmaskable interrupt</td>
 * <tr><td align="center">3</td><td>Breakpoint (one-byte INT 3 instruction)</td>
 * <tr><td align="center">4</td><td>Overflow (INTO instruction)</td>
 * <tr><td align="center">5</td><td>Bounds check (BOUND instruction)</td>
 * <tr><td align="center">6</td><td>Invalid opcode</td>
 * <tr><td align="center">7</td><td>Coprocessor not available</td>
 * <tr><td align="center">8</td><td>Double fault</td>
 * <tr><td align="center">9</td><td>(reserved)</td>
 * <tr><td align="center">10</td><td>Invalid TSS</td>
 * <tr><td align="center">11</td><td>Segment not present</td>
 * <tr><td align="center">12</td><td>Stack exception</td>
 * <tr><td align="center">13</td><td>General protection</td>
 * <tr><td align="center">14</td><td>Page fault</td>
 * <tr><td align="center">15</td><td>(reserved)</td>
 * <tr><td align="center">16</td><td>Coprecessor error</td>
 *
 * <tr><td align="center">17-31</td><td>(reserved)</td>
 * <tr><td align="center">32-255</td><td>Available for external interrupts via INTR pin</td>
 *
 * </table>
 */

#include <const.h>

#include <kernel/console.h>
#include <kernel/sched.h>

#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/mem.h>
#include <arch/paging.h>
#include <arch/v86.h>

#include <arch/exception.h>

/** \ingroup Kernel
 *  \defgroup KException Exception Manager
 *  The exception manager.
 *  @{
 */

//! \brief Panic routine.
/**
 *	The task that calls this routine must be killed.
 *	If it is invoked by the kernel we can only halt
 *	the system!
 */
void panic()
{
	if ( curr_task != NULL )
	{
		if( curr_task->pid != 0 )
		{
			// The task should be killed.
			kprintf( "\ntask [%s] (%i) killed!\n",
				curr_task->name, curr_task->pid );
		}

		// Re-enable the scheduler if it has been disabled.
		if( sched_is_disabled() )
		{
			sched_leave_critical_region();
		}

		// Re-enable interrupts.
		auto_kill( 1 );
	}
	// Ouch! Kernel panic!!! Halt the system.
	kprintf( "\nkernel panic: system halted!\n" );
	halt();
}

//! \brief
//!	Dump the CPU registers.
//! \param c The context of the current task after an exception.
static __INLINE__ void dump_registers( exc_context_t *c )
{
	register uint16_t ss;
	register uint32_t cr0, cr2, cr3, cr4;
	uint16_t fs, gs;

	// Save stack segment register.
	__asm__ __volatile__ ("movw %%ss, %0" : "=&r"(ss) : );

	// Save %fs and %gs registers.
	__asm__ __volatile__ ("movw %%fs, %0" : "=m"(fs) :);
	__asm__ __volatile__ ("movw %%gs, %0" : "=m"(gs) :);

	// Save control registers.
	__asm__ __volatile__ ("movl %%cr0, %0" : "=&r"(cr0) : );
	__asm__ __volatile__ ("movl %%cr2, %0" : "=&r"(cr2) : );
	__asm__ __volatile__ ("movl %%cr3, %0" : "=&r"(cr3) : );
	__asm__ __volatile__ ("movl %%cr2, %0" : "=&r"(cr4) : );

	// Dump registers.
	kprintf("\neax = %#010x  ds = %#010x  cr0 = %#010x  esp    = %#010x", c->eax, (uint16_t)(c->ds), cr0, c);
	kprintf("\nebx = %#010x  es = %#010x  cr2 = %#010x  ebp    = %#010x", c->ebx, (uint16_t)(c->es), cr2, c->ebp);
	kprintf("\necx = %#010x  fs = %#010x  cr3 = %#010x  eip    = %#010x", c->ecx, (uint16_t)(fs), cr3, c->eip);
	kprintf("\nedx = %#010x  gs = %#010x  cr4 = %#010x  eflags = %#010x", c->edx, (uint16_t)(gs), cr4, c->eflags);
	kprintf("\nesi = %#010x  ss = %#010x  exc = %#010x", c->esi, (uint16_t)(ss), c->EXC);
	kprintf("\nedi = %#010x  cs = %#010x  err = %#010x", c->edi, (uint16_t)(c->cs), c->err_code);
}

//! \brief
//!	This is the default exception handler. It is invoked
//!	every time an exception occurs. The kernel must route the
//!	exececution to the opportune procedures to correctly manage
//!	the exception.
//! \param c The context of the current task after an exception.
void default_exception( exc_context_t *c )
{
	switch( c->EXC )
	{
		case 0x00:
			kprintf("\nException 00: DIVISION BY ZERO\n");
		break;

		case 0x01:
			kprintf("\nException 01: DEBUG EXCEPTION DETECTED\n");
		break;

		case 0x02:
			kprintf("\nException 02: NON MASKABLE INTERRUPT\n");
		break;

		case 0x03:
			kprintf("\nException 03: BREAKPOINT INSTRUCTION DETECTED\n");
			dump_registers( c );
			kprintf("\nBreakpoint from task [%s] (%i)\n", get_pname(), get_pid());
			// After a breakpoint we can restore execution.
			return;
		break;

		case 0x04:
			kprintf("\nException 04: INTO DETECTED OVERFLOW\n");
		break;

		case 0x05:
			kprintf("\nException 05: BOUND RANGE EXCEEDED\n");
		break;

		case 0x06:
			kprintf("\nException 06: INVALID OPCODE\n");
		break;

		case 0x07:
			kprintf("\nException 07: PROCESSOR EXTENSION NOT AVAILABLE\n");
		break;

		case 0x08:
			kprintf("\nException 08: DOUBLE FAULT DETECTED\n");
		break;

		case 0x09:
			kprintf("\nException 09: PROCESSOR EXTENSION PROTECTION FAULT\n");
		break;

		case 0x0A:
			kprintf("\nException 0A: INVALID TASK STATE SEGMENT\n");
		break;

		case 0x0B:
			kprintf("\nException 0B: SEGMENT NOT PRESENT\n");
		break;

		case 0x0C:
			kprintf("\nException 0C: STACK FAULT\n");
		break;

		case 0x0D:
			if ((c->eflags & EFLAGS_VM) == EFLAGS_VM)
			{
				v86_monitor( c );
				return;
			}
			kprintf("\nException 0D: GENERAL PROTECTION FAULT\n");
		break;

		case 0x0E:
			if ((c->err_code & P_PRESENT) != P_PRESENT) {
				register uint32_t cr2;

				__asm__ __volatile__ ("movl %%cr2, %0" : "=&r"(cr2));
				if (!page_fault_handler(cr2)) {
					return;
				}
			} else if ((c->err_code & P_WRITE) == P_WRITE) {
				register uint32_t cr2;

				__asm__ __volatile__ ("movl %%cr2, %0" : "=&r"(cr2));
				if (!page_fault_cow_handler(cr2)) {
					return;
				}
			}
			kprintf("\nException 0E: PAGE FAULT (protection fault)!\n");
		break;

		case 0x0F:
			kprintf("\nException 0F: UNKNOWN EXCEPTION\n");
		break;

		default:
			kprintf("\nException %02X: UNEXPECTED !!!\n", c->EXC);
		break;
	}

	// Dump the CPU registers.
	dump_registers( c );
	panic();
}

/** @} */ // end of KException
