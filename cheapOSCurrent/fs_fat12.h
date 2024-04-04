
#ifndef	_FS_FAT12_
#define	_FS_FAT12_

#include	"config.h"

#include	"filesystem.h"
#include	"floppy.h"
#include	"kconsole.h"
#include	"kstring.h"

/*Provvisorio*/
#include	"task.h"
#include	"scheduler.h"
#include	"pidcoda.h"
#include	"mem.h"
#include	"syscall_task.h"
#include	"cache.h"
/*Provvisorio*/

#define	FAT12_IMG_SIZE		(128*1024)		//128KB immagine
#define	FATFSSESSION		((struct fat12_session_t*)*session)
#define	FATIMGDIR			((struct fat12_FileEntry_t*)FATFSSESSION->image)

struct	fat12_BootSector_t
	{
	unsigned char		BS_jmpBoot[3];
	unsigned char		BS_OEMName[8];

	//BPB---------------------------------------------------
	unsigned short		BPB_BytsPerSec;			//Byets per Settore			Default 512
	unsigned char		BPB_SecPerClus;			//Settori per Blocco			Default 1
	unsigned short		BPB_RsvdSecCnt;			//Settori riservati			Default 1
	unsigned char		BPB_NumFATs;			//Numero di FAT(su disco)	Default 2
	unsigned short		BPB_RootEntCnt;			//Lunghezza della Dir Root (In Entries)
	unsigned short		BPB_Totsec16;
	unsigned char		BPB_Media;
	unsigned short		BPB_FATSz16;
	unsigned short		BPB_SecPerTrk;
	unsigned short		BPB_NumHeads;
	unsigned int		BPB_HiddSec;
	unsigned int		BPB_TotSec32;

	//Solo per FAT12 e FAT 16-----------------------
	unsigned char		BS_DrvNum;
	unsigned char		BS_Reserved1;
	unsigned char		BS_BootSig;
	unsigned int		BS_VolID;
	unsigned char		BS_VolLab[11];
	unsigned char		BS_FilSysType[8];
	} __attribute__ ((packed));


//Entry che identifica un file/directory nella directory superiore
struct	fat12_FileEntry_t
	{
	unsigned char	Name[8];
	unsigned char	Extension[3];
	unsigned char	Attributes;
	unsigned char	Reserved[10];
	unsigned short	Time;
	unsigned short	Data;
	unsigned short	StartCluster;
	unsigned int	FileLenght;
	} __attribute__ ((packed));


//sessione cioe' un file aperto da un Task
struct fat12_session_t
	{
//	bool modificato;
	bool	directory;

//	int	upperdir_startcluster;							//Cluster iniziale della dir contenitore

	unsigned int size;									//Caratteristiche dell'immagine
	unsigned int cursore;
	char image[FAT12_IMG_SIZE];
	};

//Primitive di accesso alla FAT (immagazzinata su disco e tradotta da fisica a logica)
inline unsigned short	fat12_nextcluster	(unsigned short cluster);
inline bool	 		fat12_isAvail		(unsigned short cluster);
inline bool	 		fat12_isReserv		(unsigned short cluster);
inline bool	 		fat12_isBad		(unsigned short cluster);
inline bool	 		fat12_isUserData	(unsigned short cluster);
inline bool	 		fat12_isEndMark	(unsigned short cluster);

bool		fat12_mount();											//Monta il floppy (almeno ci prova)
bool		fat12_umount();										//Smonta

int		fat12_findindir(mem_t dir_img,int dir_size,char* nomefile);		//Cerca la Entry di 'nomefile' nell'immagine della dir 'dir_img' della lunghezza 'dir_size'

int		fat12_loadfiledir(mem_t buffer,unsigned short startcluster);		//Carica un file in 'buffer' a partire dal Blocco startcluster (accede alla FAT)


bool		fat12_init();											//Inizzializza il filesystem della fat12
//bool		fat12_shutdown();
bool		fat12_open(void** session,char* nomefile);
bool		fat12_close(void** session);
int		fat12_seek(void** session,int modo,int puntatore);
int		fat12_read(void** session,int caratteri,char* buffer);
int		fat12_write (void** session,int caratteri,char* buffer);
bool		fat12_touch(char* nomefile);
bool		fat12_remv(char* nomefile);

/*Provvisorio*/
void fat12_mountproc();
void fat12_umountproc();

void fat12_gomount();
void fat12_goumount();
/*Provvisori*/


#endif
