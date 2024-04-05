
#ifndef	_SYSCALL_
#define	_SYSCALL_


#include	"config.h"

#include	"isr.h"
#include	"kconsole.h"
#include	"task.h"
#include	"time.h"
#include	"cpu.h"

#include	"syscall_task.h"
#include	"syscall_filesystem.h"
#include	"syscall_system.h"

#define	SYSCALLINT	0x41


bool syscall_init();


#endif
