/*!	\file include/const.h
 *	\brief Global constants definition.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2003-12-16 Andrea Righi:
 *			Added pid_t typedef.\n
 *		2004-13-01 Andrea Righi:
 *			New kernel style.\n
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef CONST_H
#define CONST_H

#include <types.h>
#include <kernel/compiler.h>
#include <kernel/config.h>

// --- Global constants ----------------------------------------------- //

//! Logical value of TRUE.
#define TRUE	1
//! Logical value of FALSE.
#define FALSE	0
//! NULL constant.
#define NULL	0

//! Little endian architecture.
#define __LITTLE_ENDIAN__	1234
//! Big endian architecture.
#define __BIG_ENDIAN__		4321
//! The current architecture.
#define __BYTE_ORDER__		__LITTLE_ENDIAN__

// --- Useful macros -------------------------------------------------- //

//! This macro returns the minimum value between \a a and \a b.
#define MIN(a, b)	((a) < (b) ? (a) : (b))
//! This macro returns the maximum value between \a a and \a b.
#define MAX(a, b)	((a) > (b) ? (a) : (b))
//! This macro returns the absolute value of \a a.
#define ABS(a)		((a) < 0 ? -(a) : (a))
//! This macro truncates \a addr to the \a align boundary.
#define TRUNC(addr, align)	((addr) & ~((align) - 1))
//! This macro rounds down \a addr to the \a align boundary.
#define ALIGN_DOWN(addr, align)	TRUNC(addr, align)
//! This macro rounds up \a addr to the \a align boundary.
#define ALIGN_UP(addr, align)	( ((addr) + (align) - 1) & (~((align) - 1)) )

#endif
