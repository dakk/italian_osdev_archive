#ifndef _FS_CONSOLE_
#define _FS_CONSOLE_

#include "config.h"
#include "filesystem.h"

#include "console.h"

#define	CONSOLEFS_MAX_IMAGE_LUNG 4000

struct	console_session_t
{
	int cursore;
	unsigned char bgColor, fgColor;
	char buffer[CONSOLEFS_MAX_IMAGE_LUNG];
	bool blink;
};

//Console_init() e Console_shutdown() si trovano in console.c

bool Console_init();
bool Console_shutdown();
bool Console_open(void** session,char* nomefile);
bool Console_close(void** session);
int  Console_seek(void** session,int modo,int puntatore);
int  Console_read(void** session,int caratteri,char* buffer);
int  Console_write (void** session,int caratteri,char* buffer);
bool Console_touch(char* nomefile);
bool Console_remv(char* nomefile);




#endif
