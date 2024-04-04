/*!	\file include/kernel/sched.h
 *	\brief Headers for the scheduler.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-18\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef SCHED_H
#define SCHED_H

#include <const.h>
#include <kernel/task.h>
#include <kernel/queue.h>

//! Minimum priority level.
#define LOW_PRIORITY		( 0 )

//! Maximum priority level.
#define HIGH_PRIORITY		( -16 )

//! Low priority level for real-time tasks.
#define RT_LOW_PRIORITY		( -64 )

//! High priority level for real-time tasks.
#define RT_HIGH_PRIORITY	( -128 )

__INLINE__ task_t *get_curr_task();

__INLINE__ void sched_enter_critical_region(void);
__INLINE__ void sched_leave_critical_region(void);
__INLINE__ int sched_is_disabled( void );
__INLINE__ void sched_refresh_queues();

void schedule( void );

//! Set the task state.
#define set_task_state( t, state_value ) \
	do { (t)->state = (state_value); sched_refresh_queues(); } \
	while( 0 )

//! Set the current task state.
#define set_current_state( state_value ) \
	do { curr_task->state = (state_value); sched_refresh_queues(); } \
	while( 0 )

//! \brief Sleep a process if it is awake.
//! \param p The structure of the task to sleep.
static __INLINE__ void sleep_task(task_t *p)
{
	if( p->state != TASK_WAIT )
		set_task_state( p, TASK_WAIT );
	schedule();
}

//! \brief Wakeup a process if it is sleeping.
//! \param p The structure of the task to wakeup.
static __INLINE__ void wakeup_task(task_t *p)
{
	if( p->state != TASK_READY )
		set_task_state( p, TASK_READY );
}

//! Wake-up all the task into the wait queue.
static __INLINE__ void __wakeup_queue( wait_queue_t *waitq )
{
	unsigned long flags;
	task_t *p;
	pid_t pid;
	queue_t *entry, *dummy;
	int n;

	spin_lock_irqsave( &(waitq->lock), flags );

	queue_for_each_safe( entry, dummy, n, waitq->list )
	{
		pid = (pid_t)queue_get_entry( entry );
		p = get_task_info( pid );
		if( p != NULL )
		{
			wakeup_task( p );
		}
		queue_del_entry( &(waitq->list), entry );
	}

	spin_unlock_irqrestore( &(waitq->lock), flags );
}

//! Wake-up all the task into the wait queue.
#define wakeup_queue( waitq ) \
	__wakeup_queue( &(waitq) )

//! Wait the current task for an event condition.
#define wait_event( waitq, condition )				\
	do							\
	{							\
		uint32_t flags;					\
		local_irq_save( flags );			\
		for(;;)						\
		{						\
			disable();				\
			if( condition )				\
				break;				\
			add_wait_queue( &(waitq), (void *)(curr_task->pid) );	\
			enable();				\
			sleep_task( curr_task );		\
		}						\
		local_irq_restore( flags );			\
	} while( 0 );

#endif
