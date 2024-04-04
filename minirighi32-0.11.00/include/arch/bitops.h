/*!	\file include/arch/bitops.h
 *	\brief Bit operations.
 *	\note Copyright 1992, Linus Torvalds.
 */

#ifndef BITOPS_H
#define BITOPS_H

#include <const.h>

#define ADDR	( *(volatile long *)addr )

//! Determine whether a bit is set.
static __INLINE__ int test_bit(int nr, volatile void * addr)
{
	int oldbit;

	__asm__ __volatile__ (
		"btl %2, %1\n"
		"sbbl %0, %0\n"
		:"=r"(oldbit)
		: "m"(ADDR), "Ir"(nr) );

	return( oldbit );
}

//! Atomically set a bit in memory.
static __INLINE__ void set_bit(int nr, volatile void * addr)
{
	__asm__ __volatile__ (
		"lock ; btsl %1, %0"
		:"=m"(ADDR)
		: "Ir"(nr) );
}

//! Atomically clear a bit in memory.
static __INLINE__ void clear_bit(int nr, volatile void * addr)
{
	__asm__ __volatile__ (
		"lock ; btrl %1, %0"
		: "=m"(ADDR)
		: "Ir"(nr) );
}

//! Toggle a bit in memory (complement).
static __INLINE__ void change_bit( int nr, volatile void *addr )
{
	__asm__ __volatile__ (
		"lock ; btcl %1, %0"
		: "=m"(ADDR)
		: "Ir"(nr) );
}

//! Set a bit and return its old value.
static __INLINE__ int test_and_set_bit( int nr, volatile void *addr )
{
	int oldbit;

	__asm__ __volatile__ (
		"lock\n"
		"btsl %2, %1\n"
		"sbbl %0, %0"
		: "=r"(oldbit), "=m"(ADDR)
		: "Ir"(nr)
		: "memory" );
	return oldbit;
}

#endif
