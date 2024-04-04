
#ifndef	_SYSCALL_SYSTEM_
#define	_SYSCALL_SYSTEM_

#include	"config.h"

#include	"io.h"
#include	"task.h"
#include	"scheduler.h"
#include	"syscall_task.h"

#define	TICKSLASTTOSHUTDOWN		1500		//15 Sec

bool syscall_shutdown();
void	goshutdown();

void shutdownproc();	//Processo diShutdown

#endif

