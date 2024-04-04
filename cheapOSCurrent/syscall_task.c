
#include		"syscall_task.h"


extern struct task_t task_lista[MAX_TASK_NUM];
extern int task_exitcode_lista[MAX_TASK_NUM];
extern struct pidcoda_t	Qready,Qnotask,Qwait;


int syscall_exit(int exitcode)
	{
	int pid;
	struct task_t task;

	task_getcorrente(&task);

	mem_free((mem_t)task.mem.base);
	task_exitcode_lista[task.pid]=exitcode;

	//Devo chiudere file aperti se ce ne fossero!!!
	pidcloseall(task.pid);

	pidcoda_ins(&Qnotask,task.pid);

	//Forzo un task switch!
	pid=pidcoda_pop(&Qready);

	if (pid==0)
		asm("hlt");
	if (pid==-1)
		pid=0;
	if (pid<0)
		asm("hlt");

	task_setcorrente(&task_lista[pid]);

	//Forzo un content Switch

	asm(
		"movl	task_corrente_modo,%eax	\n"
		"cmp		$0,%eax					\n"
		"je		asm_ISRCkernelstart		\n"

		"movl	task_corrente_uebx,%ebx	\n"
		"movl	task_corrente_uecx,%ecx	\n"
		"movl	task_corrente_uedx,%edx	\n"

		"movl	task_corrente_uebp,%ebp	\n"
		"movl	task_corrente_uesp,%esp	\n"
		"movl	task_corrente_uesi,%esi		\n"
		"movl	task_corrente_uedi,%edi		\n"

		"movw	task_corrente_ugs,%gs		\n"
		"movw	task_corrente_ues,%es		\n"
		"movw	task_corrente_ufs,%fs		\n"
		"movw	task_corrente_uss,%ss		\n"

		"movl	task_corrente_ueflags,%eax	\n"
		"pushl	%eax					\n"
		"movl	task_corrente_ucs,%eax		\n"
		"pushl	%eax					\n"
		"movl	task_corrente_ueip,%eax		\n"
		"pushl	%eax					\n"

		"movl	task_corrente_ueax,%eax	\n"
		"movw	task_corrente_uds,%ds		\n"

		"iret								\n"


		"asm_ISRCkernelstart:				\n"

		"movl	task_corrente_kebx,%ebx	\n"
		"movl	task_corrente_kecx,%ecx	\n"
		"movl	task_corrente_kedx,%edx	\n"

		"movl	task_corrente_kebp,%ebp	\n"
		"movl	task_corrente_kesp,%esp	\n"
		"movl	task_corrente_kesi,%esi		\n"
		"movl	task_corrente_kedi,%edi		\n"

		"movw	task_corrente_kgs,%gs		\n"
		"movw	task_corrente_kes,%es		\n"
		"movw	task_corrente_kfs,%fs		\n"
		"movw	task_corrente_kss,%ss		\n"

		"movl	task_corrente_keflags,%eax	\n"
		"pushl	%eax					\n"
		"movl	task_corrente_kcs,%eax		\n"
		"pushl	%eax					\n"
		"movl	task_corrente_keip,%eax		\n"
		"pushl	%eax					\n"

		"movl	task_corrente_keax,%eax	\n"
		"movw	task_corrente_kds,%ds		\n"

		"iret								\n"
		);
	//Qui non arriva mai!!
	return pid;
	}


int syscall_fork()
	{
	int pidfiglio;
	struct task_t taskpadre;
	mem_t memfigliosize,memfigliobase;

	task_getcorrente(&taskpadre);

	if(pidcoda_elements(&Qnotask)<=0)
		{
		kprint("Fork Fallita PID\n");
		return -1;
		}

	pidfiglio=pidcoda_pop(&Qnotask);

	memfigliosize=taskpadre.mem.codesize+taskpadre.mem.ustacksize+taskpadre.mem.kstacksize;

	if(!(memfigliobase=mem_alloc(memfigliosize)))
		{
		pidcoda_ins(&Qnotask,pidfiglio);
		kprint("Fork Fallita MEM\n");
		return -1;
		}

	mem_copy(taskpadre.mem.base,memfigliobase,(memfigliosize-taskpadre.mem.kstacksize));

	task_lista[pidfiglio]=taskpadre;

	task_lista[pidfiglio].pid=pidfiglio;
	task_lista[pidfiglio].userRegs.ebx=0;
	task_lista[pidfiglio].mem.base=memfigliobase;
	task_lista[pidfiglio].modo=UMODE;
	task_lista[pidfiglio].userRegs.eflags=(task_lista[pidfiglio].userRegs.eflags | 0x202);//Non Serve!

	pidcoda_ins(&Qready,pidfiglio);

	//kprint("Fork!  New Task PID:%3d\n",pidfiglio);

	return pidfiglio;
	}


int syscall_getpid()
	{
	struct task_t task;

	task_getcorrente(&task);

	return task.pid;
	}

bool wtime(unsigned long tempo)
	{
	unsigned long tmp;

	tmp=time_tick();

	return (tmp>tempo);//Ritorna VERO se e' gia' passato il tempo 'tempo'
	}

unsigned long syscall_wait(unsigned long aspetta)
	{
	unsigned long inizio;

	inizio=time_tick();
	aspetta+=inizio;		//Riparto quando il tempo e' aspetta+inizio

	kwait(wtime,aspetta);

	return (time_tick()-inizio); //tempo effettivamente trascorso!!
	}

bool	wpid(unsigned long pid)
	{
	int i;

	for(i=(Qnotask.inizio+1)%MAX_TASK_NUM;i!=(Qnotask.fine+1)%MAX_TASK_NUM;i=(i+1)%MAX_TASK_NUM)
		if(Qnotask.dato[i]==pid) return TRUE;

	return FALSE;
	}

int syscall_waitpid(int pid)
	{
	struct task_t task;
	task_getcorrente(&task);

	//kprint("Aspetto pid %d\n",pid);

	if (pid>MAX_TASK_NUM || pid<1 || pid==task.pid)	//Controllo il pid da controllare....
		return -1;

	kwait(wpid,pid);

	return task_exitcode_lista[pid];
	}

int	syscall_exec(char* nomefile,int argc,char* argv0)
	{
	int i,pid,fd,size;
	char	buffer[24];
	char* argvOld;
	int caratteri;

	struct task_t padre;
	struct task_mem_t task_mem;

	task_getcorrente(&padre);

	fd=syscall_open(nomefile);
	if(fd<0) return -1;

	size=syscall_seek(fd,MFINE,0);	/*dimensione del file*/
	syscall_seek(fd,MINIZIO,0);

	size=size+(1024-(size%1024))+CODEPIUSIZE;		//Paddo a 1 kB+CODEPIUSIZE

	task_mem.base=mem_alloc(size+USTACKSIZE+KSTACKSIZE);

	if(task_mem.base==0)
		{
		syscall_close(fd);
		return -1;
		}

	task_mem.codesize=size;
	task_mem.ustacksize=USTACKSIZE;
	task_mem.kstacksize=KSTACKSIZE;

	caratteri=1;i=0;
	while(caratteri>0)
		{
		caratteri=syscall_read(fd,20,buffer);
		if(caratteri>0)
			{
			mem_copy((mem_t)buffer,(task_mem.base+i),caratteri);
			i+=caratteri;
			}
		}

	syscall_close(fd);

	if(pidcoda_elements(&Qnotask)<=0)
		{
		mem_free(task_mem.base);
		return -1;
		}

	pid=pidcoda_pop(&Qnotask);
	task_init(&task_lista[pid],&task_mem,pid,0,UMODE,0x202);

	argvOld=argv0;
	for(i=0;i<argc && i<TASK_MAXARGNUM;i++)
		{
		for(;(*argv0);argv0++);
		argv0++;
		}

	for(i=0;i<argc && i<TASK_MAXARGNUM;i++)
		{
		argv0-=2;
		for(;(*argv0 && argv0!=(argvOld-1));argv0--);
		argv0++;
		task_pusharg(&task_lista[pid],argv0);
		}

	pidcoda_ins(&Qready,pid);

	return pid;
	}

bool	syscall_kill(int pid)
	{
	int i;
	int lung,pidopt;
	bool killed;
	struct task_t task;

	task_getcorrente(&task);

	if(task.pid==pid || pid<2) return FALSE;	//Un Processo non puo' Killare Se Stesso od Idle o Startup sequence

	killed=FALSE;

	//Prima Cerco in Ready
	lung=pidcoda_elements(&Qready);

	for(i=0;i<lung && !killed;i++)
		{
		pidopt=pidcoda_pop(&Qready);

		if(pidopt==pid)
			{
			mem_free((mem_t)task_lista[pidopt].mem.base);
			task_exitcode_lista[pidopt]=-1;

			//Devo chiudere file aperti se ce ne fossero!!!
			pidcloseall(pidopt);

			pidcoda_ins(&Qnotask,pidopt);
			killed=TRUE;
			}
		else
			pidcoda_ins(&Qready,pidopt);
		}

	lung=pidcoda_elements(&Qwait);

	for(i=0;i<lung && !killed;i++)
		{
		pidopt=pidcoda_pop(&Qwait);

		if(pidopt==pid)
			{
			mem_free((mem_t)task_lista[pidopt].mem.base);
			task_exitcode_lista[pidopt]=-1;

			//Devo chiudere file aperti se ce ne fossero!!!
			pidcloseall(pidopt);

			pidcoda_ins(&Qnotask,pidopt);
			killed=TRUE;
			}
		else
			pidcoda_ins(&Qwait,pidopt);
		}

	return killed;
	}
