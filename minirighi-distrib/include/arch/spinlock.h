/*!	\file include/arch/spinlock.h
 *	\brief Simple spinlock semaphore.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-08
 *	\note Thanks to Linux.
 */

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <const.h>
#include <arch/i386.h>

//! Simple spinlock semaphore structure.
typedef struct spinlock
{
	volatile unsigned long lock;
} spinlock_t;

//! The spinlock unlocked value.
#define SPINLOCK_UNLOCKED	{ 1 }

//! The spinlock locked value.
#define SPINLOCK_LOCKED		{ 0 }

//! Declare a simple spinlock unlocked.
#define DECLARE_SPINLOCK( name ) \
	spinlock_t name = SPINLOCK_UNLOCKED

//! Declare a simple spinlock locked.
#define DECLARE_SPINLOCK_LOCKED( name ) \
	spinlock_t name = SPINLOCK_LOCKED

//! Check if a spinlock is locked.
#define spin_is_locked( s ) \
	( *(volatile signed char *)(&((s)->lock))<=0 )

//! Initialize a spinlock.
#define spin_lock_init( x ) \
	do { *(x) = (spinlock_t)SPINLOCK_UNLOCKED; } while( 0 )

//! Lock the semaphore.
static __INLINE__ void spin_lock( spinlock_t *lock )
{
	__asm__ __volatile__(
		"1: lock ; decb %0\n"
		"jge 3f\n"
		"2:\n"
		"cmpb $0, %0\n"
		"rep; nop\n"
		"jle 2b\n"
		"jmp 1b\n"
		"3:\n"
		: "=m"(lock->lock) : : "memory");
}

//! Unlock the semaphore.
static __INLINE__ void spin_unlock( spinlock_t *lock )
{
	__asm__ __volatile__(
		"movb $1, %0"
		: "=m"(lock->lock) : : "memory");
}

//! Disable irq and lock the semaphore.
#define spin_lock_irqsave(lock, flags) \
	do { local_irq_save(flags); spin_lock(lock); } while( 0 )

//! Unlock the semaphore and restore eflags.
#define spin_unlock_irqrestore(lock, flags) \
	do { spin_unlock(lock); local_irq_restore(flags); } while( 0 )

#endif
