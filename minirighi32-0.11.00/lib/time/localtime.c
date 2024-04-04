#include <stdio.h>
#include <time.h>

struct tm *localtime_r( const time_t *clock, struct tm *tm )
{
	static const uint8_t monthcnt[] = {
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	int ss, mm, hh, wday, yday, month, year;
	time_t tt = *clock;
	int i;

	ss = tt % 60;
	tt /= 60;	/* minutes */
	mm = tt % 60;
	tt /= 60;	/* hours */
	hh = tt % 24;
	tt /= 24;	/* days */
	wday = (4 + tt) % 7;	/* weekday, 'twas thursday when time started */

	for (year = 1970; tt >= 365; year++)
		tt -= isleap(year)? 366: 365;

	tt++;	/* days are 1-based */

	for (month = 0; tt > monthcnt[month]; month++)
		tt -= monthcnt[month];

	if (month > 2 && isleap(year))
		tt--;

	for( i = 0, yday = 0; i < (month - 1); i++ )
	{
		yday += monthcnt[ i ];
	}
	yday += tt;

	// Fill the time structure.
	tm->tm_sec = ss;
	tm->tm_min = mm;
	tm->tm_hour = hh;
	tm->tm_mday = tt;
	tm->tm_mon = month;
	tm->tm_year = year;
	tm->tm_wday = wday;
	tm->tm_yday = yday;
	tm->tm_isdst = 0;

	return( tm );
}

struct tm *localtime( const time_t *clock )
{
	static struct tm tm;
	return( localtime_r(clock, &tm) );
}
