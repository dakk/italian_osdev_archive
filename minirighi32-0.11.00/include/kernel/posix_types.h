/*!	\file include/kernel/posix_types.h
 *	\brief POSIX types.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-04
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef POSIX_TYPES_H
#define POSIX_TYPES_H

typedef unsigned short	dev_t;
typedef unsigned long	ino_t;
typedef unsigned short	mode_t;
typedef unsigned short	nlink_t;
typedef long		off_t;
typedef int		pid_t;
typedef unsigned short	ipc_pid_t;
typedef unsigned short	uid_t;
typedef unsigned short	gid_t;
typedef unsigned int	size_t;
typedef int		ssize_t;
typedef int		ptrdiff_t;
typedef long		time_t;
typedef long		suseconds_t;
typedef long		clock_t;
typedef int		daddr_t;
typedef char *		caddr_t;
typedef unsigned short	uid16_t;
typedef unsigned short	gid16_t;
typedef unsigned int	uid32_t;
typedef unsigned int	gid32_t;
typedef long long	loff_t;

#endif
