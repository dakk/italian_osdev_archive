
#include	"task.h"


//TSS-----------------------------------------------------------------------------------------------------
struct TSS tss=
	{
	0,																	//backlink
	(KSTACK_SIZE+KSTACK_BASE),KERNEL_MAINSTACK,						//esp0,ss0
	0,0,																	//esp1,ss1
	0,0,																	//esp2,ss2
	0,																	//cr3
	0,0x002,																//eip,eflags
	0,0,0,0,																//eax,ecx,edx,ebx
	0,0,0,0,																//esp,ebp,esi,edi
	KERNEL_DATA,KERNEL_CODE,KERNEL_MAINSTACK,						//es,cs,ss,ds,fs,gs
	KERNEL_DATA,KERNEL_DATA,KERNEL_DATA,
	0,0,0,																//ldtr,altriflag,iomapbase
	};

unsigned char tss_selector=KERNEL_TSS;

//GDT----------------------------------------------------------------------------------------------------
desc_table(GDT, 9)
	{
	{dummy:0},																//0x00	DUMMY

	stnd_desc(0,0xFFFFF, (D_CODE + D_READ + D_32BIT + D_GR4K)),					//0x08	KERNEL_CODE
	stnd_desc(0,0xFFFFF, (D_DATA + D_WRITE + D_32BIT + D_GR4K)),					//0x10	KERNEL_DATA
	stnd_desc(0,0xFFFFF, (D_DATA + D_WRITE + D_32BIT + D_GR4K)),					//0x18	KERNEL_MAINSTACK

	{dummy:0},																//0x20	USER_CODE
	{dummy:0},																//0x28	USER_DATA
	{dummy:0},																//0x30	USER_STACK

	stnd_desc(0,0xFFFFF, (D_DATA + D_WRITE + D_32BIT + D_GR4K)),					//0x38	KERNEL_TASKSTACK
	{dummy:0},																//0x40	KERNEL_TSS
	};


//Puntatore a	GDT---------------------------------------------------------------------------------
struct
	{
	unsigned short limit __attribute__ ((packed));			/*Lunghezza*/
	union DT_entry *idt __attribute__ ((packed));			/*Puntatore*/
	}	loadgdt = { (9 * sizeof(union DT_entry) - 1), GDT };



//Top dello Stack Principale del Kernel----------
const unsigned int 	kernel_main_stack_top=KSTACK_BASE+KSTACK_SIZE;


//Task Attuale Stato  USER-------------------------
unsigned int	task_corrente_ueip=0;
unsigned int	task_corrente_ucs=0;
unsigned int	task_corrente_ueflags=0;

unsigned int	task_corrente_ueax=0;
unsigned int	task_corrente_uebx=0;
unsigned int	task_corrente_uecx=0;
unsigned int	task_corrente_uedx=0;

unsigned int	task_corrente_uesp=0;
unsigned int	task_corrente_uebp=0;
unsigned int	task_corrente_uesi=0;
unsigned int	task_corrente_uedi=0;

unsigned int	task_corrente_uds=0;
unsigned int	task_corrente_ues=0;
unsigned int	task_corrente_ufs=0;
unsigned int	task_corrente_ugs=0;
unsigned int	task_corrente_uss=0;


//Task Attuale Stato  KERNEL-----------------------
unsigned int	task_corrente_keip=0;
unsigned int	task_corrente_kcs=0;
unsigned int	task_corrente_keflags=0;

unsigned int	task_corrente_keax=0;
unsigned int	task_corrente_kebx=0;
unsigned int	task_corrente_kecx=0;
unsigned int	task_corrente_kedx=0;

unsigned int	task_corrente_kesp=0;
unsigned int	task_corrente_kebp=0;
unsigned int	task_corrente_kesi=0;
unsigned int	task_corrente_kedi=0;

unsigned int	task_corrente_kds=0;
unsigned int	task_corrente_kes=0;
unsigned int	task_corrente_kfs=0;
unsigned int	task_corrente_kgs=0;
unsigned int	task_corrente_kss=0;

//Task Attuale	Altre Cose------------------------------
unsigned int 	task_corrente_modo=UMODE;
int 			task_corrente_pid=0;

unsigned int	task_corrente_kstacksize=0;			//Per ovviare ad un problema brutto!!
unsigned int	task_corrente_kstacktop=0;			//Per Quando entro in KMODE


bool	task_tss_init()
	{
	//Carica TSS (Per il cambio di privilegi)
	desc_setstnd(&(GDT[8]),(unsigned int)&tss,(unsigned long)sizeof(struct TSS),D_TSS);

	tss_selector=KERNEL_TSS;
	asm (
		"ltr tss_selector"				//Load TSS
		:
		:
		);

	return TRUE;
	}

bool	task_init(struct task_t* task,struct task_mem_t* mem,int pid,unsigned int eip,int modo,unsigned int eflags)
	{

	task->pid=pid;					// 	Pid del Task
	task->modo=modo;				//	Modo di partenza del Task
	task->mem=*mem;				//	Copia il contenutp di mem in task->mem

	//Inizio il processo in KMODE(usato per IDLE ed all'avvio!)
	if(modo==KMODE)
		{
		task->kernelRegs.eip	=eip;			//eip iniziale
		task->kernelRegs.cs		=KERNEL_CODE;	//cs	iniziale
		task->kernelRegs.eflags	=eflags;			//eflags iniziale

		task->userRegs.eip		=0;				//
		task->userRegs.cs		=USER_CODE;	//	Per Bellezza!(Questi non vengono usati!)
		task->userRegs.eflags	=0x202;			//
		}
	//Se non e' KMODE e' UMODE dimodoche' non fa' danno!!
	else
		{
		task->kernelRegs.eip	=0;				//
		task->kernelRegs.cs		=KERNEL_CODE;	//	Per Bellezza!(Questi non vengono usati!)
		task->kernelRegs.eflags	=0x002;			//

		task->userRegs.eip		=eip;			//eip iniziale
		task->userRegs.cs		=USER_CODE;	//cs	iniziale
		task->userRegs.eflags	=eflags;			//eflags iniziale
		}

	//UserMode-------------------------------------
	task->userRegs.eax=0;
	task->userRegs.ebx=0;
	task->userRegs.ecx=0;
	task->userRegs.edx=0;

	task->userRegs.esp=(mem->codesize+mem->ustacksize);	//Top dello StackUser
	task->userRegs.ebp=(mem->codesize+mem->ustacksize);
	task->userRegs.esi=0;
	task->userRegs.edi=0;

	task->userRegs.gs=	USER_DATA;
	task->userRegs.ds=	USER_DATA;
	task->userRegs.es=	USER_DATA;
	task->userRegs.fs=	USER_DATA;
	task->userRegs.ss=	USER_STACK;

	//KernelMode-----------------------------------	Questo e' Inutile!(per ora!)
	task->kernelRegs.eax=0;
	task->kernelRegs.ebx=0;
	task->kernelRegs.ecx=0;
	task->kernelRegs.edx=0;

	task->kernelRegs.esp=(mem->codesize+mem->ustacksize+mem->kstacksize)+mem->base;	//Top dello kstack (e' riferimento ad 0 LINEARE)
	task->kernelRegs.ebp=(mem->codesize+mem->ustacksize+mem->kstacksize)+mem->base;
	task->kernelRegs.esi=0;
	task->kernelRegs.edi=0;

	task->kernelRegs.gs=KERNEL_DATA;
	task->kernelRegs.ds=KERNEL_DATA;
	task->kernelRegs.es=KERNEL_DATA;
	task->kernelRegs.fs= KERNEL_DATA;
	task->kernelRegs.ss=KERNEL_TASKSTACK;

	return TRUE;
	}


void task_setcorrente(struct task_t* task)
	{
	task_corrente_pid=task->pid;
 	task_corrente_modo=task->modo;

	//UserMode-------------------------------------
	task_corrente_ueip=task->userRegs.eip;
	task_corrente_ueflags=task->userRegs.eflags;
	task_corrente_ucs=task->userRegs.cs;

	task_corrente_ueax=task->userRegs.eax;
	task_corrente_uebx=task->userRegs.ebx;
	task_corrente_uecx=task->userRegs.ecx;
	task_corrente_uedx=task->userRegs.edx;

	task_corrente_uebp=task->userRegs.ebp;
	task_corrente_uesp=task->userRegs.esp;
	task_corrente_uedi=task->userRegs.edi;
	task_corrente_uesi=task->userRegs.esi;

	task_corrente_uds=task->userRegs.ds;
	task_corrente_ugs=task->userRegs.gs;
	task_corrente_ues=task->userRegs.es;
	task_corrente_ufs=task->userRegs.fs;
	task_corrente_uss=task->userRegs.ss;

	//KernelMode-----------------------------------
	task_corrente_keip=task->kernelRegs.eip;
	task_corrente_keflags=task->kernelRegs.eflags;
	task_corrente_kcs=task->kernelRegs.cs;

	task_corrente_keax=task->kernelRegs.eax;
	task_corrente_kebx=task->kernelRegs.ebx;
	task_corrente_kecx=task->kernelRegs.ecx;
	task_corrente_kedx=task->kernelRegs.edx;

	task_corrente_kebp=task->kernelRegs.ebp;
	task_corrente_kesp=task->kernelRegs.esp;
	task_corrente_kedi=task->kernelRegs.edi;
	task_corrente_kesi=task->kernelRegs.esi;

	task_corrente_kds=task->kernelRegs.ds;
	task_corrente_kgs=task->kernelRegs.gs;
	task_corrente_kes=task->kernelRegs.es;
	task_corrente_kfs=task->kernelRegs.fs;
	task_corrente_kss=task->kernelRegs.ss;

	//Mem------------------------------------------
	desc_setstnd(&(GDT[4]),task->mem.base,	task->mem.codesize,						(D_CODE + D_READ + D_32BIT));
	//e' giusto compaia solo Qui GDT[5]
	desc_setstnd(&(GDT[5]),task->mem.base,	(task->mem.codesize+task->mem.ustacksize),		(D_DATA + D_WRITE + D_32BIT));
	desc_setstnd(&(GDT[6]),task->mem.base,	(task->mem.codesize+task->mem.ustacksize),		(D_DATA + D_WRITE + D_32BIT));

	//Per ovviare ad un problema brutto!!
	task_corrente_kstacksize=task->mem.kstacksize;
	//e' giusto compaia solo Qui task_corrente_kstacktop
	task_corrente_kstacktop=(task->mem.base+task->mem.codesize+task->mem.ustacksize+task->mem.kstacksize);

	return;
	}


void task_getcorrente(struct task_t* task)			//Okkio non aggiorna i segmenti!
	{
	task->pid=task_corrente_pid;
	task->modo=task_corrente_modo;

	//UserMode-------------------------------------
	task->userRegs.eip=task_corrente_ueip;
	task->userRegs.eflags=task_corrente_ueflags;
	task->userRegs.cs=task_corrente_ucs;

	task->userRegs.eax=task_corrente_ueax;
	task->userRegs.ebx=task_corrente_uebx;
	task->userRegs.ecx=task_corrente_uecx;
	task->userRegs.edx=task_corrente_uedx;

	task->userRegs.esp=task_corrente_uesp;
	task->userRegs.ebp=task_corrente_uebp;
	task->userRegs.esi=task_corrente_uesi;
	task->userRegs.edi=task_corrente_uedi;

	task->userRegs.ds=task_corrente_uds;
	task->userRegs.gs=task_corrente_ugs;	
	task->userRegs.es=task_corrente_ues;
	task->userRegs.fs=task_corrente_ufs;
	task->userRegs.ss=task_corrente_uss;

	//KernelMode-----------------------------------
	task->kernelRegs.eip=task_corrente_keip;
	task->kernelRegs.eflags=task_corrente_keflags;
	task->kernelRegs.cs=task_corrente_kcs;

	task->kernelRegs.eax=task_corrente_keax;
	task->kernelRegs.ebx=task_corrente_kebx;
	task->kernelRegs.ecx=task_corrente_kecx;
	task->kernelRegs.edx=task_corrente_kedx;

	task->kernelRegs.esp=task_corrente_kesp;
	task->kernelRegs.ebp=task_corrente_kebp;
	task->kernelRegs.esi=task_corrente_kesi;
	task->kernelRegs.edi=task_corrente_kedi;

	task->kernelRegs.ds=task_corrente_kds;
	task->kernelRegs.gs=task_corrente_kgs;
	task->kernelRegs.es=task_corrente_kes;
	task->kernelRegs.fs=task_corrente_kfs;
	task->kernelRegs.ss=task_corrente_kss;

	//	Mem
	task->mem.base=desc_getstnd_base(&(GDT[4]));

	task->mem.codesize=desc_getstnd_limit(&(GDT[4]));
	task->mem.ustacksize=desc_getstnd_limit(&(GDT[6]))-task->mem.codesize;
	//Per ovviare ad un problema brutto!!
	task->mem.kstacksize=task_corrente_kstacksize;

	return;
	}


void	task_dumpscreen(const struct task_t* task)
	{

	kprint("Pid: %3d\t",task->pid);

	if(task->modo==KMODE)
		{
		kprint("KernelMode\n");

		kprint("eip: 0x%8x\tcs : 0x%4x\teflags: %8x\n",task->kernelRegs.eip,task->kernelRegs.cs,task->kernelRegs.eflags);

		kprint("eax: 0x%8x\tebx: 0x%8x\tecx: 0x%8x\tedx: 0x%8x\n",task->kernelRegs.eax,task->kernelRegs.ebx,task->kernelRegs.ecx,task->kernelRegs.edx);
		kprint("esp: 0x%8x\tebp: 0x%8x\tesi: 0x%8x\tedi: 0x%8x\n",task->kernelRegs.esp,task->kernelRegs.ebp,task->kernelRegs.esi,task->kernelRegs.edi);

		kprint("ds : 0x%4x\tes : 0x%4x\tfs : 0x%4x\tgs : 0x%4x\tss : 0x%4x\n",task->kernelRegs.ds,task->kernelRegs.es,task->kernelRegs.fs,task->kernelRegs.gs,task->kernelRegs.ss);
		}
	else
		{
		kprint("UserMode\n");

		kprint("eip: 0x%8x\tcs : 0x%4x\teflags: %8x\n",task->userRegs.eip,task->userRegs.cs,task->userRegs.eflags);

		kprint("eax: 0x%8x\tebx: 0x%8x\tecx: 0x%8x\tedx: 0x%8x\n",task->userRegs.eax,task->userRegs.ebx,task->userRegs.ecx,task->userRegs.edx);
		kprint("esp: 0x%8x\tebp: 0x%8x\tesi: 0x%8x\tedi: 0x%8x\n",task->userRegs.esp,task->userRegs.ebp,task->userRegs.esi,task->userRegs.edi);

		kprint("ds : 0x%4x\tes : 0x%4x\tfs : 0x%4x\tgs : 0x%4x\tss : 0x%4x\n",task->userRegs.ds,task->userRegs.es,task->userRegs.fs,task->userRegs.gs,task->userRegs.ss);
		}

	kprint("Code   : <0x%8x> 0x%8x\t",task->mem.base,task->mem.codesize);
	kprint("UStack : <0x%8x> 0x%8x\n",(task->mem.base+task->mem.codesize),task->mem.ustacksize);
	kprint("Data   : <0x%8x> 0x%8x\t",task->mem.base,(task->mem.codesize+task->mem.ustacksize));
	kprint("KStack : <0x%8x> 0x%8x\t",(task->mem.base+task->mem.codesize+task->mem.ustacksize),task->mem.kstacksize);
	}


void	task_dumpscreen_current()
	{
	mem_t base,codesize,ustacksize;

	kprint("Pid: %3d\t",task_corrente_pid);

	if(task_corrente_modo==KMODE)
		{
		kprint("KernelMode\n");

		kprint("eip: 0x%8x\tcs : 0x%4x\teflags: %8x\n",task_corrente_keip,task_corrente_kcs,task_corrente_keflags);

		kprint("eax: 0x%8x\tebx: 0x%8x\tecx: 0x%8x\tedx: 0x%8x\n",task_corrente_keax,task_corrente_kebx,task_corrente_kecx,task_corrente_kedx);
		kprint("esp: 0x%8x\tebp: 0x%8x\tesi: 0x%8x\tedi: 0x%8x\n",task_corrente_kesp,task_corrente_kebp,task_corrente_kesi,task_corrente_kedi);

		kprint("ds : 0x%4x\tes : 0x%4x\tfs : 0x%4x\tgs : 0x%4x\tss : 0x%4x\n",task_corrente_kds,task_corrente_kes,task_corrente_kfs,task_corrente_kgs,task_corrente_kss);
		}
	else
			{
		kprint("UserMode\n");

		kprint("eip: 0x%8x\tcs : 0x%4x\teflags: %8x\n",task_corrente_ueip,task_corrente_ucs,task_corrente_ueflags);

		kprint("eax: 0x%8x\tebx: 0x%8x\tecx: 0x%8x\tedx: 0x%8x\n",task_corrente_ueax,task_corrente_uebx,task_corrente_uecx,task_corrente_uedx);
		kprint("esp: 0x%8x\tebp: 0x%8x\tesi: 0x%8x\tedi: 0x%8x\n",task_corrente_uesp,task_corrente_uebp,task_corrente_uesi,task_corrente_uedi);

		kprint("ds : 0x%4x\tes : 0x%4x\tfs : 0x%4x\tgs : 0x%4x\tss : 0x%4x\n",task_corrente_uds,task_corrente_ues,task_corrente_ufs,task_corrente_ugs,task_corrente_uss);
		}

	base=desc_getstnd_base(&(GDT[4]));
	codesize=desc_getstnd_limit(&(GDT[4]));
	ustacksize=desc_getstnd_limit(&(GDT[6]))-codesize;


	kprint("Code   : <0x%8x> 0x%8x\t",base,codesize);
	kprint("UStack : <0x%8x> 0x%8x\n",(base+codesize),ustacksize);
	kprint("Data   : <0x%8x> 0x%8x\t",base,(codesize+ustacksize));
	kprint("KStack : <0x%8x> 0x%8x\t",(base+codesize+ustacksize),task_corrente_kstacksize);
	}

int	task_dump(char* buffer,const struct task_t* task)
	{
	int cars;

	cars=ksprint(buffer,"Pid: %3d\t",task->pid);

	if(task->modo==KMODE)
		{
		cars+=ksprint(&(buffer[cars]),"KernelMode\n");

		cars+=ksprint(&(buffer[cars]),"eip: 0x%8x\tcs : 0x%4x\teflags: %8x\n",task->kernelRegs.eip,task->kernelRegs.cs,task->kernelRegs.eflags);

		cars+=ksprint(&(buffer[cars]),"eax: 0x%8x\tebx: 0x%8x\tecx: 0x%8x\tedx: 0x%8x\n",task->kernelRegs.eax,task->kernelRegs.ebx,task->kernelRegs.ecx,task->kernelRegs.edx);
		cars+=ksprint(&(buffer[cars]),"esp: 0x%8x\tebp: 0x%8x\tesi: 0x%8x\tedi: 0x%8x\n",task->kernelRegs.esp,task->kernelRegs.ebp,task->kernelRegs.esi,task->kernelRegs.edi);

		cars+=ksprint(&(buffer[cars]),"ds : 0x%4x\tes : 0x%4x\tfs : 0x%4x\tgs : 0x%4x\tss : 0x%4x\n",task->kernelRegs.ds,task->kernelRegs.es,task->kernelRegs.fs,task->kernelRegs.gs,task->kernelRegs.ss);
		}
	else
		{
		cars+=ksprint(&(buffer[cars]),"UserMode\n");

		cars+=ksprint(&(buffer[cars]),"eip: 0x%8x\tcs : 0x%4x\teflags: %8x\n",task->userRegs.eip,task->userRegs.cs,task->userRegs.eflags);

		cars+=ksprint(&(buffer[cars]),"eax: 0x%8x\tebx: 0x%8x\tecx: 0x%8x\tedx: 0x%8x\n",task->userRegs.eax,task->userRegs.ebx,task->userRegs.ecx,task->userRegs.edx);
		cars+=ksprint(&(buffer[cars]),"esp: 0x%8x\tebp: 0x%8x\tesi: 0x%8x\tedi: 0x%8x\n",task->userRegs.esp,task->userRegs.ebp,task->userRegs.esi,task->userRegs.edi);

		cars+=ksprint(&(buffer[cars]),"ds : 0x%4x\tes : 0x%4x\tfs : 0x%4x\tgs : 0x%4x\tss : 0x%4x\n",task->userRegs.ds,task->userRegs.es,task->userRegs.fs,task->userRegs.gs,task->userRegs.ss);
		}

	cars+=ksprint(&(buffer[cars]),"Code   : <0x%8x> 0x%8x\t",task->mem.base,task->mem.codesize);
	cars+=ksprint(&(buffer[cars]),"UStack : <0x%8x> 0x%8x\n",(task->mem.base+task->mem.codesize),task->mem.ustacksize);
	cars+=ksprint(&(buffer[cars]),"Data   : <0x%8x> 0x%8x\t",task->mem.base,(task->mem.codesize+task->mem.ustacksize));
	cars+=ksprint(&(buffer[cars]),"KStack : <0x%8x> 0x%8x\t",(task->mem.base+task->mem.codesize+task->mem.ustacksize),task->mem.kstacksize);

	return cars;
	}

bool	task_pusharg(struct task_t* task,char* arg)
	{
	int lung;

	if(task->modo!=UMODE)
		return FALSE;

	lung=kstrlen(arg)+1;
	task->userRegs.esp-=lung;//Metto anco il '\0'

	kstrcpy((char*)(task->mem.base+task->userRegs.esp+1),arg);

	task->userRegs.eax+=1;
	task->userRegs.ebx=task->userRegs.esp;

	return TRUE;
	}

