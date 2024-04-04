/*!	\file include/sys/times.h
 *	\brief System timers.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-20
 */

#ifndef TIMES_H
#define TIMES_H

#include <kernel/posix_types.h>

//! Timer structure.
struct tms
{
	clock_t tms_utime;	//!< user time.
	clock_t tms_stime;	//!< system time.
	clock_t tms_cutime;	//!< user time of children.
	clock_t tms_cstime;	//!< system time of children.
};

#endif
