
#ifndef	_CHEAPOSLIB_
#define	_CHEAPOSLIB_

#include		"../config.h"

#define		NUM_LUNG		12
#define		STRNG_LUNG		96

#define		TAB_SIZE		3

#define		CHOME		30
#define		CCLEAR		24
#define		CSX			174
#define		CDX			175
#define		CUP			176
#define		CDOWN		177

#define		CBLACK			178
#define		CDKBLUE		179
#define		CDKGREEN		180
#define		CDKCYAN		181
#define		CDKRED			182
#define		CDKPURPLE		183
#define		CDKBROWN		184
#define		CLTGRAY		185
#define		CDKGRAY		186
#define		CLTBLUE			187
#define		CLTGREEN		188
#define		CLTCYAN		189
#define		CLTRED			190
#define		CLTPURPLE		191
#define		CLTBROWN		192
#define		CWHITE			193
#define		CREVERSE		194
#define		CBLINK			195

#define	MREL	0
#define	MINIZIO	1
#define	MFINE	2

#define	RANSCORTA	32

/*BASE---------------------------------------------*/
void _start();
void _exit();

/*SYSTEM-----------------------------------------*/
bool shutdown();

/*TASK---------------------------------------------*/
int fork();
int getpid();
unsigned long wait(unsigned long ticks);
int waitpid(int pid);
int exec(char* nomefile,int argc,char* argv0);
bool kill(int pid);
void exit(int exitcode);

/*FILESYSTEM------------------------------------*/
int open(char* nomefile);
bool close(int fd);
unsigned int seek(int fd,int modo,int posizione);
int read(int fd,int lunghezza,char* buffer);
int write(int fd,int lunghezza,char* buffer);
bool touch(char*nomefile);
bool remv(char* nomefile);

/*UTILI-----------------------------------------------*/
bool openconsole(int console);
bool closeconsole();

int	print(const char* stringa,...);
int	baseprint(char* stringa);
unsigned char	getchar();
int	getstring(char* buffer,int maxcars);

int	random();
int	randomInt(int min,int max);

unsigned int strlen(const char* stringa);
char* strcpy(char *dest, const char *src);
bool strcmp(const char* s1,const char* s2);
bool strncmp(const char *s1,const char *s2,int  n);
unsigned int sprint(char* buffer,const char* stringa,...);
int atoi(char* buffer);

#endif
