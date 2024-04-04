/*!	\file lib/unistd/sleep.c
 *	\brief LIBC sleep and usleep routines.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-12-12
 */

#include <sys/sys.h>
#include <time.h>
#include <unistd.h>

//! Sleep for the specified number of seconds.
void usleep(unsigned long usec)
{
	struct timespec t;

	t.tv_sec = usec / 1000000;
	t.tv_nsec = (usec % 1000000) * 1000;
	(void)nanosleep(&t, &t);
}

//! Sleep for the specified number of seconds.
unsigned int sleep(unsigned int seconds)
{
	struct timespec t;
	int __ret;

	t.tv_sec = seconds;
	t.tv_nsec = 0;
	__ret = nanosleep(&t, &t);

	return __ret;
}
