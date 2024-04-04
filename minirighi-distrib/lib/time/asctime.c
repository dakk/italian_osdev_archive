#include <stdio.h>
#include <time.h>

char *asctime_r( const struct tm *timeptr, char *buf )
{
	static const char wday_name[][3] =
	{
		"sun", "mon", "tue", "wed", "thu", "fri", "sat"
	};
	static const char mon_name[][3] =
	{
		"jan", "feb", "mar", "apr", "may", "jun",
		"jul", "aug", "sep", "oct", "nov", "dec"
	};
	register const char *wn;
	register const char *mn;
	int size;

	if (timeptr->tm_wday < 0 || timeptr->tm_wday >= 7)
		wn = "???";
	else	wn = wday_name[timeptr->tm_wday];
	if (timeptr->tm_mon < 0 || timeptr->tm_mon >= 12)
		mn = "???";
	else	mn = mon_name[timeptr->tm_mon];

	size = snprintf( buf, 26, "%.3s %.3s%3d %02d:%02d:%02d %d\n",
		wn, mn,
		timeptr->tm_mday, timeptr->tm_hour,
		timeptr->tm_min, timeptr->tm_sec,
		timeptr->tm_year );
	if( size >= 26 )
		return( NULL );
	return( buf );
}

char *asctime( const struct tm *timerptr )
{
	static char result[ 26 ];
	char *res = asctime_r( timerptr, result );
	return( res );
}
