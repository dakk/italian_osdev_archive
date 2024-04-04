/*!	\file kernel/clock.c
 *	\brief The system clock manager.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-20
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <sys/times.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <kernel/floppy.h>
#include <kernel/sched.h>

#include <arch/clock.h>

//! System ticks.
volatile unsigned long ticks = 0;

//! \brief Reset the clock for rebooting
void stop_clock()
{
	uint32_t flags;

	local_irq_save( flags );

	// Set timer on.
	outportb(TIMER_MODE, SQUARE_WAVE);
	// Timer low byte.
	outportb(TIMER0, 0);
	// Timer hight byte.
	outportb(TIMER0, 0);

	local_irq_restore( flags );
}

//! \brief Delay some milliseconds
//! \param millisec How many milliseconds you want to delay.
void delay(dword millisec)
{
	unsigned long count, elapsed;
	unsigned long flags;

	// Start timer.
	elapsed = 0;

	save_flags(flags);

	for(;;) {
		if (elapsed >= millisec) break;

		// Poll ticks in memory.
		count = ticks;

		enable();
		schedule();

		while (count==ticks);
		elapsed += (ticks-count) * 1000 / HZ;
	}

	restore_flags(flags);
}

//! \brief Initialize a timeout variable.
//! \param t The timeout variable.
//! \param millisec
//! 	How many milliseconds have to elapse before the timeout occurs.
void set_timeout(timeout_t *t, unsigned int millisec)
{
	t->start_ticks = ticks;
	t->ticks_to_wait = millisec * HZ / 1000;
}

//! \brief Check if a timeout is elapsed.
//! \param t The timeout variable.
bool is_timeout(timeout_t *t)
{
	register uint32_t elapsed = ticks - t->start_ticks;
	return( elapsed >= t->ticks_to_wait );
}

//! \brief Return the system ticks.
//! \return The system ticks.
unsigned long sys_get_ticks()
{
	return( ticks );
}

//! Get the system timers.
/**
 *	\todo
 *		Return the struct tms structure.
 */
clock_t sys_times( struct tms *buf )
{
	if( buf==NULL )
		return( ticks );

	// Fill the tms buffer.
	// --- TODO ---
	buf->tms_utime = 0;
	buf->tms_stime = 0;
	buf->tms_cutime = 0;
	buf->tms_cstime = 0;
	// --- TODO ---

	return( ticks );
}

//! This is the clock handler. It is invoked every clock tick.
void clock_handler( irq_context_t *c )
{
	// Increment the system clock ticks.
	ticks++;

	// Put here every periodical task...
	floppy_thread();

	// Ack the irq line.
	end_of_irq( c->IRQ );

	// Call the scheduler.
	schedule();
}

//! \brief Initialize channel 0 of the 8253A timer
void __INIT__ init_clock()
{
	uint32_t flags;

	local_irq_save( flags );

	// Set timer on.
	outb( TIMER_MODE, SQUARE_WAVE );
	// Timer low byte.
	outb( TIMER0, TIMER_COUNT & 0xFF );
	// Timer high byte.
	outb( TIMER0, TIMER_COUNT >> 8 );

	local_irq_restore( flags );

	// Install the clock handler.
	install_trap_handler( TIMER_IRQ, (void *)&clock_handler );
}
