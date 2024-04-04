
#ifndef	_KERNEL_
#define	_KERNEL_

#include	"config.h"

#include	"descrittori.h"
#include	"io.h"
#include	"kconsole.h"
#include	"mem.h"
#include	"cpu.h"
#include	"irq.h"
#include	"isr.h"
#include	"time.h"
#include	"task.h"
#include	"syscall.h"
#include	"scheduler.h"
#include	"pidcoda.h"
#include	"kmath.h"
#include	"filesystem.h"
#include	"keyboard.h"
#include	"graphics.h"
#include	"floppy.h"
#include	"fs_fat12.h"
#include	"cache.h"
#include	"syscall_system.h"

void	_start();		//Abbocco per il loader
int	main();		//Inizio del Kernel

/*Provvisorio*/
//void orologio();
//void cache_view();
/*Provvisorio*/

#endif
