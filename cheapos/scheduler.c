
#include	"scheduler.h"

extern void asm_idle();

struct task_t 				task_lista[MAX_TASK_NUM];			//Lista dei task!!!
struct kwait_table_element_t	kwait_table[MAX_TASK_NUM];		//Lista delle Waitfunc
int						task_exitcode_lista[MAX_TASK_NUM];	//Lista degli exitcode!

struct pidcoda_t Qready,Qnotask,Qwait;

ISR_DECLARE(kwait_handler);
ISR_DECLARE(sched_exception00);
ISR_DECLARE(sched_exception04);
ISR_DECLARE(sched_exception05);
ISR_DECLARE(sched_exception06);
ISR_DECLARE(sched_exception11);
ISR_DECLARE(sched_exception12);
ISR_DECLARE(sched_exception13);

bool scheduler_init()
	{
	int i;
	struct task_mem_t idle_mem;

	time_addTJ(SCHEDULER_PERIOD,0,scheduler);
	kprint("Scheduler : ogni %d ticks\n",SCHEDULER_PERIOD);

	time_addTJ(WAITCHECK_PERIOD,0,kwait_check);
	kprint("\tWaitCheck ogni %d ticks\n",WAITCHECK_PERIOD);

	isr_setISR(KWAIT_INT,ISR_ADDRESS(kwait_handler));
	kprint("\tKWait handler int 0x%x\n",KWAIT_INT);

	isr_setISR(00,ISR_ADDRESS(sched_exception00));
	isr_setISR(04,ISR_ADDRESS(sched_exception04));
	isr_setISR(05,ISR_ADDRESS(sched_exception05));
	isr_setISR(06,ISR_ADDRESS(sched_exception06));
	isr_setISR(11,ISR_ADDRESS(sched_exception11));
	isr_setISR(12,ISR_ADDRESS(sched_exception12));
	isr_setISR(13,ISR_ADDRESS(sched_exception13));
	kprint("\tRidefiniti gestori delle eccezioni 0 4 5 6 11 12 13\n");

	//Caricare processo di IDLE in task_lista[0];
	idle_mem.base=mem_alloc(32);
	idle_mem.codesize=0;
	idle_mem.ustacksize=0;
	idle_mem.kstacksize=32;

	task_init(&task_lista[0],&idle_mem,0,(mem_t)asm_idle,KMODE,0x202);

	kprint("\tStack Idle  0x%8x <0x%8x>\n",idle_mem.base,idle_mem.kstacksize);

	pidcoda_init(&Qready);
	pidcoda_init(&Qnotask);
	pidcoda_init(&Qwait);

	for(i=1;i<MAX_TASK_NUM;i++)
		{
		task_lista[i].pid=-1;
		pidcoda_ins(&Qnotask,i);
		task_exitcode_lista[i]=0;
		}

	return TRUE;
	}

asm (
	".globl asm_idle	\n"
	"asm_idle:		\n"
	"hlt				\n"
	"jmp asm_idle		\n"
	);

void scheduler()
	{
	int pid;
	struct task_t task;

	task_getcorrente(&task);

	if (task.pid<0)		//Con IDLE anco 0 va bene!
		{
		kprint("SCH:Errore PID!");
		asm("hlt");
		}

	task_getcorrente(&task_lista[task.pid]);

	if(task.pid!=0)		//IDLE non rientra in coda!
		pidcoda_ins(&Qready,task.pid);


	pid=pidcoda_pop(&Qready);



	if (pid==0)
		{
		kprint("SCH:Errore PID!");
		asm("hlt");
		}

	if (pid==-1)
		pid=0;

	if (pid<0)
		{
		kprint("SCH:Errore PID!");
		asm("hlt");
		}

	task_setcorrente(&task_lista[pid]);

	return;
	}


void	kwait_check()
	{
	int i;
	int pid,lung;

	lung=pidcoda_elements(&Qwait);

	for(i=0;i<lung;i++)
		{
		pid=pidcoda_pop(&Qwait);

		if(kwait_table[pid].func(kwait_table[pid].param))
			pidcoda_ins(&Qready,pid);					//In Qready func(param) e' VERA!
		else
			pidcoda_ins(&Qwait,pid);					//In Qwait func(param) e' FALSA!
		}

	return;
	}


ISR_CODE_H(kwait_handler)
	{
	int pid;
	struct task_t task;

	task_getcorrente(&task);

	if (task.pid<=0)		//Con IDLE anco 0 va bene!
		{
		kprint("SCH:Errore PID!");
		asm("hlt");
		}

	if(task.modo!=KMODE)
		{
		kprint("SCH:Errore MODO!");
		asm("hlt");
		}
	task_getcorrente(&task_lista[task.pid]);

	pidcoda_ins(&Qwait,task.pid);

	//Registra kwait_event

	kwait_table[task.pid].func	  =(kwait_event_t)task_lista[task.pid].kernelRegs.ecx;
	kwait_table[task.pid].param=(unsigned long)task_lista[task.pid].kernelRegs.edx;


	//Parte un latro task!

	pid=pidcoda_pop(&Qready);

	if (pid==0)
		{
		kprint("SCH:Errore PID!");
		asm("hlt");
		}

	if (pid==-1)
		pid=0;

	if (pid<0)
		{
		kprint("SCH:Errore PID!");
		asm("hlt");
		}

	task_setcorrente(&task_lista[pid]);

	return;
	}

inline void kwait(kwait_event_t func,unsigned long param)
	{

	asm volatile(
		"sti				\n"
		"int	$ 0x42		\n"
		"cli				\n"
		:
		:"c"(func),"d"(param)
		);

	return;
	}


//	Ridefinico ISR delle eccezzioni
ISR_CODE_H(sched_exception00)
	{
	struct task_t task;

	task_getcorrente(&task);

	if (task.modo==UMODE)
		{
		kprint("Task %3d ha generato Exception 0  (Divide Error) e sara' terminato\n",task.pid);

		syscall_exit();
		//Qui non arriva mai!
		}

	kprint("SCHD:Exception! Divide Error");
	 while(1);
	 }

ISR_CODE_H(sched_exception04)
	{
	struct task_t task;

	task_getcorrente(&task);

	if (task.modo==UMODE)
		{
		kprint("Task %3d ha generato Exception 4 (OverFlow) e sara' terminato\n",task.pid);

		syscall_exit();
		//Qui non arriva mai!
		}

	kprint("SCHD:Exception! Overflow");
	 while(1);
	 }

ISR_CODE_H(sched_exception05)
	{
	struct task_t task;

	task_getcorrente(&task);

	if (task.modo==UMODE)
		{
		kprint("Task %3d ha generato Exception 5 (Bounds Check) e sara' terminato\n",task.pid);

		syscall_exit();
		//Qui non arriva mai!
		}

	kprint("SCHD:Exception! Bounds Check");
	 while(1);
	 }

ISR_CODE_H(sched_exception06)
	{
	struct task_t task;

	task_getcorrente(&task);

	if (task.modo==UMODE)
		{
		kprint("Task %3d ha generato Exception 6 (Invalid Opcod) e sara' terminato\n",task.pid);

		syscall_exit();
		//Qui non arriva mai!
		}
	kprint("SCHD:Exception! Invalid Opcode");
	 while(1);
	 }

ISR_CODE_H(sched_exception11)
	{
	struct task_t task;

	task_getcorrente(&task);

	if (task.modo==UMODE)
		{
		kprint("Task %3d ha generato Exception 11 (Segment Not Present) e sara' terminato\n",task.pid);

		syscall_exit();
		//Qui non arriva mai!
		}

	kprint("SCHD:Exception! Segment Not Present");
	 while(1);
	 }

ISR_CODE_H(sched_exception12)
	{
	struct task_t task;

	task_getcorrente(&task);

	if (task.modo==UMODE)
		{
		kprint("Task %3d ha generato Exception 12 (Stack Exception) e sara' terminato\n",task.pid);

		syscall_exit();
		//Qui non arriva mai!
		}

	kprint("SCHD:Exception!  Stack Exception");
	 while(1);
	 }

ISR_CODE_H(sched_exception13)
	{
	struct task_t task;

	task_getcorrente(&task);

	if (task.modo==UMODE)
		{
		kprint("Task %3d ha generato Exception 13 (General Protection Exception) e sara' terminato\n",task.pid);

		syscall_exit();
		//Qui non arriva mai!
		}

	kprint("SCHD:Exception! General Protection Exception(Triple Fault)");
	 while(1);
	 }
