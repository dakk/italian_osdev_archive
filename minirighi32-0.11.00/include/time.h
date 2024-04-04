/*!	\file include/time.h
 *	\brief Time management.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-25\n
 */

#ifndef TIME_H
#define TIME_H

#include <const.h>

//! Evaluate if the year is leap.
#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

//! Time structure.
struct tm {
	int tm_sec;	//!< Seconds.
	int tm_min;	//!< Minutes.
	int tm_hour;	//!< Hours.
	int tm_mday;	//!< Day of the month.
	int tm_mon;	//!< Month.
	int tm_year;	//!< Year.
	int tm_wday;	//!< Day of the week.
	int tm_yday;	//!< Day in the year.
	int tm_isdst;	//!< Daylight saving time.
};

//! Structure to specify intervals of time with nanosecond pecision.
struct timespec {
	time_t	tv_sec;		//!< seconds
	long	tv_nsec; 	//!< nanoseconds
};

//! \brief Converts Gregorian date to seconds since 1970-01-01 00:00:00.
//! \param year The year value.
//! \param mon The month value.
//! \param day The day of the month value.
//! \param hour The hours value.
//! \param min The minutes value.
//! \param sec The seconds value.
//! \return
//!	Seconds since 1970-01-01 00:00:00 (UNIX timestamp).
/**
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
static __INLINE__ time_t mktime( struct tm *tm )
{
	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int) (tm->tm_mon -= 2)) {
		/* Puts Feb last since it has leap day */
		tm->tm_mon += 12;
		tm->tm_year -= 1;
	}

	return (((
		(unsigned long) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 + 367*tm->tm_mon/12 + tm->tm_mday) +
			tm->tm_year*365 - 719499
	    )*24 + tm->tm_hour /* now have hours */
	  )*60 + tm->tm_min /* now have minutes */
	)*60 + tm->tm_sec; /* finally seconds */
}

// --- Prototypes ----------------------------------------------------- //

extern char *asctime(const struct tm *timerptr);
extern char *asctime_r(const struct tm *timeptr, char *buf);
extern char *ctime(const time_t *clock);
extern char *ctime_r(const time_t *clock, char *buf);
struct tm *localtime(const time_t *clock);
struct tm *localtime_r(const time_t *clock, struct tm *tm);

#endif
