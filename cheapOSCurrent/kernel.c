
#include "kernel.h"

extern void startupsequence();						//Sequenza di startup!

extern struct task_t task_lista[MAX_TASK_NUM];		//Lista dei task!!!
extern struct pidcoda_t Qready,Qnotask,Qwait;
extern int consoleOnScreen;
/*
//Prova-------------------------------------------------------INIZIO
extern int task_corrente_pid;
extern bool fat12_montato;
extern struct cache_t floppy_cache;
//Prova-------------------------------------------------------FINE
*/
void _start2()
	{
	asm ("pushl $2; popf");			//Imposto FLAG

	asm volatile
		(
      		"lgdtl (loadgdt)			\n"

		"movw $0x10,%%ax		\n"	//Data Desc 0x10
		"movw %%ax,%%ds		\n"
		"movw %%ax,%%es		\n"
		"movw %%ax,%%fs		\n"
		"movw %%ax,%%gs		\n"

		"movw $0x18,%%ax		\n"	//Stack Desc 0x18
		"movw %%ax,%%ss		\n"

		"movl   %0,%%esp	\n"

		"ljmp $0x08,$next		\n"	//Salta per aggiornare CS !!!(Code Desc)
		"nop					\n"
		"nop					\n"
		"next:				\n"
		:
		: "p"((unsigned long)KSTACK_SIZE+KSTACK_BASE)		//Perche'?????
		: "%eax"
		);


	//Faccio partire int main()
	main();

	//Qui non arriva Mai!!!
	asm("cli");
	asm("hlt");
	}


//---MAIN-----------------------------------------------------------------------------------------------------------------

int main()
	{
	int pid;
	struct task_mem_t uno_mem;

	//Inizializza i device base
	kconsole_init();
	kprint("\ncheapOS!\n\n");

	cpu_init();
	mem_init();

	//Altre periferiche-------------------INIZIO

	Kbd_init();
	Video_init();
	floppy_init();

	//time_addTJ(2,0,orologio);
	//kprint("Orologio.......\n");

	Kbd_add_shortcut(0x0679,goshutdown);		//CTRL+ALT+CANC
	Kbd_add_shortcut(0x0062,mem_defrag);	//F11
	Kbd_add_shortcut(0x0061,fat12_goumount);	//F10
	Kbd_add_shortcut(0x0060,fat12_gomount);	//F09

	//Kbd_add_shortcut(0x005F,filesystem_tabella_dumpscreen);

	//Altre periferiche-------------------FINE

	filesystem_init();
	syscall_init();
	scheduler_init();

	//Carico il primo programma(KMODE)

	uno_mem.base=mem_alloc(40*1024);
	uno_mem.codesize=0;
	uno_mem.ustacksize=0;
	uno_mem.kstacksize=40*1024;

	pid=pidcoda_pop(&Qnotask);
	task_init(&task_lista[pid],&uno_mem,pid,(mem_t)startupsequence,KMODE,0x202);
	pidcoda_ins(&Qready,pid);

	kprint("\tStack Startup 0x%8x <0x%8x>\n",uno_mem.base,uno_mem.kstacksize);

	//Faccio partire il primo programma!

	pid=pidcoda_pop(&Qready);

	if (pid==0)
		asm("hlt");
	if (pid==-1)
		pid=0;
	if (pid<0)
		asm("hlt");

	task_setcorrente(&task_lista[pid]);

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
	return 0;
	}

//Sequenza di Startup----------------------------------INIZIO
void startupsequence()
	{
	char c;
	int pid,fd;


	//Prova-----------------------------------------inizio
	kprint("Premi un CTRL+ALT+F1 e poi un Tasto\n");
	fd=syscall_open("CON:\\Console1");
	syscall_read(fd,1,&c);
	syscall_close(fd);
	//Prova------------------------------------------fine

	pid=1;
	while(1)
		{
		while(pid<2)
			{
			fat12_mount();
			kprint("Carico Task di Avvio...");
			pid=syscall_exec("FAT:\\ASHELL.BIN",0,0);

			if (pid>=2) kprint("Pid:%3d\n",pid);
			}
		syscall_waitpid(pid);


		fat12_umount();
		kprint("Inserisci il Disco con Ashell.bin e premi un Tasto\n");

		fd=syscall_open("CON:\\Console1");
		syscall_read(fd,1,&c);
		syscall_close(fd);

		pid=1;
		}

	//Qui non Arriva Mai!!!
	return;
	}


