/*!	\file drivers/time/time.c
 *	\brief Real time clock driver.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-26\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <const.h>
#include <errno.h>
#include <time.h>
#include <arch/interrupt.h>
#include <kernel/console.h>
#include <kernel/semaphore.h>
#include <kernel/time.h>

//! The real-time clock semaphore.
DECLARE_MUTEX( rtc_lock );

//! Set the CMOS real-time clock.
int time_read( rtc_time_t *rtc )
{
	int i;

	DOWN( &rtc_lock );

	// When the Update-In-Progress (UIP) flag goes from 1 to 0,
	// the RTC registers show the second which has precisely just
	// started.

	// Read RTC exactly on falling edge of update flag.
	for( i = 0; i < 1000000 ; i++ )	// may take up to 1 second...
		if ( CMOS_READ(RTC_FREQ_SELECT) & RTC_UIP )
			break;

	// Must try at least 2.228 ms
	for( i = 0 ; i < 1000000 ; i++ )
		if ( !(CMOS_READ(RTC_FREQ_SELECT) & RTC_UIP))
			break;

	// Read data from the CMOS.
	rtc->sec = BCD2BIN( CMOS_READ(RTC_SECONDS) );
	rtc->min = BCD2BIN( CMOS_READ(RTC_MINUTES) );
	rtc->hour = BCD2BIN( CMOS_READ(RTC_HOURS) );
	rtc->dayweek = BCD2BIN( CMOS_READ(RTC_DAY_OF_WEEK) );
	rtc->daymon = BCD2BIN( CMOS_READ(RTC_DAY_OF_MONTH) );
	rtc->mon = BCD2BIN( CMOS_READ(RTC_MONTH) );
	rtc->year = BCD2BIN( CMOS_READ(RTC_YEAR) );

	UP( &rtc_lock );

	if( (rtc->year += 1900) < 1970 )
	{
		rtc->year += 100;
	}

	return( 0 );
}

//! Set the CMOS real-time clock.
int time_write( rtc_time_t *rtc )
{
	return( -EPERM );
}

//! The real-time clock interrupt routine.
void time_handler( irq_context_t *c )
{
	// Unused.
}

//! Initialize the real-time clock to generate periodic interrupt.
// \warning Unused for now...
int init_rtc( void )
{
	uint8_t status_b;
	uint32_t flags;

	local_irq_save( flags );

	// Install the irq handler.
	install_trap_handler( RTC_IRQ, &time_handler );

	// Read status registers.
	status_b = CMOS_READ( RTC_REG_B );
	// Enable periodic interrupt.
	status_b |= RTC_PIE;

	// Write status registers.
	CMOS_WRITE( RTC_REG_B, status_b );

	local_irq_restore( flags );

	return( 0 );
}

//! \brief
//!	Get the seconds passed since midnight 1970-01-01
//!	(Unix timestamp).
//! \return
//!	The UNIX timestamp.
time_t sys_time( time_t *t )
{
	rtc_time_t rtc;
	struct tm tm;
	time_t ret = (time_t)(-1);

	if( time_read( &rtc ) == 0 )
	{
		// Copy real time clock values into the standard
		// structure.
		tm.tm_year = rtc.year;
		tm.tm_mon = rtc.mon;
		tm.tm_mday = rtc.daymon;
		tm.tm_hour = rtc.hour;
		tm.tm_min = rtc.min;
		tm.tm_sec = rtc.sec;

		// Make the UNIX timestamp.
		ret = mktime( &tm );
	}
	if( t != NULL )
	{
		// Update the argument.
		*t = ret;
	}

	return( ret );
}
