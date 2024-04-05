
#include	"syscall_system.h"

//pid del Processo che invoca lo shutdown
int pidkiller=-1;
extern struct task_t task_lista[MAX_TASK_NUM];		//Lista dei task!!!
extern struct pidcoda_t Qnotask,Qready;

bool syscall_shutdown(unsigned long tickslast)
	{
	struct task_mem_t uno_mem;

	if(pidkiller<0)
		{
		if(pidcoda_elements(&Qnotask)<=0) return FALSE;

		pidkiller=pidcoda_pop(&Qnotask);

		uno_mem.base=mem_alloc(40*1024);
		if(uno_mem.base==0) return FALSE;

		uno_mem.codesize=0;
		uno_mem.ustacksize=0;
		uno_mem.kstacksize=40*1024;

		task_init(&task_lista[pidkiller],&uno_mem,pidkiller,(mem_t)shutdownproc,KMODE,0x002);
		pidcoda_ins(&Qready,pidkiller);
		return TRUE;
		}
	else
		if(syscall_kill(pidkiller))
			{
			pidkiller=-1;
			kprint("Shutdown Interrupted\n");
			return TRUE;
			}
	return FALSE;
	}

void	goshutdown()
	{
	struct task_mem_t uno_mem;

	if(pidkiller<0)
		{
		if(pidcoda_elements(&Qnotask)<=0) return;

		pidkiller=pidcoda_pop(&Qnotask);

		uno_mem.base=mem_alloc(40*1024);
		if(uno_mem.base==0) return;

		uno_mem.codesize=0;
		uno_mem.ustacksize=0;
		uno_mem.kstacksize=40*1024;

		task_init(&task_lista[pidkiller],&uno_mem,pidkiller,(mem_t)shutdownproc,KMODE,0x002);
		pidcoda_ins(&Qready,pidkiller);
		}
	else
		if(syscall_kill(pidkiller))
			{
			pidkiller=-1;
			kprint("Shutdown Interrupted\n");
			}

	return;
	}

//Processo diShutdown
void shutdownproc()
	{
	kprint("Shutdown in %d Ticks!\n",TICKSLASTTOSHUTDOWN);
	syscall_wait((TICKSLASTTOSHUTDOWN/3)*2);
	kprint("Shutdown in %d Ticks!\n",(TICKSLASTTOSHUTDOWN/3));
	syscall_wait((TICKSLASTTOSHUTDOWN/3));
	kprint("Shutdown! _ORA_");

	asm("cli");

	while(inportb(0x64) & 0x02);		//Rebotto
	outportb(0x64, 0xfe);

	asm("hlt");
	//Qui non arriva MAI!
	}


