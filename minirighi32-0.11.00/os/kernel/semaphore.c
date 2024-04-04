/*!	\file kernel/semaphore.c
 *	\brief Spinlock semaphores.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-03-06 Andrea Righi:
 *			Changed ugly binary semaphores into
 *			wonderful spinlock semaphores!
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <arch/atomic.h>
#include <arch/spinlock.h>
#include <kernel/debug.h>
#include <kernel/queue.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>

//! The spinlock for mutual exclusion for semaphores.
DECLARE_SPINLOCK( semaphore_lock );

//! Routine invoked when a DOWN operation fails to
//! enter into the critical region.
void __down_failed( semaphore_t *sem )
{
	task_t *current = get_curr_task();
	uint32_t flags;
	task_t *t;

	spin_lock_irqsave( &semaphore_lock, flags );

	add_queue( &(sem->waitq), current );
	sem->sleepers++;
	for(;;)
	{
		int sleepers = sem->sleepers;

		if( !atomic_add_negative(sleepers - 1, &(sem->count)) )
		{
			sem->sleepers = 0;
			break;
		}
		sem->sleepers = 1;

		spin_unlock_irqrestore( &semaphore_lock, flags );

		// Sleep this task and call the scheduler.
		sleep_task( current );

		// Re-try to unlock the semaphore.
		spin_lock_irqsave( &semaphore_lock, flags );
	}
	// Remove the current task from the spinlock wait queue.
	rem_queue( &(sem->waitq), current );

	// Wake-up the next task in the spinlock queue.
	t = queue_get_entry( sem->waitq );
	if( t != NULL )
		wakeup_task( t );

	// Done.
	spin_unlock_irqrestore( &semaphore_lock, flags );
}

//! Routine invoked when a UP operation is invoked
//! and some tasks are waiting in the spinlock queue.
void __up_wakeup( semaphore_t *sem )
{
	task_t *t;
	uint32_t flags;

	spin_lock_irqsave( &semaphore_lock, flags );

	t = queue_get_entry( sem->waitq );
	if( t!=NULL )
		wakeup_task( t );

	spin_unlock_irqrestore( &semaphore_lock, flags );
}
