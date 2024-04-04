/*!	\file arch/i386/kernel/delay.c
 *	\brief Precise dalay loops for i386.
 *	\author Andrea Righi.
 *	\date Last update: 2004-03-31
 *
 *	The __delay function must _NOT_ be inlined as its execution
 *	time depends wildly on alignment on many x86 processors.
 *	The additional jump magic is needed to get the timing stable
 *	on all the CPU's we have to worry about.
 *
 *	These routines are taken from Linux to allow the exact
 *	evaluation of the bogomips.
 */

#include <arch/clock.h>
#include <arch/cpu.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <kernel/debug.h>

#include <arch/delay.h>

extern volatile unsigned long ticks;

extern cpuinfo_t cpu;

//! How many loops the CPU does in a clock tick.
unsigned long loops_per_tick = ( 1 << 12 );

//! Function used to perform the precise delay.
//! Precise i386 delay using loops.
static void __delay_loop( unsigned long loops )
{
	int d0;
	__asm__ __volatile__ (
		"jmp 1f\n"
		".align 16\n"
		"1:\tjmp 2f\n"
		".align 16\n"
		"2:\tdecl %0\n"
		"jns 2b"
		: "=&a"(d0)
		: "0"(loops) );
}

//! Precise i386 delay using timestamp counter.
static void __delay_timestamp( unsigned long loops )
{
	unsigned long register start, now, dummy;

	rdtsc( start, dummy );
	do
	{
		rep_nop();
		rdtsc( now, dummy );
	} while( (now - start) < loops );
}

//! Routine to use to perform the precise delay.
static void (*__do_delay)( unsigned long loops);

//! Precise i386 delay.
static void __delay( unsigned long loops )
{
	__do_delay( loops );
}

//! Use this inline routine to evaluate loops.
static __INLINE__ void __const_udelay( unsigned long xloops )
{
	int d0;
	__asm__ __volatile__ (
		"mull %0"
		: "=d"(xloops), "=&a"(d0)
		: "1"(xloops), "0"(loops_per_tick) );
	__delay( xloops * HZ );
}

//! Delay some microseconds.
void udelay( unsigned long usecs )
{
	// ( 2**32 / 1000000 ).
	__const_udelay( usecs * 0x000010c6 );
}

//! Delay some nanoseconds.
void ndelay( unsigned long nsecs )
{
	// ( 2**32 / 1000000000 ) rounded up.
	__const_udelay( nsecs * 0x00000005 );
}

//! Calibrate the loops_per_tick value.
void __INIT__ calibrate_loops_per_tick( void )
{
	unsigned long prev_ticks, loopbit;
	int lps_precision = 8;

	// Initialize the correct routine to perform the delay.
	__do_delay = ( cpu.feature.flags.edx.tsc ) ?
		__delay_timestamp :
		__delay_loop;

	// Begin the evaluation.
	while( loops_per_tick <<= 1 )
	{
		// Wait for the start of the next tick.
		prev_ticks = ticks;
		while( prev_ticks == ticks );
		// Perform the evaluation.
		prev_ticks = ticks;
		__delay( loops_per_tick );
		prev_ticks = ticks - prev_ticks;
		// Evaluation done; repeat until the loops
		// are big enough.
		if( prev_ticks )
			break;
	}

	// Fine calibration; precision = 8 bit.
	loops_per_tick >>= 1;
	loopbit = loops_per_tick;
	while( lps_precision-- && (loopbit >>= 1) )
	{
		loops_per_tick |= loopbit;
		prev_ticks = ticks;
		while( prev_ticks == ticks );
		prev_ticks = ticks;
		__delay( loops_per_tick );
		if( ticks != prev_ticks )
			loops_per_tick &= ~loopbit;
	}

	// Update the CPU loops_per_tick value.
	cpu.loops_per_tick = loops_per_tick;

	// Print the BogoMIPS evaluation.
	printk( KERN_INFO "%s(): %u loops per tick (%lu.%02lu BogoMIPS)\n",
		__FUNCTION__,
		loops_per_tick,
		loops_per_tick / (500000 / HZ),
		(loops_per_tick / (5000 / HZ)) % 100 );
}
