/*!	\file include/arch/clock.h
 *	\brief The system clock headers.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-20
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <sys/times.h>
#include <arch/interrupt.h>

/** \ingroup Kernel
 *  \defgroup KClock Clock Manager
 *  The system clock manager.
 *  @{
 */

#define LATCH_COUNT	0x00	//!< To copy chip count.
#define TIMER0		0x40	//!< I/O port for timer channel 0.
#define TIMER2		0x42	//!< I/O port for timer channel 2.
#define TIMER_MODE	0x43	//!< I/O port for timer mode control.
#define SQUARE_WAVE	0x36	//!< The sqare-wave form.
#define TIMER_FREQ	1193182L //!< Clock frequency for timer in PC.

//! Value to initialize timer.
#define TIMER_COUNT	(unsigned)(TIMER_FREQ/HZ)

//! LATCH is used for the interval timer.
#define LATCH		((TIMER_FREQ + HZ/2) / HZ)

//! Clock Frequancy (User settable, default=100Hz)
#define HZ		100

//! Timeout variable structure.
typedef struct timeout
{
	dword start_ticks;
	dword ticks_to_wait;
} timeout_t;

// --- Prototypes ----------------------------------------------------- //

void init_clock();
void stop_clock();
unsigned long sys_get_ticks(void);
clock_t sys_times( struct tms *buf );
void delay(dword millisec);

void set_timeout(timeout_t *t, unsigned int millisec);
bool is_timeout(timeout_t *t);

void clock_handler( irq_context_t *c );

/** @} */ // end of KClock

#endif
