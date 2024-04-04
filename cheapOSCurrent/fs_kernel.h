
#ifndef	_FS_KERNEL_
#define	_FS_KERNEL_

#include		"config.h"

#include		"filesystem.h"
#include		"kmath.h"
#include		"time.h"
#include		"keyboard.h"
#include		"keycoda.h"
#include		"scheduler.h"
#include		"pidcoda.h"
#include		"floppy.h"
#include		"cache.h"
#include		"fs_fat12.h"

#define	KERNELFS_MAX_IMAGE_LUNG	(5*1024)
#define	KFSSESSION					((struct kernelfs_session_t*)*session)
#define	KFFILENUM					11

//Sessione di un file di kernelfs
struct	kernelfs_session_t
	{
	int cursore;
	int size;
	char	image[KERNELFS_MAX_IMAGE_LUNG];
	};

bool		kernel_init();
//bool	kernel_shutdown();

bool		kernel_open(void** session,char* nomefile);
bool		kernel_close(void** session);
int		kernel_seek(void** session,int modo,int puntatore);
int		kernel_read(void** session,int caratteri,char* buffer);
int		kernel_write (void** session,int caratteri,char* buffer);
bool		kernel_touch(char* nomefile);
bool		kernel_remv(char* nomefile);

#endif
