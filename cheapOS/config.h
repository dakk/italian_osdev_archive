
#ifndef	_CONFIG_
#define	_CONFIG_

#define	TRUE	1
#define	FALSE	0

/*CONFIG-DEL_KERNEL-------------------------------------------------------------------------INIZIO*/
#define	KERNEL_START	(1024*1024)//0x7E00

#define	KSTACK_SIZE		0x7E00-0x500
#define	KSTACK_BASE	0x500

#define	DMABUFFER_BASE	0x7E00
#define	DMABUFFER_SIZE		512

#define	MEM_TB_BASE		(0x7E00+512)
#define	MEM_TB_SIZE		(20*1024)	//20k di tabella di memoria
#define	MEM_TB_RAZIO		2
#define	MEM_RESERVED1		(0x7E00+512+24*1024)	//Riservato da 0		Reserv+KMAINSTACK+DMABUFFER+MEMTABLES
#define	MEM_RESERVED2		((384+300)*1024)		//Riservato da 640k	BIOS+KERNELCODE


#define	MAX_TASK_NUM	128
#define	USTACKSIZE		(30*1024)
#define	KSTACKSIZE		(30*1024)
#define	CODEPIUSIZE		(8*1024)
/*CONFIG-DEL_KERNEL-------------------------------------------------------------------------FINE*/

#define	TASK_MAXARGNUM	5	//Numero massimo di argomenti da passare al Task

typedef	int	bool;

#endif
