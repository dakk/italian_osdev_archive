/*!	\file arch/i386/kernel/v86.c
 *	\brief Virtual 8086 mode.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-03-21 Andrea Righi:
 *			Added rep, o32, a32 prefixes and added the
 *			I/O port debug for v86 tasks (disabled for
 *			default).
 *		2004-03-05 Andrea Righi:
 *			New debug with printk()\n
 *		2003-12-16 Andrea Righi:
 *			Added temporary memory operators to get and
 *			free the temporary mapped pages.\n
 *	\note Copyright (C) 2003 Andrea Righi
 *	\bug
 *		A v86 task can cause a system block if it executes
 *		a 'cli' without a 'sti'. Please provide something like
 *		virtual interrupt disable/enable for v86 tasks in
 *		the future...
 *			Andrea.
 *	\note
 *		The v86 port debug is -very- useful to do reverse
 *		engineering with many devices to build a driver.
 *		First of all you can write your own little 16-bit
 *		program, using the BIOS interrupts. Then you can
 *		simply run it under minirighi enabling DEBUG_V86_IO
 *		constant. If you explore the system log you can find
 *		all the port accesses performed by the BIOS and write
 *		your own 32-bit driver... enjoy! :)
 *			Andrea.
 */

#include <const.h>
#include <string.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/mem.h>
#include <arch/paging.h>
#include <kernel/console.h>
#include <kernel/debug.h>
#include <kernel/kmalloc.h>
#include <kernel/queue.h>
#include <kernel/kernel_map.h>
#include <kernel/sched.h>

#include <arch/v86.h>

//! Enable this to debug the v86 tasks I/O operations.
// #define DEBUG_V86_IO

// --- Virtual 8086 mode task creation -------------------------------- //

extern queue_t *ready_queue;

//! The address where the task stack is placed.
#define V86_TASK_STACK_START	0x90000

//! \brief Create a new virtual-8086 mode process.
//! \param address
//!	The address of the starting point for the task.
//! \param buffer
//!	An address to a buffer where is placed the executable code.
//! \param size
//!	The size of the executable code buffer.
//! \param v86name
//!	The name of the new v86 task.
//! \return A pointer to the new-created task structure.
//! \exception NULL If an error occurs (in particular out-of-memory).
/*!
 *	This routine initializes a new v86-task structure, copies
 *	the content of the \a buffer to the entry point \a address and
 *	then add the new task to the ready queue.
 */
task_t *create_v86_process(void *address, void *buffer, size_t size, char *v86name)
{
	task_t *new_task;
	size_t i;

	sched_enter_critical_region();

	// Create the task structure.
	new_task = kmemalign( PAGE_SIZE, sizeof(task_t), GFP_KERNEL );
	if( new_task == NULL )
	{
		// Out of virtual memory!!!
		printk( KERN_WARNING
			"%s: out of virtual memory!!! "
			"cannot create v86 task [%s]\n",
			__FUNCTION__, v86name);

		sched_leave_critical_region();
		return( NULL );
	}

	// Add the task into the zombie queue, so if the current task
	// is killed the space allocated until now can be freed by the
	// kpager daemon.
	add_queue( &zombie_queue, new_task );
	new_task->state = TASK_NEW;

	// Create the privileged stack.
	new_task->pl0_stack = (size_t)kmalloc( STACK_SIZE, GFP_KERNEL );
	if( new_task->pl0_stack == NULL )
	{
		// Out of virtual memory!!!
		printk( KERN_WARNING
			"%s: out of virtual memory!!! "
			"cannot create v86 task [%s]\n",
			__FUNCTION__, v86name);
		// Free the previous allocated space.
		kfree( (void *)new_task );

		sched_leave_critical_region();
		return( NULL );
	}

	// Null the stack to enforce the page mapping.
	memset( (void *)(new_task->pl0_stack), 0, STACK_SIZE );

	// Setup the privileged stack.
	new_task->tss.ss0 = KERNEL_STACK;
	new_task->tss.esp0 = ALIGN_DOWN(
		(size_t)(new_task->pl0_stack) + STACK_SIZE - sizeof(uint32_t),
		sizeof(uint32_t) );

	// Setup the TSS.
	new_task->tss_sel = setup_GDT_entry(sizeof(tss_IO_t), (size_t)&(new_task->tss), TSS_SEG, 0 );
	if( new_task->tss_sel == NULL )
	{
		printk( KERN_ERR
			"%s(): out of GDT entries!!! "
			"cannot create v86 task [%s]\n",
			__FUNCTION__,
			v86name );
		kfree( (void *)(new_task->pl0_stack) );
		kfree( (void *)new_task );

		sched_leave_critical_region();
		return( NULL );
	}

	// V86 tasks run permanently at user privilege level.
	new_task->privilege = USER_PRIVILEGE;

	// Set the task type.
	new_task->flags.type = PROCESS_TYPE;

	// Create the virtual space of the task.
	new_task->pdbr = (uint32_t *)get_temp_page();
	if ( new_task->pdbr == NULL )
	{
		// Out of virtual memory!!!
		printk( KERN_ERR
			"%s(): out of GDT entries!!! "
			"cannot create v86 task [%s]\n",
			__FUNCTION__,
			v86name );
		// Free the previous allocated space.
		kfree( (void *)(new_task->pl0_stack) );
		kfree( new_task );
		remove_GDT_entry( new_task->tss_sel );

		sched_leave_critical_region();
		return( NULL );
	}

	// Initialize PDBR.
	memset( new_task->pdbr, 0, PDE_INDEX(K_VIR_START) * sizeof(uint32_t) );

	if( curr_task )
	{
		for( i = PDE_INDEX(K_VIR_START); i < 1023; i++ )
			new_task->pdbr[ i ] = curr_task->pdbr[ i ];
	}
	else
	{
		for( i = PDE_INDEX(K_VIR_START); i < 1023; i++ )
			new_task->pdbr[ i ] = ((uint32_t *)PAGE_DIR_MAP)[ i ];
	}
	// Map page directory into itself.
	new_task->pdbr[ 1023 ] =  vir_to_phys((size_t)(new_task->pdbr)) | P_PRESENT | P_WRITE;

	// Get the pdbr counter of the father
	// (we've just make the update!)
	new_task->pdbr_update_counter = curr_task->pdbr_update_counter;

	// Setup the task page directory address.
	new_task->tss.cr3 = vir_to_phys( (size_t)(new_task->pdbr) );

	// Temporary switch to the new address space.
	task_switch_mmu( curr_task, new_task );

	// Map the IVT (Interrupt Vector Table).
	for( i = BIOS_IVT_START; i < BIOS_IVT_END; i += PAGE_SIZE)
		map_page( i, i, P_PRESENT | P_WRITE | P_USER );
	// Map the VIDEO BUFFER.
	for ( i = VIDEO_BUF_START; i < VIDEO_BUF_END; i += PAGE_SIZE)
		map_page( i, i, P_PRESENT | P_WRITE | P_USER );
	// Map the BIOS ROM memory.
	for ( i = BIOS_ROM_START; i < BIOS_ROM_END; i += PAGE_SIZE)
		map_page( i, i, P_PRESENT | P_WRITE | P_USER );

	// It is not necessary to null the stack to enforce page	//
	// mapping, because V86-mode tasks run permanently at CPL3.	//
	// So a stack-fault is managed like a normal page-fault.	//
	new_task->tss.ss = SEGMENT((dword)V86_TASK_STACK_START-sizeof(uint32_t));
	new_task->tss.esp = OFFSET((dword)(V86_TASK_STACK_START-sizeof(uint32_t)));

	// Map the user code and data //
	if( address != buffer )
	{
		memcpy(address, buffer, size);
	}

	// Restore the old address space.
	task_switch_mmu( new_task, curr_task );

	// Setup the IO port mapping.
	new_task->tss.io_map_addr = sizeof(tss_t);
	#ifdef DEBUG_V86_IO
	// Disable all I/O ports => all I/O instructions trap to the kernel
	// with a GPF. In this way I/O operations can be first reported and
	// then emulated by the v86_monitor().
	memsetl( new_task->tss.io_map, 0xffffffff, IO_MAP_SIZE );
	#else
	// Enable all I/O ports.
	memsetl( (uint32_t *)(new_task->tss.io_map), 0, IO_MAP_SIZE );
	#endif

	// Initialize segment registers //
	new_task->tss.ds = new_task->tss.es =
	new_task->tss.fs = new_task->tss.gs = SEGMENT((size_t)address);

	// (IOPL < 3) && (VME == 0) =>
	// all INT instructions trap to the kernel with a GPF.
	new_task->tss.eflags = EFLAGS_IOPL0 | EFLAGS_VM | EFLAGS_IF | 0x02;

	// Setup program counter.
	new_task->tss.cs = SEGMENT((dword)address);
	new_task->tss.eip = OFFSET((dword)address);

	// --- Store the name --- //
	// The last character must be ever '\0' (end of string) //
	strncpy(new_task->name, v86name, sizeof(new_task->name)-2);

	// Set the process credentials.
	new_task->pid = new_pid();
	new_task->uid = new_task->euid =
	new_task->suid = new_task->fsuid = 1;
	new_task->gid = new_task->egid =
	new_task->sgid = new_task->fsgid = 1;

	// Set the parent.
	new_task->father = curr_task;

	// Insert the task into the ready queue.
	rem_queue( &zombie_queue, new_task );
	add_queue( &ready_queue, new_task );
	new_task->state = TASK_READY;

	sched_leave_critical_region();

	// This is a little trick... Because we exit
	// from a very long critical region we call
	// the scheduler to enforce a new task selection.
	schedule();

	return (new_task);
}


// --- Virtual 8086 monitor ------------------------------------------- //

#include <arch/exception.h>

//! \brief The virtual-8086 monitor.
/**
 *  \todo
 *	\li	Implement virtual interrupt instead of disabling/enabling
 *		real interrupts.
 */
void v86_monitor(exc_context_t *v86context)
{
	// v86_context *context = (v86_context *)(curr_task->pl0_stack-sizeof(v86_context));
	v86_context *context = (v86_context *)(curr_task->tss.esp0 - sizeof(v86_context));
	byte *ip;
	word *stack, *ivt;
	dword *stack32;

	ip = (byte *) LINEAR(context->cs, context->ip);
	stack = (word *) LINEAR(context->ss, context->sp);
	stack32 = (dword *)stack;
	ivt = (word *) BIOS_IVT_START;

	bool o32_prefix = FALSE, a32_prefix = FALSE, rep_prefix = FALSE;
	int i=0;

	while(TRUE)
	{
		switch (ip[i])
		{

		// --- PREFIXES --------------------------------------- //
		case 0x66:	// 32-bit operation prefix //
			o32_prefix = TRUE;
			i++;
			context->ip = (word)(context->ip + 1);
		break;


		case 0x67:	// 32-bit address prefix //
			a32_prefix = TRUE;
			i++;
			context->ip = (word)(context->ip + 1);
		break;

		case 0xFC:	// rep prefix //
			rep_prefix = TRUE;
			i++;
			context->ip = (word)(context->ip + 1);
		break;

		// --- INSTRUCTIONS ----------------------------------- //
		case 0x9C:	// PUSHF //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'pushf'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			if( !a32_prefix )
			{
				context->sp = ((context->sp & 0xFFFF) - 2) & 0xFFFF;
				stack--;
				stack[0] = (word) context->eflags;
			}
			else
			{
				context->sp = ((context->sp & 0xFFFF) - 4) & 0xFFFF;
				stack32--;
				stack32[0] = (dword) context->eflags;
			}
			context->ip = (word)(context->ip + 1);
			return;

		case 0x9D:	// POPF //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with popf\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			if( !a32_prefix )
			{
				context->eflags = stack[0] | EFLAGS_VM;
				context->sp = ((context->sp & 0xFFFF) + 2) & 0xFFFF;
			}
			else
			{
				context->eflags = stack32[0] | EFLAGS_VM;
				context->sp = ((context->sp & 0xFFFF) + 4) & 0xFFFF;
			}
			context->ip = (word)(context->ip + 1);
			return;

		case 0xCD:	// INT n //
			if (ip[i+1]==MINIRIGHI_INT)
				// Exit to system (syscalls are reserved for 32-bit tasks!!!)
				auto_kill( 1 );
			else
			{
				if (ip[i+1]==DOS_INT)
				{
					// Emulate DOS 0x20 interrupt... not yet implemented!
					// If you like to do it... here are the source... good luck! :)
					printk(KERN_WARNING "%s: "
					"INT 0x20 emulation not yet implemented @CS=%#06x,IP=%#06x\n",
						__FUNCTION__, context->cs, (uint16_t)(context->ip));
					printk("killing task [%s] (%i)\n",
						get_pname(), get_pid());
					auto_kill( 1 );
				}
				stack -= 3;
				context->sp = ((context->sp & 0xFFFF) - 6) & 0xFFFF;
				stack[0] = (word) (context->ip + 2);
				stack[1] = (word) context->cs;
				stack[2] = (word) context->eflags;

				context->cs = ivt[ip[i+1] * 2 + 1];
				context->ip = ivt[ip[i+1] * 2];
				return;
			}

		case 0xCF:	// IRET //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'iret'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			context->ip = stack[0];
			context->cs = stack[1];
			context->eflags = stack[2] | EFLAGS_VM;
			context->sp = ((context->sp & 0xFFFF) + 6) & 0xFFFF;
			return;

		case 0xFA:	// CLI //
			// FIXME: Provide something like virtual interrupt
			// instead of disabling real interrupts!!!
			if( rep_prefix || o32_prefix || a32_prefix )
			{
				printk("%s: prefixes are forbidden with 'cli'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			context->eflags = (context->eflags & (~EFLAGS_IF));
			context->ip = (word)(context->ip + 1);
			return;

		case 0xFB:	// STI //
			// FIXME: Provide something like virtual interrupt
			// instead of enabling real interrupts!!!
			if( rep_prefix || o32_prefix || a32_prefix )
			{
				printk("%s: prefixes are forbidden with 'sti'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			context->eflags = (context->eflags | EFLAGS_IF);
			context->ip = (word)(context->ip + 1);
			return;

		case 0xF4:	// HLT //
			// If interrupts are disabled the current task
			// wants to disable the CPU, so kill this task!
			if ((context->eflags & EFLAGS_IF) != EFLAGS_IF)
			{
				printk(KERN_WARNING "%s: Attempt to disable the CPU\n", __FUNCTION__);
				printk(KERN_WARNING "killing task [%s] (%i)\n", get_pname(), get_pid());
				auto_kill( 1 );
			}
			// Wait for an interrupt
			context->ip = (word)(context->ip + 1);
			enable();
			__asm__("hlt");
			return;

#ifdef DEBUG_V86_IO
		case 0xE6:	// out al, imm8 //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'out al,imm8'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			printk("%s: 0xE6 -> outb %#04x, %#04x\n",
				__FUNCTION__,
				v86context->eax & 0xff, ip[i+1]);
			out( ip[i+1], v86context->eax & 0xff );
			context->ip = (word)(context->ip + 2);
			return;
		break;

		case 0xE7:	// out ax, imm8 | out eax, imm8 //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'out ax,imm8'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			if( !o32_prefix )
			{
				printk("%s: 0xE7 -> outw %#06x, %#04x\n",
					__FUNCTION__,
					v86context->eax & 0xffff, ip[i+1]);
				out16( ip[i+1], v86context->eax & 0xffff );
			}
			else
			{
				printk("%s: 0xE7 -> outl %#010x, %#04x\n",
					__FUNCTION__,
					v86context->eax, ip[i+1]);
				out32( ip[i+1], v86context->eax );
			}
			context->ip = (word)(context->ip + 2);
			return;
		break;

		case 0xEE:	// out al, dx //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'out al,dx'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			printk("%s: 0xEE -> outb %#04x, %#06x\n",
				__FUNCTION__,
				v86context->eax & 0xff,
				v86context->edx & 0xffff);
			out( v86context->edx & 0xffff, v86context->eax & 0xff );
			context->ip = (word)(context->ip + 1);
			return;
		break;

		case 0xEF:	// out ax, dx | out eax, dx //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'out ax,dx'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			if( !o32_prefix )
			{
				printk("%s: 0xEF -> outw %#06x, %#06x\n",
					__FUNCTION__,
					v86context->eax & 0xffff,
					v86context->edx & 0xffff);
				out16( v86context->edx & 0xffff, v86context->eax & 0xffff );
			}
			else
			{
				printk("%s: 0xEF -> outl %#010x, %#06x\n",
					__FUNCTION__,
					v86context->eax,
					v86context->edx & 0xffff);
				out32( v86context->edx & 0xffff, v86context->eax );
			}
			context->ip = (word)(context->ip + 1);
			return;
		break;

		case 0xE4:	// in imm8, al //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'in imm8,al'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			v86context->eax = in( ip[i+1] ) & 0xff;
			printk("%s: 0xE4 -> inb %#04x = %X\n",
				__FUNCTION__,
				ip[i+1], v86context->eax);
			context->ip = (word)(context->ip + 2);
			return;
		break;

		case 0xE5:	// in imm8, ax | in imm8, eax //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'in imm8,ax'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			if( !o32_prefix )
				v86context->eax = in16( ip[i+1] ) & 0xffff;
			else
				v86context->eax = in32( ip[i+1] );
			printk("%s: 0xE5 -> inb %#04x = %X\n",
				__FUNCTION__,
				ip[i+1], v86context->eax);
			context->ip = (word)(context->ip + 2);
			return;
		break;

		case 0xEC:	// in dx, al //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'in dx,al'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			v86context->eax = in( v86context->edx & 0xffff ) & 0xff;
			printk("%s: 0xEC -> inb %#06x = %X\n",
				__FUNCTION__,
				v86context->edx & 0xffff, v86context->eax);
			context->ip = (word)(context->ip + 1);
			return;
		break;

		case 0xED:	// in dx, ax | in dx, eax //
			if( rep_prefix )
			{
				printk("%s: 'rep' prefix forbidden with 'in dx,ax'\n",
					__FUNCTION__);
				auto_kill( 1 );

			}
			if( !o32_prefix )
			{
				v86context->eax = in16( v86context->edx & 0xffff ) & 0xffff;
				printk("%s: 0xED -> inw %#06x = %X\n",
					__FUNCTION__,
					v86context->edx & 0xffff, v86context->eax);
			}
			else
			{
				v86context->eax = in32( v86context->edx & 0xffff );
				printk("%s: 0xED -> inl %#06x = %X\n",
					__FUNCTION__,
					v86context->edx & 0xffff, v86context->eax);
			}
			context->ip = (word)(context->ip + 1);
			return;
		break;

		case 0x6E:	// outsb //
			if( !a32_prefix )
			{
				outsb(	v86context->edx & 0xffff,
					(void *)(LINEAR(v86context->ds, v86context->esi) & 0xffff),
					v86context->ecx & 0xffff );
			}
			else
			{
				outsb(	v86context->edx & 0xffff,
					(void *)(LINEAR(v86context->ds, v86context->esi)),
					v86context->ecx );
			}
			context->ip = (word)(context->ip + 1);
			return;
		break;

		case 0x6F:	// outsw | outsl //
			if( !a32_prefix )
			{
				if( !o32_prefix )
					outsw(	v86context->edx & 0xffff,
						(void *)(LINEAR(v86context->ds, v86context->esi) & 0xffff),
						v86context->ecx & 0xffff );
				else
					outsl(	v86context->edx & 0xffff,
						(void *)(LINEAR(v86context->ds, v86context->esi) & 0xffff),
						v86context->ecx & 0xffff );
			}
			else
			{
				if( !o32_prefix )
					outsw(	v86context->edx & 0xffff,
						(void *)(LINEAR(v86context->ds, v86context->esi)),
						v86context->ecx );
				else
					outsl(	v86context->edx & 0xffff,
						(void *)(LINEAR(v86context->ds, v86context->esi)),
						v86context->ecx );
			}
			context->ip = (word)(context->ip + 1);
			return;
		break;

		case 0x6C:	// insb //
			if( !a32_prefix )
			{
				insb(	v86context->edx & 0xffff,
					(void *)(LINEAR(v86context->es, v86context->edi) & 0xffff),
					v86context->ecx & 0xffff );
			}
			else
			{
				outsb(	v86context->edx & 0xffff,
					(void *)(LINEAR(v86context->es, v86context->edi)),
					v86context->ecx );
			}
			context->ip = (word)(context->ip + 1);
			return;
		break;

		case 0x6D:	// insw | insl //
			if( !a32_prefix )
			{
				if( !o32_prefix )
					insw(	v86context->edx & 0xffff,
						(void *)(LINEAR(v86context->es, v86context->edi) & 0xffff),
						v86context->ecx & 0xffff );
				else
					insl(	v86context->edx & 0xffff,
						(void *)(LINEAR(v86context->es, v86context->edi) & 0xffff),
						v86context->ecx & 0xffff );
			}
			else
			{
				if( !o32_prefix )
					insw(	v86context->edx & 0xffff,
						(void *)(LINEAR(v86context->es, v86context->edi)),
						v86context->ecx );
				else
					insl(	v86context->edx & 0xffff,
						(void *)(LINEAR(v86context->es, v86context->edi)),
						v86context->ecx );
			}
			context->ip = (word)(context->ip + 1);
			return;
		break;
#endif // DEBUG_V86_IO

		default:	// Unknown instruction! //
			printk(KERN_WARNING "%s: forbidden operation %#04x @ (CS=%#06x,IP=%#06x)\n",
				__FUNCTION__, ip[0], context->cs, (uint16_t)(context->ip));
			context->ip = (word)(context->ip + 1);
			printk(KERN_WARNING "killing task [%s] (%i)\n",
				get_pname(), get_pid());
			auto_kill( 1 );
			return;
		}
	}
}
