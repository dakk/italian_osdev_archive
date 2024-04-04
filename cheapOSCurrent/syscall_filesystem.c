
#include	"syscall_filesystem.h"

extern struct fsi_t tabella_filesystem[FILESYSTEM_NUM];
extern struct filesystem_accesso_t tabella_accessofile[TABELLA_ACCESSOFILE_LUNG];


int syscall_open(char* nomefile)
	{
	int i,fd,fs;
	bool opt ;
	struct task_t task;

	//Cerca il FS nella tabella
	opt=FALSE;
	for(fs=0;fs<FILESYSTEM_NUM && !opt;fs++)
		opt=(kstrncmp(tabella_filesystem[fs].nome,nomefile,VOLUME_NAME_LUNG));
	if (!opt) return -1;
	fs--;

	//File gia' in uso?
	opt=FALSE;
	for(i=0;i<TABELLA_ACCESSOFILE_LUNG && !opt;i++)
		opt=(tabella_accessofile[i].pid>0) && (kstrcmp(tabella_accessofile[i].nomefile,nomefile));
	if (opt) return -1;

	//Cerco un FD libero (Prendo Posto nella Tabella)
	opt=FALSE;
	for(fd=0;fd<TABELLA_ACCESSOFILE_LUNG && !opt;fd++)
		opt=(tabella_accessofile[fd].pid<0);
	if (!opt) return -1;
	fd--;

	//Prendo il mio FD
	task_getcorrente(&task);

	tabella_accessofile[fd].pid=task.pid;
	tabella_accessofile[fd].fs=fs;
	kstrcpy(tabella_accessofile[fd].nomefile,nomefile);

	//Prova ad Aprire..
	opt=tabella_filesystem[fs].open(&(tabella_accessofile[fd].session),&(tabella_accessofile[fd].nomefile[VOLUME_NAME_LUNG]));


	//OK
	if(opt)
		return fd;

	//ERR(Libero FD)
	kstrcpy(tabella_accessofile[fd].nomefile,"");
	tabella_accessofile[fd].pid=-1;

	return -1;
	}

bool syscall_close(int fd)
	{
	bool opt;
	struct task_t task;

	//Cerco Sessione Precedente
	if(fd>=TABELLA_ACCESSOFILE_LUNG)
		{
		kprint("FD Illegale\n");
		return -1;
		}
	task_getcorrente(&task);

	if(tabella_accessofile[fd].pid!=task.pid)
		{
		kprint("FD non del Task\n");
		return -1;
		}

	//Provo a Chiudere
	opt=tabella_filesystem[tabella_accessofile[fd].fs].close(&(tabella_accessofile[fd].session));

	//OK(Libero FD)
	if(opt)
		{

		tabella_accessofile[fd].pid=-1;
		kstrcpy(tabella_accessofile[fd].nomefile,"");
		return TRUE;
		}

	//ERR
	return FALSE;
	}

int syscall_seek(int fd,int modo,int spostamento)
	{
	unsigned int retval;
	struct task_t task;

	//Cerco Sessione Precedente

	if(fd>=TABELLA_ACCESSOFILE_LUNG)
		{
		kprint("FD Illegale\n");
		return -1;
		}
	task_getcorrente(&task);

	if(tabella_accessofile[fd].pid!=task.pid)
		{
		kprint("FD non del Task\n");
		return -1;
		}

	//Provo a Seekare
	retval=tabella_filesystem[tabella_accessofile[fd].fs].seek(&(tabella_accessofile[fd].session),modo,spostamento);

	return retval;
	}


int syscall_read(int fd,int lung,char* buffer)
	{
	int retval;
	struct task_t task;

	//Cerco Sessione Precedente
	if(fd>=TABELLA_ACCESSOFILE_LUNG)
		{
		kprint("FD Illegale\n");
		return -1;
		}
	task_getcorrente(&task);

	if(tabella_accessofile[fd].pid!=task.pid)
		{
		kprint("FD non del Task\n");
		return -1;
		}

	//Provo a Readare
	retval=tabella_filesystem[tabella_accessofile[fd].fs].read(&(tabella_accessofile[fd].session),lung,buffer);

	return retval;
	}


int syscall_write(int fd,int lung,char*buffer)
	{
	int retval;
	struct task_t task;

	//Cerco Sessione Precedente

	if(fd>=TABELLA_ACCESSOFILE_LUNG)
		{
		kprint("FD Illegale\n");
		return -1;
		}
	task_getcorrente(&task);

	if(tabella_accessofile[fd].pid!=task.pid)
		{
		kprint("FD non del Task\n");
		return -1;
		}

	//Provo a Writare
	retval=tabella_filesystem[tabella_accessofile[fd].fs].write(&(tabella_accessofile[fd].session),lung,buffer);

	return retval;
	}

bool syscall_remv(char* nomefile)
	{
	int i,fd,fs;
	bool opt ;
	struct task_t task;

 	//Cerca il FS nella tabella
	opt=FALSE;
	for(fs=0;fs<FILESYSTEM_NUM && !opt;fs++)
		opt=(kstrncmp(tabella_filesystem[fs].nome,nomefile,VOLUME_NAME_LUNG));
	if (!opt) return -1;
	fs--;

	//File gia' in uso?
	opt=FALSE;
	for(i=0;i<TABELLA_ACCESSOFILE_LUNG && !opt;i++)
		opt=(tabella_accessofile[i].pid>0) && (kstrcmp(tabella_accessofile[i].nomefile,nomefile));
	if (opt) return -1;

	//Cerco un FD libero (Prendo Posto nella Tabella)
	opt=FALSE;
	for(fd=0;fd<TABELLA_ACCESSOFILE_LUNG && !opt;fd++)
		opt=(tabella_accessofile[fd].pid<0);
	if (!opt) return -1;
	fd--;

	//Prendo il mio FD
	task_getcorrente(&task);

	tabella_accessofile[fd].pid=task.pid;
	tabella_accessofile[fd].fs=fs;
	kstrcpy(tabella_accessofile[fd].nomefile,nomefile);


	//Provo a Rimuovere
	opt=tabella_filesystem[fs].remv(&(tabella_accessofile[fd].nomefile[VOLUME_NAME_LUNG]));


	//Libero FD
	kstrcpy(tabella_accessofile[fd].nomefile,"");
	tabella_accessofile[fd].pid=-1;

	return opt;
	}


bool syscall_touch(char* nomefile)
	{
	int i,fd,fs;
	bool opt ;
	struct task_t task;

 	//Cerca il FS nella tabella
	opt=FALSE;
	for(fs=0;fs<FILESYSTEM_NUM && !opt;fs++)
		opt=(kstrncmp(tabella_filesystem[fs].nome,nomefile,VOLUME_NAME_LUNG));
	if (!opt) return -1;
	fs--;

	//File gia' in uso?
	opt=FALSE;
	for(i=0;i<TABELLA_ACCESSOFILE_LUNG && !opt;i++)
		opt=(tabella_accessofile[i].pid>0) && (kstrcmp(tabella_accessofile[i].nomefile,nomefile));
	if (opt) return -1;

	//Cerco un FD libero (Prendo Posto nella Tabella)
	opt=FALSE;
	for(fd=0;fd<TABELLA_ACCESSOFILE_LUNG && !opt;fd++)
		opt=(tabella_accessofile[fd].pid<0);
	if (!opt) return -1;
	fd--;

	//Prendo il mio FD
	task_getcorrente(&task);

	tabella_accessofile[fd].pid=task.pid;
	tabella_accessofile[fd].fs=fs;
	kstrcpy(tabella_accessofile[fd].nomefile,nomefile);


	//Provo a Creare
	opt=tabella_filesystem[fs].touch(&(tabella_accessofile[fd].nomefile[VOLUME_NAME_LUNG]));


	//Libero FD
	kstrcpy(tabella_accessofile[fd].nomefile,"");
	tabella_accessofile[fd].pid=-1;

	return opt;
	}
