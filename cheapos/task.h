
#ifndef	_TASK_
#define	_TASK_

#include	"config.h"

#include	"mem.h"
#include	"descrittori.h"
#include	"kstring.h"

#define	KMODE	0
#define	UMODE	1

#define	KERNEL_CODE		0x08
#define	KERNEL_DATA		0x10
#define	KERNEL_MAINSTACK	0x18

#define	USER_CODE			0x20		//
#define	USER_DATA			0x28		//	DPL3
#define	USER_STACK			0x30		//

#define	KERNEL_TASKSTACK	0x38
#define	KERNEL_TSS			0x40

//Stato del Processore
struct	task_regs_t
	{
	unsigned int eip,cs,eflags;		//
	unsigned int eax,ebx,ecx,edx;		//Presi dallo stato del processore
	unsigned int esp,ebp,esi,edi;		//
	unsigned int ds,ss,es,fs,gs;		//
	};

//Segmento di memoria del Task
struct	task_mem_t
	{
	mem_t base;							//	|		CODE		|	USTACK  |	KSTACK|
	mem_t codesize,ustacksize,kstacksize;		//	|				DATA			|
	};

//Struttura che identifica un Task
struct	task_t
	{
	signed int pid;					//
	struct task_mem_t	mem;		//	Questi vengono fissati all'inizio e non cambiano!

	unsigned int modo;				//
	struct task_regs_t	userRegs;	//	Questi cambiano durante l'esecuzione
	struct task_regs_t	kernelRegs;	//
	};

//TSS----Per il Cambio di Privilegio--------------------------------------
struct TSS
	{
	unsigned int	backlink;
	unsigned int	esp0;
	unsigned int	ss0;
	unsigned int	esp1;
	unsigned int	ss1;
	unsigned int	esp2;
	unsigned int	ss2;
	unsigned int	cr3;
	unsigned int	eip;
	unsigned int	eflags;
	unsigned int	eax;
	unsigned int	ecx;
	unsigned int	edx;
	unsigned int	ebx;
	unsigned int	esp;
	unsigned int	ebp;
	unsigned int	esi;
	unsigned int	edi;
	unsigned int	es;
	unsigned int	cs;
	unsigned int	ss;
	unsigned int	ds;
	unsigned int	fs;
	unsigned int	gs;
	unsigned int	ldtr;
	unsigned int	altriflag;
	unsigned int	iomapbase;
	} __attribute__ ((packed));

//Carica la TSS
bool	task_tss_init();

//Inizzializza una struttura 'task' (mem fa riferimento al segmento dove e' allocato il task)
bool	task_init(struct task_t* task,struct task_mem_t* mem,int pid,unsigned int eip,int modo,unsigned int eflags);

//Salva e carica il processo corrente in 'task'
void task_setcorrente(struct task_t* task);
void task_getcorrente(struct task_t* task);

//Stampa a video 'task' e il processo corrente
void	task_dumpscreen(const struct task_t* task);
void	task_dumpscreen_current();
int	task_dump(char* buffer,const struct task_t* task);

//Passa un argomento al Task (usare dopo task_init();
bool	task_pusharg(struct task_t* task,char* arg);

#endif
