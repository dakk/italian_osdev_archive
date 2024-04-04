/*!	\file kernel/kpagerd.c
 *	\brief The page manager.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-12-08\n
 *		2004-12-08 Andrea Righi:
 *			Added the fork() syscall.\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <arch/mem.h>
#include <arch/paging.h>
#include <kernel/cow.h>
#include <kernel/debug.h>
#include <kernel/kmalloc.h>
#include <kernel/umalloc.h>
#include <kernel/sched.h>

#include <arch/clock.h>

extern queue_t *zombie_queue;

//! KPager daemon task structure.
task_t *kpagd = NULL;

//! \brief
//!	This special kernel thread provides to free the memory space and
//!	resources owned by the zombie tasks.
void kpagerd(int argc, char **argv)
{
	task_t *t;
	size_t addr;
	size_t __pde;

	for (;;) {
		// Look into the zombie queue.
		sched_enter_critical_region();
		t = pick_queue(&zombie_queue);
		sched_leave_critical_region();

		if (t == NULL) {
			// No task in the zombie queue => IDLE!
			sleep_task(curr_task);
			continue;
		}

		// New task or thread to be freed.
		switch (t->flags.type) {
			case THREAD_TYPE:
				// Free the task stack.
				if (t->privilege == USER_PRIVILEGE)
				if (t->father != NULL)
				if (t->father->state != TASK_ZOMBIE)
				if (t->pdbr != NULL) {
					// Free the stack of the thread in the
					// father's heap space.
					do_ufree(t->father, (void *)(t->stack));
				}
			break;

			case PROCESS_TYPE:
				if (t->pdbr != NULL) {
					sched_enter_critical_region();

					// Temporary switch to the task
					// address space.
					task_switch_mmu(kpagd, t);

					// Free the user address space.
					addr = 0;
					while (addr < K_VIR_START) {
						__pde = *ADDR_TO_PDE(addr);
						if ((__pde & P_PRESENT) == P_PRESENT) {
							delete_page(addr);
							addr += PAGE_SIZE;
						} else {
							addr = PAGE_DIR_ALIGN_UP(addr);
						}
					}
					// Restore the kpager address space.
					task_switch_mmu(t, kpagd);

					sched_leave_critical_region();

					// Free the page directory of the task.
					free_temp_page(t->pdbr, TRUE);
				}
			break;
		}

		// Free the pl0-stack.
		kfree((void *)(t->pl0_stack));

		// Destroy the TSS.
		remove_GDT_entry(t->tss_sel);

		// The task is dead.
		sched_enter_critical_region();
		rem_queue(&zombie_queue, t);
		sched_leave_critical_region();

		// Free the task structure.
		kfree((void *)t);
	}
}
