
#include "mem.h"


struct mem_elemento_t* mem_used_base;
struct mem_elemento_t* mem_free_base;

unsigned long	mem_used_size,mem_free_size;
mem_t	mem_totale_size,mem_usata_size,mem_libera_size;		//Memoria Totale ,Memoria Usata,Memoria Libera

void	mem_init()
	{
	unsigned long i;
	struct cmos_dati_t	daticmos;

	i=MEM_TB_SIZE/sizeof(struct mem_elemento_t);

	mem_used_base	=(struct mem_elemento_t*)MEM_TB_BASE;
	mem_used_size	=i/MEM_TB_RAZIO;

	mem_free_base	=(struct mem_elemento_t*)(MEM_TB_BASE+(mem_used_size+1)*sizeof(struct mem_elemento_t));
	mem_free_size	=i-(i/MEM_TB_RAZIO);

	for(i=0;i<mem_free_size;i++)
		mem_free_base[i].base=mem_free_base[i].size=mem_free_base[i].flag=0;


	for(i=0;i<mem_used_size;i++)
		mem_used_base[i].base=mem_used_base[i].size=mem_used_base[i].flag=0;

	mem_totale_size=mem_conta_memoria();

	mem_libera_size=mem_totale_size;
	mem_usata_size=0;

	//Riservo un po' di memoria per il codice e setto il resto come libera!
	mem_used_base[0].base=0;
	mem_used_base[0].size=(mem_t)MEM_RESERVED1;
	mem_used_base[0].flag=1;

	mem_free_base[0].base=(mem_t)MEM_RESERVED1;
	mem_free_base[0].size=(mem_t)((640*1024)-MEM_RESERVED1);
	mem_free_base[0].flag=1;

	mem_used_base[1].base=(mem_t)(640*1024);
	mem_used_base[1].size=(mem_t)MEM_RESERVED2;
	mem_used_base[1].flag=1;

	mem_free_base[1].base=(mem_t)(MEM_RESERVED2+(640*1024));
	mem_free_base[1].size=mem_totale_size-(mem_t)(MEM_RESERVED2+(640*1024));
	mem_free_base[1].flag=1;

	mem_libera_size-=MEM_RESERVED1+MEM_RESERVED2;
	mem_usata_size+=MEM_RESERVED1+MEM_RESERVED2;

	cmos_getdati(&daticmos);					//Prende i dati da CMOS

	kprint("Memoria: %d Mb\n",(mem_totale_size/(1024*1024)));

	kprint("\tBase: %d kb\tExt: %d kb\tTest: %s\n",(daticmos.meminfo.base/1024),(daticmos.meminfo.ext/1024),(!daticmos.meminfo.compare_error)?"PASS":"FAIL");

	kprint("\tFb: 0x%8x Fs: %d\n\tUb: 0x%8x Us: %d\n",mem_free_base,mem_free_size,mem_used_base,mem_used_size);

	return;
	}


mem_t mem_alloc(mem_t requestsize)
	{
	unsigned long i,j;

	//Padding a 1024
	requestsize+=(1024-(requestsize%1024));

	//kprint("Mem Richiesta %d\n",requestsize/1024);

	//Cerca memoria libera nella tabella free
	for(i=0;i<mem_free_size && !(mem_free_base[i].flag==1 && mem_free_base[i].size>=requestsize);i++);
	if(i>=mem_free_size)
		return 0;

	//kprint("Mem Trovata %d\n",mem_free_base[i].size/1024);

	//Cerca posto nella tabella used
	for(j=0;j<mem_used_size && mem_used_base[j].flag==1;j++);
	if(j>=mem_used_size)	return 0;

	//Alloca in used  libera in free
	mem_used_base[j].base=mem_free_base[i].base;
	mem_used_base[j].size=requestsize;
	mem_used_base[j].flag=1;

	mem_free_base[i].base+=requestsize;
	mem_free_base[i].size-=requestsize;
	if(mem_free_base[i].size<=0) mem_free_base[i].flag=0;

	mem_libera_size-=requestsize;
	mem_usata_size+=requestsize;

	return mem_used_base[j].base;
	}

bool mem_free(mem_t puntatore)
	{
	unsigned long i,j;

	//Cerco la entry corrispondente al puntatore
	for(i=0;i<mem_used_size && !(puntatore==mem_used_base[i].base && mem_used_base[i].flag==1);i++);
	if(i>=mem_used_size)	 return FALSE;

	//Cerco posto libero nella tabella free
	for(j=0;j<mem_free_size && mem_free_base[j].flag==1;j++);
	if(j>=mem_free_size)	return FALSE;

	//Libero la memoria
	mem_free_base[j]=mem_used_base[i];//Si pole??
	mem_used_base[i].flag=0;

	mem_libera_size+=mem_used_base[i].size;
	mem_usata_size-=mem_used_base[i].size;

	return TRUE;
	}

bool mem_resize(mem_t puntatore,mem_t newsize)
	{
	unsigned long i,j;
	mem_t adiacente_base,adiacente_size,oldsize;

	//Padding a 1024
	newsize+=(1024-(newsize%1024));

	//Cerco la entry corrispondente al puntatore
	for(i=0;i<mem_used_size && !(puntatore==mem_used_base[i].base && mem_used_base[i].flag);i++);
	if(i>=mem_used_size)	return FALSE;

	if (newsize==mem_used_base[i].size) return TRUE;

	oldsize=mem_used_base[i].size;

	if(newsize>mem_used_base[i].size)
		{
		adiacente_base=mem_used_base[i].base+mem_used_base[i].size;
		adiacente_size=newsize-mem_used_base[i].size;
		//Cerca memoria libera nella tabella free(che sia adiacente a quella gia' disponibile)
		for(j=0;j<mem_free_size && !(mem_free_base[j].flag && mem_free_base[j].base==adiacente_base && mem_free_base[j].size>=adiacente_size);j++);
		if(j>=mem_free_size)
			return FALSE;

		//Aggiusta le tabelle
		mem_used_base[i].size=newsize;

		mem_free_base[j].base=+adiacente_size;
		mem_free_base[j].size=-adiacente_size;
		if(mem_free_base[j].size<=0) mem_free_base[j].flag=0;

		return TRUE;
		}
	else
		{
		//Cerco posto libero nella tabella free
		for(j=0;j<mem_free_size && mem_free_base[j].flag;j++);
		if(j>=mem_free_size)	return FALSE;

		//Ajjusto le tabelle
		mem_used_base[i].size=newsize;

		mem_free_base[j].base=mem_used_base[i].base+newsize;
		mem_free_base[j].size=mem_used_base[i].size-newsize;
		mem_free_base[j].flag=1;

		return TRUE;
		}

	mem_libera_size-=(newsize-oldsize);
	mem_usata_size+=(newsize-oldsize);

	return FALSE;
	}



void	mem_defrag()
	{
	int i,j;
	int fusi;
	mem_t fine;
	bool compattato;

	fusi=1;
	kprint("\nDefrag <:");
	while(fusi!=0)
		{
		fusi=0;
		for(i=0;i<mem_free_size;i++)
			if(mem_free_base[i].flag==1)
				{
				compattato=FALSE;
				fine=mem_free_base[i].base+mem_free_base[i].size;
				for(j=0;j<mem_free_size && !compattato;j++)
					if(mem_free_base[j].flag==1 && mem_free_base[j].base==fine && i!=j)
						{
						mem_free_base[i].size+=mem_free_base[j].size;
						mem_free_base[j].flag=0;
						compattato=TRUE;
						fusi++;
						}
				}
		kprint("%d:",fusi);
		}

	kprint("> Defrag\n");
	return;
	}




mem_t mem_totale()
	{
	return mem_totale_size;
	}

mem_t mem_libera()
	{
	return mem_libera_size;
	}

mem_t mem_usata()
	{
	return mem_usata_size;
	}

bool	mem_copy(mem_t porigine,mem_t pdestinazione,mem_t size)
	{
	mem_t i;

	//for(i=0;i<size;((unsigned char*)pdestinazione)[i]=((unsigned char*)porigine)[i++]);

	for(i=0;i<size;i++)
		asm volatile (
			"mov	(%0),%%eax		\n"
			"mov	%%eax,(%1)		\n"
			:
			:"r"((porigine+i)),"r"((pdestinazione+i))
			:"%eax"
			);

	return TRUE;
	}

bool	mem_clear(mem_t puntatore,mem_t size)
	{
	mem_t i;

	//for(i=0;i<size;((unsigned char*)puntatore)[i++]=0);
	for(i=0;i<size;i++)
		asm volatile (
			"xor		%%eax,%%eax	\n"
			"mov	%%eax,(%0)		\n"
			:
			:"r"((puntatore+i))
			:"%eax"
			);

	return TRUE;
	}

mem_t mem_conta_memoria()
	{
	mem_t mem_end, bse_end;
	register unsigned long *mem;
	mem_t mem_count, a;
	unsigned short memkb;
	unsigned char irq1, irq2;
	unsigned long cr0;	//Perche'??

 	//salvo lo stato degli irq
	irq1 = inportb(0x21);
	irq2 = inportb(0xA1);

	//disattivo tutti gli irq
	outportb(0x21, 0xFF);
	outportb(0xA1, 0xFF);

	mem_count = 0;
	memkb = 0;

	//memorizzo una copia di cr0
         __asm__ __volatile("movl %%cr0, %%ebx":"=a"(cr0)::"ebx");

	// disattivo la cache
	__asm__ __volatile__("wbinvd");

	//rimetto il cr0 con PE/CD e NW
	__asm__ __volatile__("movl %0, %%cr0": :"g"(cr0 | 0x00000001 | 0x40000000 | 0x20000000));

	do
		{
		memkb++;
		mem_count += 1024 * 1024;
		mem = (unsigned long *) mem_count;

		a = *mem;

		//0x55AA55AA è un valore preso a caso
		*mem = 0x55AA55AA;

		asm("": : :"memory");//Ajjiro le ottimizzazzioni di gcc

		//controllo se l'indirzzo è ancora valido
		if (*mem != 0x55AA55AA)
			mem_count = 0;
		else
			{
			*mem = 0xAA55AA55;
			asm("": : :"memory");
			if (*mem != 0xAA55AA55)
				mem_count = 0;
			}

		asm("": : :"memory");
		*mem = a;
		}
		while (memkb < 4096 && mem_count != 0);

         __asm__ __volatile__("movl %%ebx, %%cr0" :: "a" (cr0) : "ebx");

	mem_end = memkb << 20;
	mem = (unsigned long *) 0x413;
	bse_end = ((*mem) & 0xFFFF) << 6;

	outportb(0x21, irq1);
	outportb(0xA1, irq2);

	return mem_end;
	}

void mem_dumpscreen(mem_t address)
	{
	mem_t i,j;
	char tmpchar;

	for(j=0;j<8;j++)
		{
		kprint("<%6xh>\t",(address+(j*16)));

		for(i=0;i<16;i++)
			kprint("%2x ",(unsigned long)((unsigned char*)address)[(i+j*16)]);

		kprint("\t");

		for(i=0;i<16;i++)
			{
			tmpchar=((unsigned char*)address)[(i+j*16)];

			if ( (tmpchar>='0' && tmpchar<='9') ||  (tmpchar>='a' && tmpchar<='z') ||  (tmpchar>='A' && tmpchar<='Z') )
				kprint("%c",tmpchar);
			else
				kprint(".");
			}

		kprint("\n");
		}

	return;
	}

void mem_tabledumpscreen()
	{
	unsigned long i,j;

	kprint("Used Mem: %d kb\nFree Mem: %d kb\n",mem_usata()/1024,mem_libera()/1024);

	for(i=j=0;i<mem_free_size;i++)
		if(mem_free_base[i].flag==1)	j++;
	kprint("Free Table: %3d/%3d\n",j,mem_free_size);

	for(i=j=0;i<mem_used_size;i++)
		if(mem_used_base[i].flag==1)	j++;
	kprint("Used Table: %3d/%3d",j,mem_used_size);

	return;
	}

int mem_tabledump(char* buffer)
	{
	unsigned long i,j,cur;

	cur=ksprint(buffer,"Used Mem: %d kb\nFree Mem: %d kb\n",mem_usata()/1024,mem_libera()/1024);

	for(i=j=0;i<mem_free_size;i++)
		if(mem_free_base[i].flag==1)	j++;
	cur+=ksprint(&(buffer[cur]),"Free Table: %3d/%3d\n",j,mem_free_size);

	for(i=j=0;i<mem_used_size;i++)
		if(mem_used_base[i].flag==1)	j++;
	cur+=ksprint(&(buffer[cur]),"Used Table: %3d/%3d",j,mem_used_size);

	return cur;
	}

