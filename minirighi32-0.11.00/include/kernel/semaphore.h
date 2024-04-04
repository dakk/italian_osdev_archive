/*!	\file include/kernel/semaphore.h
 *	\brief Semaphore header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-06
 *	\note Copyright (C) 2003 Andrea Righi
 */

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <const.h>
#include <arch/atomic.h>
#include <kernel/queue.h>

//! Spinlock semaphore structure.
typedef struct semaphore
{
	atomic_t count;
	int sleepers;
	queue_t *waitq;
} semaphore_t;

//! Another semaphore type definition.
typedef semaphore_t sem_t;

//! Declare a spinlock semaphore unlocked.
#define DECLARE_MUTEX( name ) \
	semaphore_t name = { atomic_init(1), 0, NULL }

//! Declare a spinlock semaphore locked.
#define DECLARE_MUTEX_LOCKED( name ) \
	semaphore_t name = { atomic_init(0), 0, NULL }

//! Initialize a spinlock semaphore.
static __INLINE__ void init_MUTEX( semaphore_t *sem )
{
	atomic_set( &(sem->count), 1 );
	sem->sleepers = 0;
	sem->waitq = NULL;
}

//! Initialize a locked spinlock semaphore.
static __INLINE__ void init_MUTEX_LOCKED( semaphore_t *sem )
{
	atomic_set( &(sem->count), 0 );
	sem->sleepers = 0;
	sem->waitq = NULL;
}

//! Check if a semaphore is locked
#define sem_is_locked( s ) \
		( atomic_read((&((s)->count)))!=1 )

//! Semaphore "DOWN" routine.
static __INLINE__ void DOWN( semaphore_t *sem )
{
	__asm__ __volatile__ (
		"lock ; decl %0\n"	// --( sem->count );
		"jge 1f\n"		// if( sem->count<0 )
		"pushl %1\n"		// {
		"call __down_failed\n"	//	__down_failed( sem );
		"popl %1\n"		// }
		"1:\n"
		: "=m"(sem->count) : "r"(sem) : "memory");
}

//! Semaphore "UP" routine.
static __INLINE__ void UP( semaphore_t *sem )
{
	__asm__ __volatile__ (
		"lock ; incl %0\n"	// ++( sem->count )
		"jg 1f\n"		// if ( sem->count<=0 )
		"pushl %1\n"		// {
		"call __up_wakeup\n"	//	__up_wakeup( sem );
		"popl %1\n"		// }
		"1:\n"
		: "=m"(sem->count) : "r"(sem) : "memory");
}

#endif
