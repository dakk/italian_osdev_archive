
#ifndef	_SYSCALL_FILESYSTEM_
#define	_SYSCALL_FILESYSTEM_

#include	"config.h"

#include	"filesystem.h"
#include	"kstring.h"

#include	"syscall_task.h"	//Per Le Prove

#define	VOLUME_NAME_LUNG	4

int	syscall_open(char* nomefile);
bool	syscall_close(int fd);
int	syscall_seek(int fd,int modo,int spostamento);
int	syscall_read(int fd,int lung,char* buffer);
int	syscall_write(int fd,int lung,char*buffer);
bool	syscall_remv(char* nomefile);
bool	syscall_touch(char* nomefile);

#endif
