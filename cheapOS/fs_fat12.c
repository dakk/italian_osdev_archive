

#include	"fs_fat12.h"

struct fat12_BootSector_t*	fat12_BootSector_img;	//Immagine BootSector

bool					fat12_montato;

unsigned short *		fat12_FAT_img;			//Immagine della FAT (convertita)
unsigned int			fat12_FAT_size;			//Lunghezza della FAT (in unsigned short)
//bool					fat12_FAT_modificata;	//La fat e' stata modificata??

unsigned short			fat12_ROOTDIR_startcluster;	//Primo Cluster ROOTDIR
unsigned short			fat12_DATI_startcluster;		//Primo Cluster DATI

/*Provvisorio*/
extern struct task_t task_lista[MAX_TASK_NUM];		//Lista dei task!!!
extern struct pidcoda_t Qnotask,Qready;
extern struct cache_t floppy_cache;
/*Provvisorio*/

/*
FAT 12 valori
Avail 000	Reserv 001	UserData 002-FF6		Bad FF7		EndMark FF8-FFF
*/

inline unsigned short fat12_nextcluster(unsigned short cluster)
	{
	return fat12_FAT_img[cluster];
	}

inline bool	 fat12_isAvail(unsigned short cluster)
	{
	return (cluster==0x000);
	}

inline bool	 fat12_isReserv(unsigned short cluster)
	{
	return (cluster==0x001);
	}

inline bool	 fat12_isUserData(unsigned short cluster)
	{
	return (cluster>0x001 && cluster<0xFF7);
	}

inline bool	 fat12_isBad(unsigned short cluster)
	{
	return (cluster==0xFF7);
	}

inline bool	fat12_isEndMark(unsigned short cluster)
	{
	return (cluster>0xFF7 && cluster<0x1000);
	}


bool	fat12_mount()
	{
	int i,j;
	int lung_logical,lung_physical;
	int RootDirSectors,DataSec,Clusters;

	if(fat12_montato) return FALSE;

	cache_clear(&floppy_cache);		//clear cache

	fat12_BootSector_img=(struct fat12_BootSector_t*)mem_alloc(512*sizeof(unsigned char));

	if(fat12_BootSector_img==0) return FALSE;

	while(!floppy_read(0,(mem_t)fat12_BootSector_img,1))
		kprint("Floppy Riprovo\n");


	/*Controlo se e' proprio una FAT12*/

	if(	(fat12_BootSector_img->BPB_SecPerClus	!=1)		||
		(fat12_BootSector_img->BPB_BytsPerSec	!=512)	||
		(fat12_BootSector_img->BPB_Media		!=0xF0)	||
		(fat12_BootSector_img->BPB_NumFATs		!=2)		||
		(fat12_BootSector_img->BPB_Totsec16		!=2880)	)
		{
		mem_free((mem_t)fat12_BootSector_img);
		kprint("Disk non Fat12\n");
		return FALSE;
		}

	if(fat12_BootSector_img->BPB_FATSz16==0)
		{
		mem_free((mem_t)fat12_BootSector_img);
		kprint("Disk non Fat12\n");
		return FALSE;
		}

	RootDirSectors=((fat12_BootSector_img->BPB_RootEntCnt*32)+(fat12_BootSector_img->BPB_BytsPerSec-1))/fat12_BootSector_img->BPB_BytsPerSec;
	DataSec=fat12_BootSector_img->BPB_Totsec16-(fat12_BootSector_img->BPB_RsvdSecCnt+(fat12_BootSector_img->BPB_FATSz16*fat12_BootSector_img->BPB_NumFATs)+RootDirSectors);
	Clusters=DataSec/fat12_BootSector_img->BPB_SecPerClus;

	if(Clusters>=4085)
		{
		mem_free((mem_t)fat12_BootSector_img);
		kprint("Disk non Fat12\n");
		return FALSE;
		}

	/*Carico FAT in memoria*/
	lung_physical	   =fat12_BootSector_img->BPB_FATSz16*512;
	lung_physical  +=(3-(lung_physical%3));							//Paddo a 3;
	lung_logical	   =(lung_physical/3)*4;							//Spazio per la conversione!

	fat12_FAT_img=(unsigned short*)mem_alloc(lung_logical);			//C'e' spazio per quando la traduco!

	if(fat12_FAT_img==0)
		{
		mem_free((mem_t)fat12_BootSector_img);
		return FALSE;
		}

	while(	!floppy_read(fat12_BootSector_img->BPB_RsvdSecCnt		,	(mem_t)(fat12_FAT_img)	,	fat12_BootSector_img->BPB_FATSz16)	)
		kprint("Floppy riprovo\n");

	i=lung_logical-4;
	j=lung_physical-3;

	while(j>=0 && i>=0)
		{
		fat12_FAT_img[i		/2]=(((unsigned char*)fat12_FAT_img)[j+0]+( ((unsigned char*)fat12_FAT_img)[j+1]<<	8))&0xFFF;
		fat12_FAT_img[(i+2)	/2]=(((unsigned char*)fat12_FAT_img)[j+1]+( ((unsigned char*)fat12_FAT_img)[j+2]<<	8))>>	4;
  		i-=4;
		j-=3;
		}

	fat12_FAT_size=lung_logical/2;
	//fat12_FAT_modificata=FALSE;

	fat12_ROOTDIR_startcluster	=fat12_BootSector_img->BPB_RsvdSecCnt+(fat12_BootSector_img->BPB_NumFATs*fat12_BootSector_img->BPB_FATSz16);
	fat12_DATI_startcluster		=fat12_ROOTDIR_startcluster+((fat12_BootSector_img->BPB_RootEntCnt)*sizeof(struct fat12_FileEntry_t)/512)-2;			//Perche'????

	fat12_montato=TRUE;
	return TRUE;
	}


bool	fat12_umount()
	{
	if(!fat12_montato) return FALSE;

	mem_free((mem_t)fat12_BootSector_img);
	mem_free((mem_t)fat12_FAT_img);
	fat12_montato=FALSE;

	cache_clear(&floppy_cache);		//clear cache

	return TRUE;
	}


int	fat12_findindir(mem_t dir_img,int dir_size,char* nomefile)
	{
	int i,j;
	int entries,entrynum,entryfound;
	bool fatto,uguali;

	entries=dir_size/sizeof(struct fat12_FileEntry_t);

	entryfound=-1;

	for(entrynum=0,fatto=FALSE;!fatto && entrynum<entries;entrynum++)
		{
		if(((struct fat12_FileEntry_t*)dir_img)[entrynum].Name[0]==0x00)
			fatto=TRUE;

		if(((struct fat12_FileEntry_t*)dir_img)[entrynum].Name[0]!=0xE5 && ((struct fat12_FileEntry_t*)dir_img)[entrynum].Name[0]!=0x00 && ((struct fat12_FileEntry_t*)dir_img)[entrynum].Attributes!=0x0F)
			{
			//DA CORREGGERE
			for(i=0,j=0,uguali=TRUE;nomefile[j] && uguali;j++,i++)
				{
				if (nomefile[j]=='.')
					{
					j++;
					i=8;
					}
				uguali=(nomefile[j]==((struct fat12_FileEntry_t*)dir_img)[entrynum].Name[i]);
				}
			if (uguali && (' '==((struct fat12_FileEntry_t*)dir_img)[entrynum].Name[i] || i>11))
				{
				entryfound=entrynum;
				fatto=TRUE;
				}
			}
		}

	return entryfound;
	}


int	fat12_loadfiledir(mem_t buffer,unsigned short startcluster)
	{
	int i;

	if (startcluster==0)
		{
		while(!floppy_read(fat12_ROOTDIR_startcluster,buffer,(fat12_BootSector_img->BPB_RootEntCnt)*sizeof(struct fat12_FileEntry_t)/512))
			kprint("Floppy Riprovo\n");

		return ((fat12_BootSector_img->BPB_RootEntCnt)*sizeof(struct fat12_FileEntry_t)/512);
		}


	i=0;
	while(fat12_isUserData(startcluster))
		{
		while(!floppy_read((fat12_DATI_startcluster+startcluster),(buffer+512*i),1))
			kprint("Floppy Riprovo\n");
		i++;
		startcluster=fat12_nextcluster(startcluster);
		}


	if(!fat12_isEndMark(startcluster))
		return -1;

	return (i*512);
	}


bool	fat12_init()
	{
	fat12_montato=FALSE;
	return TRUE;
	}


bool	fat12_open(void** session,char* nomefile)
	{
	int i,j;
	int parser;
	int opt,finedir;
	bool mappato,errore,optbool;
	char buffer[24];					//Nella Fat on ci sono nomi piu' lunghi di 11!
	struct fat12_FileEntry_t aux;

	if(!fat12_montato) return FALSE;

	if(nomefile[0]!='\\')				//Deve esserci almeno la RootDir!
		{
		//kprint("Manca Root");
		return FALSE;
		}
	nomefile++;

	*session=(void*)mem_alloc((mem_t)sizeof(struct fat12_session_t));
	if(*session==0)
		{
		kprint("Manca Memoria\n");
		return FALSE;
		}

	if(fat12_loadfiledir((mem_t)FATFSSESSION->image,0)<0)
		{
		//kprint("RootDir Illeggibile\n");
		mem_free((mem_t)*session);
		return FALSE;
		}

	FATFSSESSION->cursore=0;
	FATFSSESSION->size=(fat12_BootSector_img->BPB_RootEntCnt)*sizeof(struct fat12_FileEntry_t);
	FATFSSESSION->directory=TRUE;

	mappato=errore=FALSE;
	parser=0;
	while(!mappato && !errore)
		{
		if(nomefile[parser]=='\0')
			{
			/*mappa dir corrente su file*/
			FATFSSESSION->cursore=0;
			finedir=FALSE;
			for(i=0;!finedir && i<(FATFSSESSION->size/sizeof(struct fat12_FileEntry_t));i++)
				{
				aux=FATIMGDIR[i];

				if(aux.Name[0]==0x00) finedir=TRUE;
				if(aux.Name[0]!=0xE5 && FATIMGDIR[i].Name[0]!=0x00  && aux.Name[0]!='.' && aux.Attributes!=0x0F)
					{
					/*Nome file*/
					optbool=TRUE;
					for(j=0;j<8;j++)
						{
						if((aux.Attributes & 0x10) && aux.Name[j]==' ' && optbool)
							{
							FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"\\");
							optbool=FALSE;
							}
						else
							FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"%c",aux.Name[j]);
						}
					if(!(aux.Attributes & 0x10))
						FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),".");

					for(j=0;j<3;j++)
							FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"%c",aux.Extension[j]);

					if(aux.Attributes & 0x10)
						FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore])," ");

					/*Attributi*/
					FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"\t%c",	(aux.Attributes&0x10)?'D':'-');
					FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"%c",	(aux.Attributes&0x01)?'R':'-');
					FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"%c",	(aux.Attributes&0x02)?'S':'-');
					FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"%c",	(aux.Attributes&0x04)?'H':'-');
					FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"%c",	(aux.Attributes&0x08)?'V':'-');

					/*Dimensione*/
					if(!(aux.Attributes&0x10))
						FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"\t%6d bytes",aux.FileLenght);

					FATFSSESSION->cursore+=ksprint(&(FATFSSESSION->image[FATFSSESSION->cursore]),"\n");
					}
				}
			FATFSSESSION->size=FATFSSESSION->cursore-1;//elimino l'ultimo accapo!!
			FATFSSESSION->cursore=0;

			return TRUE;
			}

		/*Prossimo Token*/
		for(i=0;nomefile[parser] && nomefile[parser]!='\\' && i<24;parser++,i++)
			buffer[i]=nomefile[parser];
		buffer[i]='\0';

		if(i>=24)
			{
			//kprint("nome troppo lungo %s\n",buffer);
			mem_free((mem_t)*session);
			return FALSE;
			}

		else if (nomefile[parser]=='\0')
			{
			/*Mappo il file*/
			opt=fat12_findindir((mem_t)FATFSSESSION->image,FATFSSESSION->size,buffer);

			if(opt<0)
				{
				//kprint("Non Esiste: %s\n",buffer);
				mem_free((mem_t)*session);
				return FALSE;
				}

			if(FATIMGDIR[opt].Attributes & 0x10)
				{
				//kprint("E' Directory: %s\n",buffer);
				mem_free((mem_t)*session);
				return FALSE;
				}

			FATFSSESSION->directory=FALSE;
			FATFSSESSION->cursore=0;
			FATFSSESSION->size=FATIMGDIR[opt].FileLenght;

			if(FATFSSESSION->size>FAT12_IMG_SIZE)
				{
				//kprint("Troppo Grande(%d): %s\n",FATFSSESSION->size,buffer);
				mem_free((mem_t)*session);
				return FALSE;
				}

			if(fat12_loadfiledir((mem_t)FATFSSESSION->image,FATIMGDIR[opt].StartCluster)<0)
				{
				//kprint("File Illeggibile\n");
				mem_free((mem_t)*session);
				return FALSE;
				}

			return TRUE;
			}

		else if (nomefile[parser]=='\\')
			{
			/*Scendo ad un a directory inferiore*/
			opt=fat12_findindir((mem_t)FATFSSESSION->image,FATFSSESSION->size,buffer);

			if(opt<0)
				{
				//kprint("Non Esiste: %s\n",buffer);
				mem_free((mem_t)*session);
				return FALSE;
				}

			if(!FATIMGDIR[opt].Attributes & 0x10)
				{
				//kprint("E' File: %s\n",buffer);
				mem_free((mem_t)*session);
				return FALSE;
				}

			FATFSSESSION->directory=TRUE;
			FATFSSESSION->cursore=0;

			if(FATFSSESSION->size>FAT12_IMG_SIZE)
				{
				//kprint("Troppo Grande(%d): %s\n",FATFSSESSION->size,buffer);
				mem_free((mem_t)*session);
				return FALSE;
				}

			FATFSSESSION->size=fat12_loadfiledir((mem_t)FATFSSESSION->image,FATIMGDIR[opt].StartCluster);
			if(FATFSSESSION->size<0)
				{
				//kprint("Dir Illeggibile\n");
				mem_free((mem_t)*session);
				return FALSE;
				}
			//kprint("Dir Lunghezza: %d\n",FATFSSESSION->size);
			parser++;		//Salta	'\\'
			/*Continua il While*/
			}


		else
			{
			//kprint("???\n");
			mem_free((mem_t)*session);
			return FALSE;
			}
		}


	return FALSE;
	}



bool	fat12_close(void** session)
	{
	return mem_free((mem_t)*session);
	}



int	fat12_seek(void** session,int modo,int puntatore)
	{
	int newcursore;

	switch(modo)
		{
		case (MREL) :
			newcursore=FATFSSESSION->cursore+puntatore;
			if(newcursore>=0 && newcursore<FATFSSESSION->size)
				KFSSESSION->cursore+=puntatore;
		break;

		case (MINIZIO) :
			if(puntatore>=0 && puntatore<FATFSSESSION->size)
				KFSSESSION->cursore=puntatore;
		break;

		case (MFINE) :
			newcursore=(FATFSSESSION->size-1)-puntatore;
			if(newcursore>=0 && newcursore<FATFSSESSION->size)
				KFSSESSION->cursore=newcursore;
		break;

		default :
			kprint("Modo Seek Sconosciuto\n");
		}

	return FATFSSESSION->cursore;
	}



int	fat12_read(void** session,int caratteri,char* buffer)
	{
	int cardacopiare;

	//quanti caratteri vuoi e quanti ne restano???
	if((FATFSSESSION->cursore+caratteri)<FATFSSESSION->size)
		cardacopiare=caratteri;
	else
		cardacopiare=(FATFSSESSION->size-FATFSSESSION->cursore);

	mem_copy((mem_t)(FATFSSESSION->image+FATFSSESSION->cursore),(mem_t)buffer,(mem_t)cardacopiare);

	FATFSSESSION->cursore+=cardacopiare;

	return cardacopiare;

	}



int	fat12_write (void** session,int caratteri,char* buffer)
	{
	return -1;
	}



bool	fat12_touch(char* nomefile)
	{
	return FALSE;
	}



bool	fat12_remv(char* nomefile)
	{
	return FALSE;
	}


/*Provvisorio*/
void fat12_mountproc()
	{
	if(!fat12_montato)
		fat12_mount();
	syscall_exit();
	//Qui NON DEVE arrivare!
	return;
	}

void fat12_umountproc()
	{
	if(fat12_montato)
		fat12_umount();
	syscall_exit();
	//Qui NON DEVE arrivare!
	return;
	}

void fat12_gomount()
	{
	int pid;
	struct task_mem_t uno_mem;

	if(pidcoda_elements(&Qnotask)<=0) return;

	pid=pidcoda_pop(&Qnotask);

	uno_mem.base=mem_alloc(40*1024);
	if(uno_mem.base==0) return;

	uno_mem.codesize=0;
	uno_mem.ustacksize=0;
	uno_mem.kstacksize=40*1024;

	task_init(&task_lista[pid],&uno_mem,pid,(mem_t)fat12_mountproc,KMODE,0x202);
	pidcoda_ins(&Qready,pid);

	return;
	}

void fat12_goumount()
	{
	int pid;
	struct task_mem_t uno_mem;

	if(pidcoda_elements(&Qnotask)<=0) return;

	pid=pidcoda_pop(&Qnotask);

	uno_mem.base=mem_alloc(40*1024);
	if(uno_mem.base==0) return;

	uno_mem.codesize=0;
	uno_mem.ustacksize=0;
	uno_mem.kstacksize=40*1024;

	task_init(&task_lista[pid],&uno_mem,pid,(mem_t)fat12_umountproc,KMODE,0x202);
	pidcoda_ins(&Qready,pid);

	return;
	}
/*Provvisorio*/
