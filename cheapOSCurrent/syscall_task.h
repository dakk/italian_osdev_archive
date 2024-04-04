
#ifndef	_SYSCALL_TASK_
#define	_SYSCALL_TASK_

#include		"config.h"

#include		"task.h"
#include		"pidcoda.h"
#include		"scheduler.h"
#include		"syscall_filesystem.h"
#include		"filesystem.h"

int	syscall_exit();
int	syscall_fork();
int	syscall_getpid();
unsigned long	syscall_wait();
int	syscall_waitpid(int pid);
int	syscall_exec(char* nomefile,int argc,char* argv0);
bool	syscall_kill(int pid);

#endif
