/*!	\file include/sys/sys.h
 *	\brief Generic minirighi syscall library.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-03-26 Andrea Righi:
 *			Arguments of the system calls are passed
 *			using registers and not the stack.
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#ifndef SYS_H
#define SYS_H

#include <const.h>
#include <time.h>
#include <types.h>
#include <sys/times.h>
#include <net/socket.h>

extern int errno;

//! Go back to the system.
#define SYS_exit		0
//! Get a char from stdin.
#define SYS_getchar		1
//! Put a character on stdout.
#define SYS_putchar		2
//! Allocate a memory area.
#define SYS_malloc		3
//! Free a memory area.
#define SYS_free		4
//! Administer the system log book.
#define SYS_syslog		5
//! Get the system ticks.
#define SYS_times		6
//! Create a new thread.
#define SYS_thread_spawn	7
//! Returns the process ID of the current process.
#define SYS_getpid		8
//! Returns the process ID of the parent of the current process.
#define SYS_getppid		9
//! Get the UNIX timestamp.
#define SYS_time		10
//! Create a socket.
#define SYS_socket		11
//! Close a socket.
#define SYS_closesocket		12
//! Bind a name to a socket.
#define SYS_bind		13
//! Receive a datagram packet.
#define SYS_recvfrom		14
//! Send a datagram packet.
#define SYS_sendto		15
//! Fork a new task.
#define SYS_fork		16
//! Sleep some micro-seconds.
#define SYS_nanosleep		17

//! Return from a system call.
#define __syscall_return(type, res)	\
	do 				\
	{ 				\
		if( (unsigned long)(res) >= (unsigned long)(-125) ) \
		{			\
			errno = -(res);	\
		}			\
		return( (type)(res) );	\
	} while( 0 )

//! The core of a system call with no argument.
#define _syscall0(type, name)		\
type name(void)				\
{					\
	long __res;			\
	__asm__ __volatile__ (		\
		"int $0x80"		\
		: "=a"(__res)		\
		: "0"(SYS_##name) );	\
	__syscall_return(type, __res);	\
}

//! The core of a system call with 1 argument.
#define _syscall1(type, name, type1, arg1) \
type name( type1 arg1 )			\
{					\
	long __res;			\
	__asm__ __volatile__ (		\
		"int $0x80"		\
		: "=a"(__res)		\
		: "0"(SYS_##name), "b"((long)(arg1)) );	\
	__syscall_return(type, __res);	\
}

//! The core of a system call with 2 arguments.
#define _syscall2(type, name, type1, arg1, type2, arg2) \
type name( type1 arg1, type2 arg2 )	\
{					\
	long __res;			\
	__asm__ __volatile__ (		\
		"int $0x80"		\
		: "=a"(__res)		\
		: "0"(SYS_##name), 	\
		"b"((long)(arg1)), 	\
		"c"((long)(arg2)) );	\
	__syscall_return(type, __res);	\
}

//! The core of a system call with 3 arguments.
#define _syscall3(type, name, type1, arg1, type2, arg2, type3, arg3) \
type name( type1 arg1, type2 arg2, type3 arg3 )	\
{					\
	long __res;			\
	__asm__ __volatile__ (		\
		"int $0x80"		\
		: "=a"(__res)		\
		: "0"(SYS_##name),	\
		"b"((long)(arg1)),	\
		"c"((long)(arg2)),	\
		"d"((long)(arg3)) );	\
	__syscall_return(type, __res);	\
}

//! The core of a system call with 4 arguments.
#define _syscall4(type, name, type1, arg1, type2, arg2, type3, arg3, type4, arg4) \
type name( type1 arg1, type2 arg2, type3 arg3, type4 arg4 )	\
{					\
	long __res;			\
	__asm__ __volatile__ (		\
		"int $0x80"		\
		: "=a"(__res)		\
		: "0"(SYS_##name),	\
		"b"((long)(arg1)),	\
		"c"((long)(arg2)),	\
		"d"((long)(arg3)),	\
		"S"((long)(arg4)) );	\
	__syscall_return(type, __res);	\
}

//! The core of a system call with 5 arguments.
#define _syscall5(type, name, type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5) \
type name( type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5 )	\
{					\
	long __res;			\
	__asm__ __volatile__ (		\
		"int $0x80"		\
		: "=a"(__res)		\
		: "0"(SYS_##name),	\
		"b"((long)(arg1)),	\
		"c"((long)(arg2)),	\
		"d"((long)(arg3)),	\
		"S"((long)(arg4)),	\
		"D"((long)(arg5)) );	\
	__syscall_return(type, __res);	\
}

//! The core of a system call with 6 arguments.
#define _syscall6(type, name, type1, arg1, type2, arg2, type3, arg3, type4, arg4, type5, arg5, type6, arg6) \
type name( type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5, type6 arg6 )	\
{					\
	long __res;			\
	__asm__ __volatile__ (		\
		"push %%ebp\n"		\
		"movl %%eax, %%ebp\n"	\
		"movl %1, %%eax\n"	\
		"int $0x80\n"		\
		"pop %%ebp"		\
		: "=a"(__res)		\
		: "i"(SYS_##name),	\
		"b"((long)(arg1)),	\
		"c"((long)(arg2)),	\
		"d"((long)(arg3)),	\
		"S"((long)(arg4)),	\
		"D"((long)(arg5)),	\
		"0"((long)(arg6)) );	\
	__syscall_return(type, __res);	\
}

// --- Prototypes ----------------------------------------------------- //

static __INLINE__ _syscall1(int, exit, int, exitcode);
static __INLINE__ _syscall0(int, getchar);
static __INLINE__ _syscall1(int, putchar, int, c);
static __INLINE__ _syscall1(void *, malloc, size_t, size);
static __INLINE__ _syscall1(void, free, void *, ptr);
static __INLINE__ _syscall3(long, syslog, int, type, char *, buf, int, len);
static __INLINE__ _syscall1(clock_t, times, struct tms *, buf);
static __INLINE__ _syscall4(pid_t, thread_spawn, void *, routine, int, argc, char **, argv, char *, pname);
static __INLINE__ _syscall0(pid_t, getpid);
static __INLINE__ _syscall0(pid_t, getppid);
static __INLINE__ _syscall1(time_t, time, time_t *, t);

static __INLINE__ _syscall3(int, socket, int, domain, int, type, int, protocol);
static __INLINE__ _syscall1(int, closesocket, int, sockfd);
static __INLINE__ _syscall3(int, bind, int, sockfd, struct sockaddr *, my_addr, socklen_t, addrlen);
static __INLINE__ _syscall6(int, recvfrom,
				int, s, void *, buf, size_t, len,
				int, flags, struct sockaddr *, from, socklen_t *, fromlen );
static __INLINE__ _syscall6(int, sendto,
				int, s, const void *, msg, size_t, len,
				int, flags, const struct sockaddr *, to, socklen_t, tolen );
static __INLINE__ _syscall0(pid_t, fork);
static __INLINE__ _syscall2(int, nanosleep, const struct timespec *, req, struct timespec *, rem);

#endif
