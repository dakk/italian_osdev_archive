
#include		"fs_kernel.h"


extern struct pidcoda_t Qready,Qwait;
extern struct keycoda_t Qbufferkey;
extern struct cache_t floppy_cache;
extern struct fsi_t tabella_filesystem[FILESYSTEM_NUM];
extern struct task_t task_lista[MAX_TASK_NUM];
extern int task_corrente_pid;
extern bool fat12_montato;
struct fat12_BootSector_t*	fat12_BootSector_img;

char kernel_fsfile[KFFILENUM][16]=
	{
	"tasks\\",
	"pids",
	"keybuffer",
	"mem",
	"random",
	"credits",
	"time",
	"fat",
	"cache",
	"volumes",
	"sessions",
	};

bool		kernel_init()
		{
		return TRUE;
		}

/*
bool	kernel_shutdown();
	{
	return TRUE;
	}
*/

bool	kernel_open(void** session,char* nomefile)
	{
	int i;
	int virtualfilenum;
	int opt,pid,lung;
	struct time_tm_t ora;

	virtualfilenum=-1;

	if(nomefile[0]!='\\')
		{
		kprint("Manca RootDir\n");
		return FALSE;
		}

	nomefile++;
	if(nomefile[0]=='\0')//Directory List
		{
		*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
		if(*session==0) return FALSE;

		KFSSESSION->size=0;
		for(i=0;i<KFFILENUM;i++)
			KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"%s\n",kernel_fsfile[i]);

		KFSSESSION->size--;//Elimino l'accapo finale!
		KFSSESSION->cursore=0;
		return TRUE;
		}


	for(i=0;i<KFFILENUM;i++)
		if(kstrcmp(kernel_fsfile[i],nomefile))
			virtualfilenum=i;

	switch(virtualfilenum)
		{
		case 0:		//task//
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;

			KFSSESSION->size=0;

			pid=task_corrente_pid;
			KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"task%3d\t",task_lista[pid].pid);
			KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"C");
			KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),((task_lista[pid].modo==KMODE)?"K":"U"));
			KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\t%6d bytes\n",(task_lista[pid].mem.codesize+task_lista[pid].mem.ustacksize+task_lista[pid].mem.kstacksize));

			lung=pidcoda_elements(&Qready);
			for(i=0;i<lung;i++)
				{
				pid=pidcoda_pop(&Qready);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"task%3d\t",task_lista[pid].pid);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"R");
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),((task_lista[pid].modo==KMODE)?"K":"U"));
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\t%6d bytes\n",(task_lista[pid].mem.codesize+task_lista[pid].mem.ustacksize+task_lista[pid].mem.kstacksize));
				pidcoda_ins(&Qready,pid);
				}

			lung=pidcoda_elements(&Qwait);
			for(i=0;i<lung;i++)
				{
				pid=pidcoda_pop(&Qwait);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"task%3d\t",task_lista[pid].pid);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"W");
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),((task_lista[pid].modo==KMODE)?"K":"U"));
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\t%6d bytes\n",(task_lista[pid].mem.codesize+task_lista[pid].mem.ustacksize+task_lista[pid].mem.kstacksize));
				pidcoda_ins(&Qwait,pid);
				}


			KFSSESSION->size--;
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 1:		//pids
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;
			KFSSESSION->size=ksprint(KFSSESSION->image,"Qready:");
			KFSSESSION->size+=pidcoda_dump(&(KFSSESSION->image[KFSSESSION->size]),&Qready);
			KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\nQwait :");
			KFSSESSION->size+=pidcoda_dump(&(KFSSESSION->image[KFSSESSION->size]),&Qwait);
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 2:		//keybuffer
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;
			KFSSESSION->size=ksprint(KFSSESSION->image,"Qbufferkey:");
			KFSSESSION->size+=keycoda_dump(&(KFSSESSION->image[KFSSESSION->size]),&Qbufferkey);
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 3:		//mem
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;
			KFSSESSION->size=mem_tabledump(KFSSESSION->image);
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 4:		//random
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;

			KFSSESSION->size=5;

   			opt=krand();
			KFSSESSION->image[0]=opt & 0xFF;
			KFSSESSION->image[1]=(opt>>8) & 0xFF;
			KFSSESSION->image[2]=(opt>>16) & 0xFF;;
			KFSSESSION->image[3]=(opt>>24) & 0xFF;;
			KFSSESSION->image[4]='\0';

			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 5:		//credits
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;
			KFSSESSION->size=ksprint(KFSSESSION->image,"cippOS by Pigna e AlexMfM\0");
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 6:		//time
			time_tm(&ora);
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;
			KFSSESSION->size=ksprint(KFSSESSION->image,"%2d:%2d:%2d",(unsigned long)ora.ore,(unsigned long)ora.min,(unsigned long)ora.sec);
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 7:		//fat
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;

			KFSSESSION->size=0;

			if(!fat12_montato)
				KFSSESSION->size+=ksprint(KFSSESSION->image,"SMontato");
			else
				{
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"Floppy BootSector\n");

				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tjmpBoot: ");
				for(i=0;i<3;i++)
					KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"%c",fat12_BootSector_img->BS_jmpBoot[i]);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\n");

				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tOEMName: ");
				for(i=0;i<8;i++)
					KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"%c",fat12_BootSector_img->BS_OEMName[i]);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\nBPB\n");

				//BPB---------------------------------------------------
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tBytsPerSec: %d\n",fat12_BootSector_img->BPB_BytsPerSec);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tSecPerClus: %d\n",fat12_BootSector_img->BPB_SecPerClus);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tRsvdSecCnt: %d\n",fat12_BootSector_img->BPB_RsvdSecCnt);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tNumFATs:    %d\n",fat12_BootSector_img->BPB_NumFATs);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tRootEntCnt: %d\n",fat12_BootSector_img->BPB_RootEntCnt);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tTotsec16:   %d\n",fat12_BootSector_img->BPB_Totsec16);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tMedia:      %d\n",fat12_BootSector_img->BPB_Media);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tFATSz16:    %d\n",fat12_BootSector_img->BPB_FATSz16);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tSecPerTrk:  %d\n",fat12_BootSector_img->BPB_SecPerTrk);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tNumHeads:   %d\n",fat12_BootSector_img->BPB_NumHeads);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tHiddSec:    %d\n",fat12_BootSector_img->BPB_HiddSec);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tTotSec32:   %d\n",fat12_BootSector_img->BPB_TotSec32);

				//Solo per FAT12 e FAT 16-----------------------
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"FAT12/16\n");
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tDrvNum:    %d\n",fat12_BootSector_img->BS_DrvNum);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tReserved1: %d\n",fat12_BootSector_img->BS_Reserved1);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tBootSig:   %d\n",fat12_BootSector_img->BS_BootSig);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tVolID:     %d\n",fat12_BootSector_img->BS_VolID);

				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tVolLab:    ");
				for(i=0;i<11;i++)
					KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"%c",fat12_BootSector_img->BS_VolLab[i]);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\n");

				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\tFSType:    ");
				for(i=0;i<8;i++)
					KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"%c",fat12_BootSector_img->BS_FilSysType[i]);
				}

			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 8:		//cache
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;
			KFSSESSION->size=cache_dump(KFSSESSION->image,&floppy_cache);
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 9:		//volumes
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;

			KFSSESSION->size=0;
			for(i=0;i<FILESYSTEM_NUM;i++)
				{
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]), tabella_filesystem[i].nome);
				KFSSESSION->size+=ksprint(&(KFSSESSION->image[KFSSESSION->size]),"\n");
				}
			KFSSESSION->size--;
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		case 10:		//sessions
			*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
			if(*session==0) return FALSE;

			KFSSESSION->size=filesystem_tabella_dump(KFSSESSION->image);

			KFSSESSION->size--;
			KFSSESSION->cursore=0;
			return TRUE;
		break;

		default:

			if(kstrncmp(nomefile,"tasks\\task",10) && kstrlen(nomefile)==13)
				{
				//SottoDir tasks
				kprint("SottoDir\n");
				for(i=0;i<3;i++)
					if(nomefile[10+i]<'0' || nomefile[10+i]>'9') return FALSE;

				pid=katoi(&(nomefile[10]));

				if(task_lista[pid].pid!=pid) return FALSE;

				*session=(void*)mem_alloc((mem_t)sizeof(struct kernelfs_session_t));
				if(*session==0) return FALSE;

				KFSSESSION->size=task_dump(KFSSESSION->image,&(task_lista[pid]));
				KFSSESSION->cursore=0;

				return TRUE;
				}
			else
			//kprint("File Inesistente\n");
			//file Inesistente
			return FALSE;
		}

	return FALSE;
	}

bool	kernel_close(void** session)
	{
	return mem_free((mem_t)*session);
	}

int	kernel_seek(void** session,int modo,int puntatore)
	{
	int newcursore;

	switch(modo)
		{
		case (MREL) :
			newcursore=KFSSESSION->cursore+puntatore;
			if(newcursore>=0 && newcursore<KFSSESSION->size)
				KFSSESSION->cursore+=puntatore;
		break;

		case (MINIZIO) :
			if(puntatore>=0 && puntatore<KFSSESSION->size)
				KFSSESSION->cursore=puntatore;
		break;

		case (MFINE) :
			newcursore=(KFSSESSION->size-1)-puntatore;
			if(newcursore>=0 && newcursore<KFSSESSION->size)
				KFSSESSION->cursore=newcursore;
		break;

		default :
			kprint("Modo Seek Sconosciuto\n");
		}

	return KFSSESSION->cursore;
	}

int	kernel_read(void** session,int caratteri,char* buffer)
	{
	int cardacopiare;

	//quanti caratteri vuoi e quanti ne restano???
	if((KFSSESSION->cursore+caratteri)<KFSSESSION->size)
		cardacopiare=caratteri;
	else
		cardacopiare=(KFSSESSION->size-KFSSESSION->cursore);

	mem_copy((mem_t)(KFSSESSION->image+KFSSESSION->cursore),(mem_t)buffer,(mem_t)cardacopiare);

	KFSSESSION->cursore+=cardacopiare;

	return cardacopiare;
	}

int	kernel_write (void** session,int caratteri,char* buffer)
	{
	return -1;
	}

bool	kernel_touch(char* nomefile)
	{
	return FALSE;
	}

bool	kernel_remv(char* nomefile)
	{
	return FALSE;
	}
