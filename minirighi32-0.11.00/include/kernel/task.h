/*!	\file include/kernel/task.h
 *	\brief Headers for the task manager.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-06
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef TASK_H
#define TASK_H

#include <const.h>
#include <types.h>
#include <arch/interrupt.h>
#include <arch/paging.h>
#include <arch/i386.h>
#include <kernel/semaphore.h>

#include <string.h>

//! Stack size in byte for every kernel-mode task
//! (please use at least 2 pages).
#define STACK_SIZE		0x4000

//! The address where the user-privilege task stack is placed.
#define TASK_STACK_START	0xC0000000

//! The start of the heap for a user-mode task.
#define TASK_HEAP_START		0x40000000

//! The size of the heap for a user-mode task.
#define TASK_HEAP_DEFAULT_SIZE	0x40000000

#define KERNEL_PRIVILEGE	0 //!< Max privilege (kernel).
#define USER_PRIVILEGE		3 //!< Min privilege (user).

// --- TASK STATES ---------------------------------------------------- //

#define TASK_NEW		0 //!< The task is going to be created.
#define TASK_READY		1 //!< The task is ready for the CPU.
#define TASK_WAIT		2 //!< The task is waiting for I/O.
#define TASK_ZOMBIE		4 //!< The task is dying.

// --- TASK TYPES ----------------------------------------------------- //

#define THREAD_TYPE		0x00 //!< A kernel thread type.
#define PROCESS_TYPE		0x01 //!< A task type.

// --- STRUCTURES ----------------------------------------------------- //

//! TSS without I/O bit map.
typedef struct tss
{
	uint32_t	link;
	uint32_t	esp0;
	uint16_t	ss0, __ss0h;
	uint32_t	esp1;
	uint16_t	ss1, __ss1h;
	uint32_t	esp2;
	uint16_t	ss2, __ss2h;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	eflags;
	uint32_t	eax, ecx, edx, ebx;
	uint32_t	esp;
	uint32_t	ebp;
	uint32_t	esi;
	uint32_t	edi;
	uint16_t	es, __esh;
	uint16_t	cs, __csh;
	uint16_t	ss, __ssh;
	uint16_t	ds, __dsh;
	uint16_t	fs, __fsh;
	uint16_t	gs, __gsh;
	uint16_t	ldtr, __ldtrh;
	uint16_t	trace, io_map_addr;
} tss_t;

//! Size in double-words of the I/O bit map.
#define IO_MAP_SIZE	2048

//! TSS with I/O bit map.
typedef struct tss_IO
{
	uint32_t	link;
	uint32_t	esp0;
	uint16_t	ss0, __ss0h;
	uint32_t	esp1;
	uint16_t	ss1, __ss1h;
	uint32_t	esp2;
	uint16_t	ss2, __ss2h;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	eflags;
	uint32_t	eax, ecx, edx, ebx;
	uint32_t	esp;
	uint32_t	ebp;
	uint32_t	esi;
	uint32_t	edi;
	uint16_t	es, __esh;
	uint16_t	cs, __csh;
	uint16_t	ss, __ssh;
	uint16_t	ds, __dsh;
	uint16_t	fs, __fsh;
	uint16_t	gs, __gsh;
	uint16_t	ldtr, __ldtrh;
	uint16_t	trace, io_map_addr;
	uint32_t	io_map[IO_MAP_SIZE];
} tss_IO_t;

//! The flags structure for a task.
typedef struct task_flags
{
	unsigned long	type		: 1;
	unsigned long	uninterruptible	: 1;
	unsigned long	__res		: 30;
} __attribute__ ((packed)) task_flags_t;

//! The task structure.
typedef struct task
{
	//! Must be the first to allow the TSS to
	//! be page-aligned.
	tss_IO_t	tss;
	//! The selector the tss.
	uint16_t	tss_sel;

	//! The page directory of the task.
	uint32_t	*pdbr;
	//! A counter to report if is necessary to update the
	//! page directory with the global kernel page directory.
	//! In this way we preserve coherency of the kernel memory
	//! area between all tasks.
	unsigned long	pdbr_update_counter;

	// Process credentials.
	pid_t		pid;
	uid_t		uid, euid, suid, fsuid;
	gid_t		gid, egid, sgid, fsgid;

	//! A pointer to the privileged stack of the task.
	size_t		pl0_stack;
	//! A pointer to the user level stack.
	size_t		stack;

	//! The start of the task heap.
	size_t		heap_start;
	//! The size of the task heap.
	size_t		heap_size;
	//! A semaphore for the heap mutual exclusion .
	semaphore_t	heap_sem;

	//! The state of the task.
	byte		state;
	//! A pointer to the task structure of the father.
	struct task	*father;
	//! The console where task runs.
	int		console;
	//! The task flags.
	task_flags_t	flags;
	//! The privilege level of the task.
	int		privilege;
	//! The initial priority.
	int		priority;
	//! The dynamic priority.
	int		counter;
	//! The name of the task.
	char		name[ 256 ];
} task_t;

//! Get the current task privilege level.
#define suser() \
	(get_curr_task()->euid == 0)

//! Get the current task privilege level with respect to the filesystem.
#define fsuser() \
	(get_curr_task()->fsuid == 0)

//! Macro equivalent to auto_kill() (...for now).
/*!
 *	\todo
 *	The exit returning values between tasks.
 */
#define _exit( n )	auto_kill( n )

extern task_t *curr_task;
extern queue_t *ready_queue, *wait_queue, *zombie_queue;

//! \brief
//! Switch into a different task virtual address space.
static __INLINE__ void task_switch_mmu( task_t *prev, task_t *next )
{
	uint32_t cr3_prev = vir_to_phys( (size_t)(prev->pdbr) );
	uint32_t cr3_next = vir_to_phys( (size_t)(next->pdbr) );

	if( cr3_prev != cr3_next )
	{
		if( next->pdbr_update_counter != prev->pdbr_update_counter )
		{
			uint32_t flags;
			local_irq_save( flags );

			// Update the page directory for kernel address space.
			// This must be done to preserve coherency between
			// page directories of each task.
			memcpy( PDE_OFFSET(next->pdbr, K_VIR_START),
				PDE_OFFSET(prev->pdbr, K_VIR_START),
				( PDE_INDEX(PAGE_DIR_MAP) - PDE_INDEX(K_VIR_START) ) * sizeof(uint32_t) );

			// Update the pdbr counter.
			next->pdbr_update_counter = prev->pdbr_update_counter;

			local_irq_restore( flags );

		}
		// Switch into the new virtual space.
		switch_mmu( cr3_next );
	}
}

// --- Prototypes ----------------------------------------------------- //

__INLINE__ pid_t new_pid();
__INLINE__ pid_t get_pid();
__INLINE__ char *get_pname();

task_t *get_task_info( pid_t pid );

task_t * FASTCALL( create_process(void *routine, int argc, char **argv, char *pname, int privilege) );
task_t * FASTCALL( create_thread(void *routine, int argc, char **argv, char *pname) );

task_t * exec_file(int dir, char *filename, int argc, char **argv);

bool FASTCALL( kill(pid_t pid) );
pid_t FASTCALL( waitpid(pid_t pid, int *status, int options) );

void auto_kill(int n) __NORETURN__;

void init_multitasking();
void ps();

pid_t sys_create_thread(void *routine, int argc, char **argv, char *pname);
pid_t do_fork(syscall_context_t *regs);

#endif
