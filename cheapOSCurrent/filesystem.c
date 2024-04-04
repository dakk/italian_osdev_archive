
#include	"filesystem.h"


//Dichiarazione filesystem--------------------------INIZIO

struct fsi_t tabella_filesystem[FILESYSTEM_NUM]=
	{
		{
		"KER:",
		kernel_init,
		//kernel_shutdown,
		kernel_open,
		kernel_close,
		kernel_seek,
		kernel_read,
		kernel_write,
		kernel_touch,
		kernel_remv,
		},
		{
		"FAT:",
		fat12_init,
		//kernel_shutdown,
		fat12_open,
		fat12_close,
		fat12_seek,
		fat12_read,
		fat12_write,
		fat12_touch,
		fat12_remv,
		},
		{
		"CON:",
		Console_init,
		//Console_shutdown,
		Console_open,
		Console_close,
		Console_seek,
		Console_read,
		Console_write,
		Console_touch,
		Console_remv,
		},
	};

//Dichiarazione filesystem--------------------------FINE


struct filesystem_accesso_t tabella_accessofile[TABELLA_ACCESSOFILE_LUNG];


bool filesystem_init()
	{
	int i;
	bool	opt;

	kprint("Inizzializzo FileSystem:");

	for(i=0;i<FILESYSTEM_NUM;i++)
		{
		kprint("\t");
		kprint(tabella_filesystem[i].nome);

		opt=tabella_filesystem[i].init();

		kprint(opt?" (OK)":" (ERR)");
		}

	kprint("\n");

	for(i=0;i<TABELLA_ACCESSOFILE_LUNG;i++)
		{
		kstrcpy(tabella_accessofile[i].nomefile,"");
		tabella_accessofile[i].pid=-1;
		tabella_accessofile[i].session=0;
		}

	kprint("\tTabella Accesso File %d elementi\n",TABELLA_ACCESSOFILE_LUNG);

	return TRUE;
	}

void	filesystem_tabella_dumpscreen()
	{
	int i;

	for(i=0;i<TABELLA_ACCESSOFILE_LUNG;i++)
		if(tabella_accessofile[i].pid>=0)
			kprint("Fd:%2d\tPid:%3d\tSession:0x%8x\tNomeFile:%s\n",i,tabella_accessofile[i].pid,tabella_accessofile[i].session,tabella_accessofile[i].nomefile);

	return ;
	}

int	filesystem_tabella_dump(char*buffer)
	{
	int i,cur;

	cur=0;
	for(i=0;i<TABELLA_ACCESSOFILE_LUNG;i++)
		if(tabella_accessofile[i].pid>=0)
			cur+=ksprint(buffer,"Fd:%2d\tPid:%3d\tSession:0x%8x\tNomeFile:%s\n",i,tabella_accessofile[i].pid,tabella_accessofile[i].session,tabella_accessofile[i].nomefile);

	return cur;
	}

int	pidcloseall(int pid)
	{
	int i,count;

	//Da migliorare si perde Memoria!!!
	count=0;
	for(i=0;i<TABELLA_ACCESSOFILE_LUNG;i++)
		if(tabella_accessofile[i].pid==pid)
			{
			kstrcpy(tabella_accessofile[i].nomefile,"");
			tabella_accessofile[i].pid=-1;
			tabella_accessofile[i].session=0;
			count++;
			}

	return count;
	}
