/*!	\file kernel/syscall.c
 *	\brief Some standard system call implementation.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-01\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <time.h>
#include <types.h>
#include <arch/clock.h>
#include <arch/delay.h>
#include <arch/interrupt.h>
#include <kernel/task.h>

//! \brief Returns the process ID of the current process.
pid_t sys_getpid()
{
	return curr_task->pid;
}

//! \brief Returns the process ID of the parent of the current process.
pid_t sys_getppid()
{
	if (curr_task->father != NULL)
		return curr_task->father->pid;
	else
		return 0;
}

//! \brief Create a new thread for the current process.
//! \param routine The entry point of the new thread.
//! \param argc The number of parameters.
//! \param argv A pointer to the string parameters
//! \param pname The name of the thread.
//! \return The pid of the new thread.
pid_t sys_thread_spawn( void *routine, int argc, char **argv, char *name )
{
	pid_t __ret;
	__ret = create_thread(routine, argc, argv, name)->pid;
	return __ret;
}

//! \brief Create a child process.
//! \param regs The father context.
//! \return The pid of the new task.
pid_t sys_fork(syscall_context_t *regs)
{
	pid_t __ret;
	__ret = do_fork(regs);
	return __ret;
}

//! \brief Sleep some microseconds.
//! \param usec The microseconds the calling process must sleep.
int sys_nanosleep(const struct timespec *req, struct timespec *rem)
{
	if ((req->tv_nsec >= 1000000000L) || (req->tv_nsec < 0) || (req->tv_sec < 0))
		return -EINVAL;

	if (req->tv_sec)
		delay(req->tv_sec * 1000);
	if (req->tv_nsec)
		ndelay(req->tv_nsec);

	return 0;
}
