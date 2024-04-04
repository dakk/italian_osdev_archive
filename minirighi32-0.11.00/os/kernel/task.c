/*!	\file kernel/task.c
 *	\brief Task Management.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-12-08 Andrea Righi:
 *			Added fork() system call.\n
 *		2004-03-29 Andrea Righi:
 *			Added multi-threading support.\n
 *		2004-03-13 Andrea Righi:
 *			Fixed asyncronous kill() operation.\n
 *		2004-03-08 Andrea Righi:
 *			Put idle and kpager threads in separated files.\n
 *		2003-12-15 Andrea Righi:
 *			Added temporary memory operators to get and
 *			free the temporary mapped pages.\n
 *		2003-12-16 Andrea Righi:
 *			Fixed mutual exclusion bug in kill() and
 *			waitpid()\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <stdlib.h>
#include <string.h>

#include <arch/atomic.h>
#include <arch/clock.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/mem.h>
#include <arch/paging.h>
#include <arch/v86.h>

#include <kernel/console.h>
#include <kernel/cow.h>
#include <kernel/debug.h>
#include <kernel/elf32.h>
#include <kernel/fat.h>
#include <kernel/kmalloc.h>
#include <kernel/queue.h>
#include <kernel/semaphore.h>
#include <kernel/shell.h>
#include <kernel/umalloc.h>
#include <kernel/sched.h>

#include <kernel/task.h>

extern task_t *kpagd;
extern void kpagerd(void);

extern queue_t *ready_queue;
extern queue_t *wait_queue;
extern queue_t *zombie_queue;

extern task_t	*curr_task;

extern task_t *idle_task;

extern void __task_exit_point;

extern void ret_from_syscall;

// --- PID operators ---------------------------------------------------//

/** \ingroup Multitasking
 *  \defgroup MTPid PID Operators
 *  Operators to manage the pids for the tasks.
 *  @{
 */

//! Last used pid.
pid_t last_pid = 0;

//! A spinlock used to generate new pids.
DECLARE_SPINLOCK( pid_lock );

//! \brief Create a new pid.
//! \return The new pid created.
/*!
 *	This routine generate a pid using an atomic increment.
 */
__INLINE__ pid_t new_pid()
{
	pid_t ret;

	spin_lock( &pid_lock );
	ret = ++last_pid;
	spin_unlock( &pid_lock );

	return( ret );
}

//! \brief Return the current task pid if there is a curr_task.
//! \return The current task pid.
__INLINE__ pid_t get_pid()
{
	if ( !curr_task ) return( NULL );
	return( curr_task->pid );
}

/** @} */ // end of MTPid

// --- Process info ----------------------------------------------------//

/** \ingroup Multitasking
 *  \defgroup MTProcessInfo Process Informations
 *  Operators to get some processes informations.
 *  @{
 */

//! \brief Return the current task name if there is a curr_task.
//! \return The current task name.
__INLINE__ char *get_pname()
{
	if ( !curr_task )
		return( NULL );
	return( curr_task->name );
}

//! Return the structure of the task that has pid as identifier
//! or NULL if the process is not found.
//! \warning This routine must be called in mutual exclusion!!!
task_t *get_task_info( pid_t pid )
{
	// Queues where to search the process.
	static queue_t **q[] = { &ready_queue, &wait_queue };
	queue_t *entry;
	task_t *p;
	int i, n;

	if( (curr_task->pid == pid) || (pid == -1) )
		return( curr_task );

	if( pid == 1 )
		return( idle_task );

	// Look in every queues.
	for( i = 0; i < _countof( q ); i++ )
	{
		queue_for_each( entry, n, *(q[i]) )
		{
			p = queue_get_entry( entry );
			if( p->pid == pid )
			{
				goto out;
			}
		}
	}
	p = NULL;
out:
	return( p );
}

/** @} */ // end of MTProcessInfo

// --- Task's routines -------------------------------------------------//

/** \ingroup Multitasking
 *  \defgroup MTTaskRoutines Generic Routines
 *  The generic routines for task management.
 *  @{
 */

//! \brief Initialize the stack for a new process.
//! \param argc
//!	The number of arguments passed to the new process.
//! \param argv
//!	A pointer to the argument strings.
//!	<b>NOTE: The content of \p argv may be changed!!!</b>
//! \param stack_start
//!	Where the stack is placed in memory.
//! \param privilege
//!	The privilege of the task.
//! \return The starting stack pointer after initialization.
static __INLINE__ uint32_t *task_setup_stack( int argc, char **argv, size_t stack_start )
{
	uint32_t *stack = (uint32_t *)
		( ALIGN_DOWN(stack_start, sizeof(uint32_t)) - sizeof(uint32_t) );
	uint32_t *user_argv;
	int i;

	// Copy external parameters strings.
	for( i = 0; i < argc; i++ )
	{
		stack = (uint32_t *)((size_t)stack - strlen(argv[i]) - 1);
		strcpy( (char *)stack, argv[i] );
		argv[i] = (char *)stack;
	}

	// Round down the stack pointer to the stack boundaries.
	stack = (uint32_t *)( ALIGN_DOWN((size_t)stack, sizeof(uint32_t)) );

	// Copy parameter string pointers.
	for( i = 0; i < argc; i++ )
	{
		*(--stack) = (uint32_t)(argv[argc-i-1]);
	}

	// Set the process argv pointer.
	user_argv = stack;
	*(--stack) = (uint32_t)user_argv;
	// Copy number of parameters value.
	*(--stack) = argc;
	// Copy the exit point address.
	*(--stack) = (size_t)(&__task_exit_point);

	// Insert a NULL into the stack.
	*(stack - 1) = NULL;

	// Return the initial stack pointer.
	return( stack );
}

//! \brief Create a new virtual space.
//! \return
//!	The physical address of the page directory for the new
//!	virtual space.
//! \exception NULL If an error occurs (in particular out-of-memory).
//! \warning Be sure to enter here in mutual exclusion!!!
static void new_vspace( task_t *t )
{
	int i;

	// Create the virtual space of the task.
	t->pdbr = (uint32_t *)get_temp_page();
	if( t->pdbr == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: out of temporary memory space!\n",
			__FUNCTION__);
		return;
	}

	// Initialize PDBR.
	memset( t->pdbr, 0, PDE_INDEX(K_VIR_START) * sizeof(uint32_t) );

	if( curr_task )
	{
		for( i = PDE_INDEX(K_VIR_START); i < 1023; i++ )
			t->pdbr[ i ] = curr_task->pdbr[ i ];
	}
	else
	{
		for( i = PDE_INDEX(K_VIR_START); i < 1023; i++ )
			t->pdbr[ i ] = ((uint32_t *)PAGE_DIR_MAP)[ i ];
	}
	// Map page directory into itself.
	t->pdbr[ 1023 ] =  vir_to_phys((size_t)(t->pdbr)) | P_PRESENT | P_WRITE;

	// Get the pdbr counter of the father
	// (we've just made the update!)
	if( curr_task )
	{
		t->pdbr_update_counter = curr_task->pdbr_update_counter;
	}
}

//! \brief Create a new task.
//! \param routine
//!	The address of the entry point for the task.
//! \param argc
//!	The number of parameters.
//! \param argv
//!	The vector of the parameter strings.
//! \param pname
//!	The name of the new task.
//! \param privilege
//!	The privilege level (KERNEL_PRIVILEGE or USER_PRIVILEGE).
//! \return A pointer to the new-created thread structure.
//! \exception NULL If an error occurs (in particular out-of-memory).
task_t *create_process( void *routine, int argc, char **argv, char *pname, int privilege )
{
	task_t *new_task;

	sched_enter_critical_region();

	// --- Create the task structure ------------------------------ //
	//!
	//! IA-32 Intel(R) Architecture Software Developer's Manual
	//! Volume 3: System Programming Guide - Chapter 6 - reports:
	//!
	//! "If paging is used, care should be taken to avoid placing a
	//! page boundary within the part of the TSS that the processor
	//! reads during a task switch (the first 104 bytes). If a page
	//! boundary is placed within this part of the TSS, the pages on
	//! either side of the boundary must be present at the same time
	//! and contiguous in physical memory.
	//!
	//! The reason for this restriction is that when accessing a TSS
	//! during a task switch, the processor reads and writes into
	//! the first 104 bytes of each TSS from contiguous physical
	//! addresses beginning with the physical address of the first
	//! byte of the TSS. It may not perform address translation at a
	//! page boundary if one occurs within this area. So, after the
	//! TSS access begins, if a part of the 104 bytes is not both
	//! present and physically contiguous, the processor will access
	//! incorrect TSS information, without generating a page-fault
	//! exception. The reading of this incorrect information will
	//! generally lead to an unrecoverable exception later in the
	//! task switch process..."
	new_task = kmemalign( PAGE_SIZE, sizeof(task_t), GFP_KERNEL );
	if( new_task == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: cannot allocate task structure for '%s'\n",
			__FUNCTION__, pname);

		sched_leave_critical_region();
		return( NULL );
	}

	// Add the task into the zombie queue, so if the current task
	// is killed the space allocated until now can be freed by the
	// kpager daemon.
	add_queue( &zombie_queue, new_task );
	new_task->state = TASK_NEW;

	// Create the pl0-stack.
	new_task->pl0_stack = (size_t)kmalloc( STACK_SIZE, GFP_KERNEL );
	if( new_task->pl0_stack == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: cannot allocate pl0 stack for '%s'\n",
			__FUNCTION__, pname);
		kfree( new_task );

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
			"cannot create task '%s'\n",
			__FUNCTION__,
			pname );
		kfree( new_task );
		kfree( (void *)(new_task->pl0_stack) );

		sched_leave_critical_region();
		return( NULL );
	}

	// Set the privilege level.
	new_task->privilege = privilege;

	// Set the task type.
	new_task->flags.type = PROCESS_TYPE;

	// Create the virtual space of the task.
	new_vspace( new_task );
	if ( new_task->pdbr == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: out of memory! cannot create pdbr for '%s'!\n",
			__FUNCTION__, pname);
		kfree( new_task );
		kfree( (void *)(new_task->pl0_stack) );
		remove_GDT_entry( new_task->tss_sel );

		sched_leave_critical_region();
		return( NULL );
	}

	// Setup the task page directory address.
	new_task->tss.cr3 = vir_to_phys( (size_t)(new_task->pdbr) );

	// Temporary switch to the new address space.
	if( curr_task != NULL )
		task_switch_mmu( curr_task, new_task );
	else
		switch_mmu( new_task->tss.cr3 );

	// Create the task stack.
	new_task->tss.ss = (privilege == KERNEL_PRIVILEGE) ? KERNEL_STACK : USER_STACK | 3;
	if ( privilege == KERNEL_PRIVILEGE )
	{
		new_task->tss.esp = (size_t)
			task_setup_stack( argc, argv,
				new_task->tss.esp0 + sizeof(uint32_t) );
	}
	else
	{
		new_task->tss.esp = (size_t)
			task_setup_stack( argc, argv,
				TASK_STACK_START );
	}

	// Initialize the user heap.
	umalloc_init( new_task, TASK_HEAP_START, TASK_HEAP_DEFAULT_SIZE );

	// Restore the old address space.
	if( curr_task != NULL )
		task_switch_mmu( new_task, curr_task );

	// Setup the IO port mapping.
	new_task->tss.io_map_addr = sizeof(tss_t);
	memsetl( new_task->tss.io_map, 0xffffffff, IO_MAP_SIZE );

	// Setup general registers.
	if ( privilege == KERNEL_PRIVILEGE )
	{
		new_task->tss.ds = new_task->tss.es =
		new_task->tss.fs = new_task->tss.gs = KERNEL_DATA;
	}
	else
	{
		new_task->tss.ds = new_task->tss.es =
		new_task->tss.fs = new_task->tss.gs = USER_DATA | 3;
	}

	// Setup the eflags register.
	new_task->tss.eflags = EFLAGS_IOPL0 | EFLAGS_IF | 0x02;

	// Setup starting address (program counter).
	new_task->tss.cs = (privilege == KERNEL_PRIVILEGE) ? KERNEL_CODE : USER_CODE | 3;
	new_task->tss.eip = (size_t)routine;

	// Store the process name.
	// The last character must be ever '\0' (end of string).
	strncpy( new_task->name, pname, sizeof(new_task->name) - 2 );

	// Set the process credentials.
	new_task->pid = new_pid();
	if( privilege == KERNEL_PRIVILEGE )
	{
		new_task->uid = new_task->euid =
		new_task->suid = new_task->fsuid = 0;
		new_task->gid = new_task->egid =
		new_task->sgid = new_task->fsgid = 0;
	}
	else
	{
		new_task->uid = new_task->euid =
		new_task->suid = new_task->fsuid = 1;
		new_task->gid = new_task->egid =
		new_task->sgid = new_task->fsgid = 1;
	}

	// Set the parent.
	new_task->father = curr_task;

	// Set the console.
	if( curr_task != NULL )
	{
		new_task->console = curr_task->console;
	}

	// Setup the priority.
	new_task->priority = new_task->counter = HIGH_PRIORITY;

	// Insert the task into the ready queue.
	rem_queue( &zombie_queue, new_task );
	add_queue( &ready_queue, new_task );
	new_task->state = TASK_READY;

	sched_leave_critical_region();

	// This is a little trick... Because we exit
	// from a very long critical region we call
	// the scheduler to enforce a new task selection.
	if( curr_task != NULL )
		schedule();

	return( new_task );
}

//! \brief Create a new thread of a process.
//! \param routine
//!	The address of the entry point for the thread.
//! \param argc
//!	The number of parameters.
//! \param argv
//!	The vector of the parameter strings.
//! \param pname
//!	The name of the new task.
//! \return A pointer to the new-created thread structure.
//! \exception NULL If an error occurs (in particular out-of-memory).
/**
 *  \warning
 *	This routine must be executed in mutual exclusion, because if
 *	the current task is killed during the thread creation, the
 *	space allocated before will be lost.
 */
task_t *create_thread( void *routine, int argc, char **argv, char *pname )
{
	task_t *new_thread, *proc_father;
	int privilege;

	if( curr_task == NULL )
		return( NULL );

	sched_enter_critical_region();

	// --- Create the thread structure ---------------------------- //
	//!
	//! IA-32 Intel(R) Architecture Software Developer's Manual
	//! Volume 3: System Programming Guide - Chapter 6 - reports:
	//!
	//! "If paging is used, care should be taken to avoid placing a
	//! page boundary within the part of the TSS that the processor
	//! reads during a task switch (the first 104 bytes). If a page
	//! boundary is placed within this part of the TSS, the pages on
	//! either side of the boundary must be present at the same time
	//! and contiguous in physical memory.
	//!
	//! The reason for this restriction is that when accessing a TSS
	//! during a task switch, the processor reads and writes into
	//! the first 104 bytes of each TSS from contiguous physical
	//! addresses beginning with the physical address of the first
	//! byte of the TSS. It may not perform address translation at a
	//! page boundary if one occurs within this area. So, after the
	//! TSS access begins, if a part of the 104 bytes is not both
	//! present and physically contiguous, the processor will access
	//! incorrect TSS information, without generating a page-fault
	//! exception. The reading of this incorrect information will
	//! generally lead to an unrecoverable exception later in the
	//! task switch process..."
	new_thread = kmemalign( PAGE_SIZE, sizeof(task_t), GFP_KERNEL );
	if( new_thread == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: cannot allocate task structure for '%s'\n",
			__FUNCTION__, pname);

		sched_leave_critical_region();
		return( NULL );
	}

	// Add the thread into the zombie queue, so if the current task
	// is killed the space allocated until now can be freed by the
	// kpager daemon.
	add_queue( &zombie_queue, new_thread );
	new_thread->state = TASK_NEW;

	// Create the pl0-stack.
	new_thread->pl0_stack = (size_t)kmalloc( STACK_SIZE, GFP_KERNEL );
	if( new_thread->pl0_stack == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: cannot allocate pl0 stack for '%s'\n",
			__FUNCTION__, pname);
		kfree( new_thread );

		sched_leave_critical_region();
		return( NULL );
	}

	// Null the stack to enforce the page mapping.
	memset( (void *)(new_thread->pl0_stack), 0, STACK_SIZE );

	// Setup the privileged stack.
	new_thread->tss.ss0 = KERNEL_STACK;
	new_thread->tss.esp0 = ALIGN_DOWN(
		(size_t)(new_thread->pl0_stack) + STACK_SIZE - sizeof(uint32_t),
		sizeof(uint32_t) );

	// Setup the TSS.
	new_thread->tss_sel = setup_GDT_entry(sizeof(tss_IO_t), (size_t)&(new_thread->tss), TSS_SEG, 0);
	if( new_thread->tss_sel == NULL )
	{
		printk( KERN_ERR
			"%s(): out of GDT entries!!! "
			"cannot create thread '%s'\n",
			__FUNCTION__,
			pname );
		kfree( new_thread );
		kfree( (void *)(new_thread->pl0_stack) );

		sched_leave_critical_region();
		return( NULL );
	}

	// Set the process type.
	new_thread->flags.type = THREAD_TYPE;

	// Get the father's privilege.
	privilege = new_thread->privilege = curr_task->privilege;

	// Get the first process between all the parents of the
	// current thread. This is necessary to setup properly
	// the user space. Remember that every thread share the
	// same address space.
	proc_father = curr_task;
	while( proc_father->flags.type == THREAD_TYPE )
	{
		proc_father = proc_father->father;
	}

	// Setup the thread heap equal to the father's heap.
	new_thread->heap_start = proc_father->heap_start;
	new_thread->heap_size = proc_father->heap_size;

	// Setup the task address space.
	new_thread->tss.cr3 = proc_father->tss.cr3;

	// Get the same page directory of the father.
	new_thread->pdbr = curr_task->pdbr;

	// Get the same pdbr counter of the father.
	new_thread->pdbr_update_counter = curr_task->pdbr_update_counter;

	// Create the thread stack.
	new_thread->tss.ss = (privilege == KERNEL_PRIVILEGE) ? KERNEL_STACK : USER_STACK | 3;
	if ( privilege == KERNEL_PRIVILEGE )
	{
		new_thread->tss.esp = (size_t)
			task_setup_stack( argc, argv,
				new_thread->tss.esp0 + sizeof(uint32_t) );
	}
	else
	{
		// Put the thread's stack in the heap.
		new_thread->stack = (size_t)umalloc( STACK_SIZE );
		if( new_thread->stack == NULL )
		{
			printk( KERN_ERR
				"%s(): not enough space in the heap! "
				"cannot create thread '%s'\n",
				__FUNCTION__,
				pname );
			kfree( new_thread );
			kfree( (void *)(new_thread->pl0_stack) );

			sched_leave_critical_region();
			return( NULL );
		}
		new_thread->tss.esp = (size_t)
			task_setup_stack( argc, argv, new_thread->stack + STACK_SIZE );
	}

	// Setup the IO port mapping.
	new_thread->tss.io_map_addr = sizeof( tss_t );
	memsetl( new_thread->tss.io_map, 0xffffffff, IO_MAP_SIZE );

	// Setup general registers.
	if ( privilege == KERNEL_PRIVILEGE )
	{
		new_thread->tss.ds = new_thread->tss.es =
		new_thread->tss.fs = new_thread->tss.gs = KERNEL_DATA;
	}
	else
	{
		new_thread->tss.ds = new_thread->tss.es =
		new_thread->tss.fs = new_thread->tss.gs = USER_DATA | 3;
	}

	// Setup the eflags register.
	new_thread->tss.eflags = EFLAGS_IOPL0 | EFLAGS_IF | 0x02;

	// Setup starting address (program counter).
	new_thread->tss.cs =
		(privilege == KERNEL_PRIVILEGE) ? KERNEL_CODE : (USER_CODE | 3);
	new_thread->tss.eip = (size_t)routine;

	// Store the process name.
	// The last character must be ever '\0' (end of string).
	strncpy( new_thread->name, pname, sizeof(new_thread->name) - 2 );

	// Set the process credentials.
	new_thread->pid = new_pid();
	new_thread->uid = curr_task->uid;
	new_thread->euid = curr_task->euid;
	new_thread->suid = curr_task->suid;
	new_thread->fsuid = curr_task->fsuid;
	new_thread->gid = curr_task->gid;
	new_thread->egid = curr_task->egid;
	new_thread->sgid = curr_task->sgid;
	new_thread->fsgid = curr_task->fsgid;

	// Get the father.
	new_thread->father = curr_task;

	// Set the console.
	new_thread->console = curr_task->console;

	// Setup the priority.
	new_thread->priority = new_thread->counter = HIGH_PRIORITY;

	// Insert the task into the ready queue.
	rem_queue( &zombie_queue, new_thread );
	add_queue( &ready_queue, new_thread );
	new_thread->state = TASK_READY;

	sched_leave_critical_region();

	// This is a little trick... Because we exit
	// from a very long critical region we call
	// the scheduler to enforce a new task selection.
	schedule();

	return( new_thread );
}

pid_t do_fork(syscall_context_t *regs)
{
	task_t *new_task;
	int i;

	sched_enter_critical_region();

	// --- Create the task structure ------------------------------ //
	//!
	//! IA-32 Intel(R) Architecture Software Developer's Manual
	//! Volume 3: System Programming Guide - Chapter 6 - reports:
	//!
	//! "If paging is used, care should be taken to avoid placing a
	//! page boundary within the part of the TSS that the processor
	//! reads during a task switch (the first 104 bytes). If a page
	//! boundary is placed within this part of the TSS, the pages on
	//! either side of the boundary must be present at the same time
	//! and contiguous in physical memory.
	//!
	//! The reason for this restriction is that when accessing a TSS
	//! during a task switch, the processor reads and writes into
	//! the first 104 bytes of each TSS from contiguous physical
	//! addresses beginning with the physical address of the first
	//! byte of the TSS. It may not perform address translation at a
	//! page boundary if one occurs within this area. So, after the
	//! TSS access begins, if a part of the 104 bytes is not both
	//! present and physically contiguous, the processor will access
	//! incorrect TSS information, without generating a page-fault
	//! exception. The reading of this incorrect information will
	//! generally lead to an unrecoverable exception later in the
	//! task switch process..."
	new_task = kmemalign(PAGE_SIZE, sizeof(task_t), GFP_KERNEL);
	if (new_task == NULL) {
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: cannot allocate task structure for '%s'\n",
			__FUNCTION__, curr_task->name);

		sched_leave_critical_region();
		return NULL;
	}

	// Add the task into the zombie queue, so if the current task
	// is killed the space allocated until now can be freed by the
	// kpager daemon.
	add_queue(&zombie_queue, new_task);
	new_task->state = TASK_NEW;

	// Setup segment registers.
	new_task->tss.ds = regs->ds;
	new_task->tss.es = regs->es;
	new_task->tss.fs = 0;
	new_task->tss.gs = 0;

	// Setup general registers.
	new_task->tss.edi = regs->edi;
	new_task->tss.esi = regs->esi;
	new_task->tss.ebp = regs->ebp;
	new_task->tss.ebx = regs->ebx;
	new_task->tss.edx = regs->edx;
	new_task->tss.ecx = regs->ecx;

	// Setup EFLAGS register.
	new_task->tss.eflags = regs->eflags;

	// Setup stack pointer.
	new_task->tss.ss = regs->uss;
	new_task->tss.esp = regs->uesp;

	// Setup instruction pointer.
	new_task->tss.cs = regs->cs;
	new_task->tss.eip = regs->eip;

	// Return 0 to the child.
	new_task->tss.eax = 0;

	// Create the pl0-stack.
	new_task->pl0_stack = (size_t)kmalloc(STACK_SIZE, GFP_KERNEL);
	if (new_task->pl0_stack == NULL) {
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: cannot allocate pl0 stack for '%s'\n",
			__FUNCTION__, curr_task->name);
		kfree(new_task);

		sched_leave_critical_region();
		return NULL;
	}

	// Copy the pl0 stack of the father.
	memset((void *)(new_task->pl0_stack), 0, STACK_SIZE);

	// Setup the privileged stack.
	new_task->tss.ss0 = KERNEL_STACK;
	new_task->tss.esp0 = ALIGN_DOWN(
		(size_t)(new_task->pl0_stack) + STACK_SIZE - sizeof(uint32_t),
		sizeof(uint32_t));

	// Setup the TSS.
	new_task->tss_sel = setup_GDT_entry(sizeof(tss_IO_t), (size_t)&(new_task->tss), TSS_SEG, 0);
	if (new_task->tss_sel == NULL) {
		printk(KERN_ERR
			"%s(): out of GDT entries!!! "
			"cannot create task '%s'\n",
			__FUNCTION__,
			curr_task->name);
		kfree(new_task);
		kfree((void *)(new_task->pl0_stack));

		sched_leave_critical_region();
		return NULL;
	}

	// Set the privilege level.
	new_task->privilege = curr_task->privilege;

	// Set the task type.
	new_task->flags.type = PROCESS_TYPE;

	// Create the virtual space of the task.
	new_vspace(new_task);
	if (new_task->pdbr == NULL) {
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: out of memory! cannot create pdbr for '%s'!\n",
			__FUNCTION__, curr_task->name);
		kfree(new_task);
		kfree((void *)(new_task->pl0_stack));
		remove_GDT_entry(new_task->tss_sel);

		sched_leave_critical_region();
		return( NULL );
	}

	// Copy all the user space of the father.
	// Map each page table as read-only to know when a COW operation
	// occurs for the first time.
	for (i = 0; i < PDE_INDEX(K_VIR_START); i++) {
		int __ret;

		curr_task->pdbr[i] &= (~P_WRITE);
		new_task->pdbr[i] = curr_task->pdbr[i];

		if ((curr_task->pdbr[i] & P_PRESENT) == P_PRESENT) {
			__ret = set_shared_page(PAGE_ALIGN(curr_task->pdbr[i]));
			if (__ret < 0) {
				printk(KERN_ERR
					"%s: out of memory! cannot create pdbr for '%s'!\n",
					__FUNCTION__, curr_task->name);
				kfree(new_task);
				kfree((void *)(new_task->pl0_stack));
				remove_GDT_entry(new_task->tss_sel);

				sched_leave_critical_region();
				return NULL;
			}
		}
	}

	// Setup the task page directory address.
	new_task->tss.cr3 = vir_to_phys((size_t)(new_task->pdbr));

	// Setup the stack.
	new_task->stack = curr_task->stack;

	// Setup the heap.
	new_task->heap_start = curr_task->heap_start;
	new_task->heap_size = curr_task->heap_size;
	init_MUTEX(&(new_task->heap_sem));

	// Setup the IO port mapping.
	new_task->tss.io_map_addr = sizeof(tss_t);
	memsetl(new_task->tss.io_map, 0xffffffff, IO_MAP_SIZE);

	// Copy the process name.
	// strcpy() here is safe, because we have checked the size
	// of the string when the father task was created.
	strcpy(new_task->name, curr_task->name);

	// Set the process credentials.
	new_task->pid = new_pid();
	new_task->uid = curr_task->uid;
	new_task->euid = curr_task->euid;
	new_task->suid = curr_task->suid;
	new_task->fsuid = curr_task->fsuid;
	new_task->gid = curr_task->gid;
	new_task->egid = curr_task->egid;
	new_task->sgid = curr_task->sgid;
	new_task->fsgid = curr_task->fsgid;

	// Get the father.
	new_task->father = curr_task;

	// Set the console.
	new_task->console = curr_task->console;

	// Setup the priority.
	new_task->priority = new_task->counter = HIGH_PRIORITY;

	// Insert the task into the ready queue.
	rem_queue(&zombie_queue, new_task);
	add_queue(&ready_queue, new_task);
	new_task->state = TASK_READY;

	sched_leave_critical_region();

	// This is a little trick... Because we exit
	// from a very long critical region we call
	// the scheduler to enforce a new task selection.
	schedule();

	// Return the child pid to the father.
	return new_task->pid;
}

// --- Task execution module ------------------------------------------ //

//! \brief Execute a file creating a new task.
//! \param filename
//!	The name of the file to execute.
//! \param argc
//!	The number of arguments passed to the new process.
//! \param argv
//!	A pointer to the argument strings.
//! \return A pointer to the new-created task structure.
//! \exception NULL If an error occurs (in particular out-of-memory).
/*!
 *	This routine can execute only ELF32 files. The file is loaded
 *	in a temporary buffer, then the sections are copied into memory
 *	and the task structure is initialized. At the end of this the
 *	new task is putted into the ready queue.
 */
task_t *exec_file( int dir, char *filename, int argc, char **argv )
{
	task_t *new_task;
	size_t entry;
	void *filebuffer;
	int filesize = fat12_file_size( dir, filename );

	if ( filesize <= 0 )
		return( NULL );

	// Load the ELF32 file into the buffer.
	filebuffer = kmalloc( filesize, GFP_KERNEL );
	if( filebuffer == NULL )
	{
		return( NULL );
	}

	// Get the entry point.
	entry = elf32_load_file( filebuffer, filesize, dir, filename );
	if ( entry == NULL )
	{
		kfree( filebuffer );

		return( NULL );
	}

	sched_enter_critical_region();

	// --- Create the task structure ------------------------------ //
	//!
	//! IA-32 Intel(R) Architecture Software Developer's Manual
	//! Volume 3: System Programming Guide - Chapter 6 - reports:
	//!
	//! "If paging is used, care should be taken to avoid placing a
	//! page boundary within the part of the TSS that the processor
	//! reads during a task switch (the first 104 bytes). If a page
	//! boundary is placed within this part of the TSS, the pages on
	//! either side of the boundary must be present at the same time
	//! and contiguous in physical memory.
	//!
	//! The reason for this restriction is that when accessing a TSS
	//! during a task switch, the processor reads and writes into
	//! the first 104 bytes of each TSS from contiguous physical
	//! addresses beginning with the physical address of the first
	//! byte of the TSS. It may not perform address translation at a
	//! page boundary if one occurs within this area. So, after the
	//! TSS access begins, if a part of the 104 bytes is not both
	//! present and physically contiguous, the processor will access
	//! incorrect TSS information, without generating a page-fault
	//! exception. The reading of this incorrect information will
	//! generally lead to an unrecoverable exception later in the
	//! task switch process..."
	new_task = kmemalign( PAGE_SIZE, sizeof(task_t), GFP_KERNEL );
	if( new_task == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: cannot allocate task structure for '%s'\n",
			__FUNCTION__, filename);
		kfree( filebuffer );

		sched_leave_critical_region();
		return( NULL );
	}

	// Add the task into the zombie queue, so if the current task
	// is killed the space allocated until now can be freed by the
	// kpager daemon.
	add_queue( &zombie_queue, new_task );
	new_task->state = TASK_NEW;

	// Create the pl0-stack.
	new_task->pl0_stack = (size_t)kmalloc( STACK_SIZE, GFP_KERNEL );
	if( new_task->pl0_stack == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: cannot allocate pl0 stack for '%s'\n",
			__FUNCTION__, filename);
		kfree( filebuffer );
		kfree( new_task );

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
			"cannot create task '%s'\n",
			__FUNCTION__,
			filename );
		kfree( filebuffer );
		kfree( new_task );
		kfree( (void *)(new_task->pl0_stack) );

		sched_leave_critical_region();
		return( NULL );
	}

	// Set the privilege level.
	new_task->privilege = USER_PRIVILEGE;

	// Set the task type.
	new_task->flags.type = PROCESS_TYPE;

	// Create the virtual space of the task.
	new_vspace( new_task );
	if ( new_task->pdbr == NULL )
	{
		// Out of virtual memory!!!
		printk(KERN_ERR
			"%s: out of memory! cannot create pdbr for '%s'!\n",
			__FUNCTION__, filename);
		kfree( filebuffer );
		kfree( new_task );
		kfree( (void *)(new_task->pl0_stack) );
		remove_GDT_entry( new_task->tss_sel );

		sched_leave_critical_region();
		return( NULL );
	}

	// Setup the task page directory address.
	new_task->tss.cr3 = vir_to_phys( (size_t)(new_task->pdbr) );

	// Temporary switch to the new address space.
	task_switch_mmu( curr_task, new_task );

	// Create the task stack.
	new_task->tss.ss = USER_STACK | 3;
	new_task->tss.esp = (size_t)task_setup_stack( argc, argv, TASK_STACK_START );

	// Initialize the user heap.
	umalloc_init( new_task, TASK_HEAP_START, TASK_HEAP_DEFAULT_SIZE );

	// Copy the file sections into memory.
	entry = elf32_copy_sections( filebuffer );
	if ( entry == NULL )
	{
		kfree( filebuffer );
		kfree( new_task );
		kfree( (void *)(new_task->pl0_stack) );
		remove_GDT_entry( new_task->tss_sel );
		free_temp_page( new_task->pdbr, TRUE );

		sched_leave_critical_region();
		return( NULL );
	}

	// Restore the old address space.
	task_switch_mmu( new_task, curr_task );

	// Setup the IO port mapping.
	new_task->tss.io_map_addr = sizeof(tss_t);
	memsetl( new_task->tss.io_map, 0xffffffff, IO_MAP_SIZE );

	// Setup general registers.
	new_task->tss.ds = new_task->tss.es =
	new_task->tss.fs = new_task->tss.gs = USER_DATA | 3;

	// Setup the eflags register.
	new_task->tss.eflags = EFLAGS_IOPL0 | EFLAGS_IF | 0x02;

	// Setup starting address (program counter).
	new_task->tss.cs = USER_CODE | 3;
	new_task->tss.eip = (size_t)entry;

	// Store the process name.
	// The last character must be ever '\0' (end of string).
	strncpy( new_task->name, filename, sizeof(new_task->name) - 2 );

	// Set the process credentials.
	new_task->pid = new_pid();
	new_task->uid = new_task->euid =
	new_task->suid = new_task->fsuid = 1;
	new_task->gid = new_task->egid =
	new_task->sgid = new_task->fsgid = 1;

	// Set the parent.
	new_task->father = curr_task;

	// Set the console.
	if( curr_task )
	{
		new_task->console = curr_task->console;
	}

	// Setup the priority.
	new_task->priority = new_task->counter = HIGH_PRIORITY;

	// Insert the task into the ready queue.
	rem_queue( &zombie_queue, new_task );
	add_queue( &ready_queue, new_task );
	new_task->state = TASK_READY;

	sched_leave_critical_region();

	kfree( filebuffer );

	// This is a little trick... Because we exit
	// from a very long critical region we call
	// the scheduler to enforce a new task selection.
	schedule();

	return( new_task );
}

// --- END of Task execution module ----------------------------------- //

//! \brief Kill a task by the pid.
//! \param pid The process id.
//! \return
//!	\li #TRUE if the task has been successfully killed
//!	\li #FALSE if the task cannot be killed.
/*!
 *	This routine move the task having the \a pid from the ready queue
 *	to the zombie queue. Then the kpager() daemon should provide
 *	to free the memory and resources owned by this task.
 */
bool kill( pid_t pid )
{
	// Queues where to search the processes.
	static queue_t **q[] = { &ready_queue, &wait_queue };

	task_t *p;
	bool retval = FALSE;
	register int i, n;
	queue_t *entry;
	int flag_repeat;

	// No running processes(???) => quit!
	if ( curr_task == NULL )
		return( FALSE );

	sched_enter_critical_region();

	// Explore each task queue.
	for( i = 0; i < _countof( q ); ++i )
	{
		queue_for_each( entry, n, *(q[ i ]) )
		{
			// Get the next task in the queue.
			p = queue_get_entry( entry );

			if( p->pid == pid )
			{
				set_task_state( p, TASK_ZOMBIE );

				// Wake-up the father.
				if( p->father != NULL )
					wakeup_task( p->father );

				// Task killed!
				retval = TRUE;
				goto found;
			}
		}
	}
found:
	flag_repeat = 0;
	for( i = 0; i < _countof( q ); ++i )
	{
		queue_for_each( entry, n, *(q[ i ]) )
		{
			// Get the next task in the queue.
			p = queue_get_entry( entry );

			if( p->father != NULL )
			{
				if( p->father->state == TASK_ZOMBIE )
				{
					p->father = NULL;
					if( p->flags.type == THREAD_TYPE )
					{
						set_task_state( p, TASK_ZOMBIE );
						flag_repeat = 1;
					}
				}
			}
		}
	}
	if( flag_repeat )
		goto found;

	sched_leave_critical_region();

	// Check if the task has killed itself.
	if( curr_task->state == TASK_ZOMBIE )
	{
		// Call the scheduler.
		schedule();
	}

	return( retval );
}

//! \brief Kill the current running task.
//! \param n The return code.
/*!
 *	This routine move the current task from the ready queue
 *	to the zombie queue. Then the kpager() daemon should provide
 *	to free the memory and resources owned by this task.
 */
void auto_kill(int n)
{
	kill( curr_task->pid );
	for(;;);
}

//! \brief Wait for the exit of a child.
//! \param pid
//!	\li The child process id
//!	\li -1 to wait for the exit of a generic child.
//! \param status Reports the status of the child.
//! \param options Options flags.
//! \return
//!	The pid of the child or -1 if the process has not been
//!	found.
/*!
 *	\todo \a status flags and \a options flags not yet implemented!
 */
pid_t waitpid(pid_t pid, int *status, int options)
{
	// Queues where to search the processes.
	static queue_t **q[] = { &ready_queue, &wait_queue };
	task_t *p;
	register int i, n;
	queue_t *entry;
	bool found = FALSE;

	// A task can't wait for yourself!
	if (curr_task->pid == pid)
		return(0);

repeat:
	sched_enter_critical_region();

	// Search into every task queue.
	for( i=0; i < _countof( q ); ++i )
	{
		queue_for_each( entry, n, *(q[i]) )
		{
			p = queue_get_entry( entry );
			if ( (p->father == curr_task) && ((pid==-1) || (p->pid==pid)) )
			{
				sched_leave_critical_region();

				// Found!
				found = TRUE;

				// Update the pid of the child that we
				// are waiting (when pid is -1).
				pid = p->pid;

				// Move the current task (father) into
				// the wait queue and wait.
				sleep_task( curr_task );

				// Task waked-up! Check if the child
				// that we are waiting is dead.
				goto repeat;
			}
		}

	}
	sched_leave_critical_region();

	// Return the child pid or -1 if the process has not been found	//
	return( (found) ? pid : -1 );
}
/** @} */ // end of MTTaskRoutines

// --- Initialization --------------------------------------------------//

/** \ingroup Multitasking
 *  \defgroup MTInit Task Initialization
 *  The multitasking initialization routine.
 *  @{
 */

//! \brief Initialize the multitasking management.
void __INIT__ init_multitasking()
{
	// Create the "init" task.
	curr_task = create_process( NULL, 0, NULL, "init", KERNEL_PRIVILEGE );

	// Set the console.
	curr_task->console = 1;

	// After the init thread is out it will become the idle task.
	idle_task = curr_task;

	// Load task register.
	__asm__ __volatile__ ("ltr %0" : : "a" (curr_task->tss_sel));
}
/** @} */ // end of MTInit

// --- Debug procedures ------------------------------------------------//

/** \ingroup Multitasking
 *  \defgroup MTDebug Debug
 *  Debug procedures for multitasking.
 *  @{
 */

//! \brief
//!	Print the state of every process from the ready, wait
//!	and zombie queues.
void ps()
{
	// Queues where to search the processes.
	static queue_t **q[] = { &ready_queue, &wait_queue, &zombie_queue };
	static char queue_state[] = "RWZ";

	queue_t *entry;
	task_t *p;
	int i, n;

	// Header.
	set_color( WHITE );
	kprintf("OWNER     PID       STATE       CONSOLE     COMMAND\n");
	set_color( DEFAULT_COLOR );

	sched_enter_critical_region();

	// Look in every queues.
	for( i=0; i < _countof( q ); ++i )
	{
		queue_for_each( entry, n, *(q[i]) )
		{
			p = queue_get_entry( entry );

			if( p->privilege == KERNEL_PRIVILEGE )
			{
			}
			else
			{
			}
			// Print process informations.
			kprintf( "%s",
				p->privilege == KERNEL_PRIVILEGE ?
				"kernel    " :
				"user      " );
			kprintf( "%-12u", p->pid );
			kprintf( "%c", queue_state[i] );
			if( p->console )
				kprintf( "         tty%-9d", p->console );
			else
				kprintf( "         ?           " );

			// Print the name.
			kprintf( "%s\n", p->name );
		}
	}

	sched_leave_critical_region();
}

/** @} */ // end of MTTaskRoutines
