
#ifndef	_FILESYSTEM_
#define	_FILESYSTEM_

#include	"config.h"

#include	"kconsole.h"
#include "kstring.h"

//Include dei file dei filesystems--------------------------INIZIO
#include	"fs_kernel.h"
#include	"fs_fat12.h"

//#include	"fs_con.h"
//Include dei file dei filesystems--------------------------FINE

#define	FILESYSTEM_NUM				3
#define	TABELLA_ACCESSOFILE_LUNG	24

#define	MREL	0
#define	MINIZIO	1
#define	MFINE	2

typedef	bool	(*fsi_init_t)();
//typedef	bool	(*f_shutdown)();

typedef	bool	(*fsi_open_t)(void** session,char* nomefile);				//Apre il file nomefile e crea la sessione **session
typedef	bool	(*fsi_close_t)(void** session);							//Chiude il file relativo alla sessiopne **session
typedef	int	(*fsi_seek_t) (void** session,int modo,int puntatore);		//Seeka
typedef	int	(*fsi_read_t) (void** session,int caratteri,char* buffer);		//Legge 'caratteri' caratteri dal file al buffer
typedef	int	(*fsi_write_t) (void** session,int caratteri,char* buffer);		//Scrive 'caratteri' caratteri dal buffer al file
typedef	bool	(*fsi_touch_t)(char* nomefile);							//Crea nomefile
typedef	bool	(*fsi_remv_t) (char* nomefile);						//Cancella nomefile


struct fsi_t
	{
	char	nome[8];

	fsi_init_t		init;
	//fsi_shutdown	shutdown;

	fsi_open_t	open;
	fsi_close_t	close;
	fsi_seek_t	seek;
	fsi_read_t	read;
	fsi_write_t	write;
	fsi_touch_t	touch;
	fsi_remv_t	remv;
	};

//Elemento che tiene conto del file aperto dal task (ovvero una sessione)
struct filesystem_accesso_t
	{
	char nomefile[KSTRNG_LUNG];	//nome del file aperto
	int pid;						//Task che accede al file
	int fs;						//FileSystem Usato
	void* session;					//sessione
	};


bool filesystem_init();				//Inizzializza i filesystems
int	pidcloseall(int pid);				//Chiude tutti i File Aperti dal Task con pid 'pid'
void	filesystem_tabella_dumpscreen();	//Stampa a Video la Tebella dei Filesystem
int	filesystem_tabella_dump(char*buffer);

#endif
