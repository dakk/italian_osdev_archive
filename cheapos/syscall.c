
#include	"syscall.h"

ISR_DECLARE(syscall_handler);


bool syscall_init()
	{
	isr_setISR(SYSCALLINT,ISR_ADDRESS(syscall_handler));

	kprint("Syscall : handler int 0x%x\n",SYSCALLINT);

	return TRUE;
	}


ISR_CODE_S(syscall_handler)
	{
	struct task_t task;

	task_getcorrente(&task);

	switch(task.userRegs.eax)
		{

					//SYSTEM syscalls
		case 0x0000 :
			//kprint("SYSTEM:Shutdown!\n");
			*((bool*)&(task.userRegs.ebx))=syscall_shutdown();
		break;


					//TASK syscalls
		case 0x0100 :
			//kprint("TASK:Exit\n");
			syscall_exit(task.userRegs.ecx);
		break;

		case 0x0101 :
			//kprint("TASK:Fork\n");
			*((signed int*)&(task.userRegs.ebx))=syscall_fork();
		break;

		case 0x0102 :
			//kprint("TASK:GetPid\n");
			*((signed int*)&(task.userRegs.ebx))=syscall_getpid();
		break;

		case 0x0103 :
			//kprint("TASK:Wait\n");
			task.userRegs.ebx=syscall_wait(task.userRegs.ecx);
		break;

		case 0x0104 :
			//kprint("TASK:WaitPid\n");
			*((signed int*)&(task.userRegs.ebx))=syscall_waitpid((signed int)task.userRegs.ecx);
		break;

		case 0x0105 :
			//kprint("TASK:Exec\n");
			*((signed int*)&(task.userRegs.ebx))=syscall_exec((char*)(task.userRegs.ecx+task.mem.base),task.userRegs.ebx,(char*)(task.userRegs.edx+task.mem.base));
		break;

		case 0x0106 :
			//kprint("TASK:Kill\n");
			*((bool*)&(task.userRegs.ebx))=syscall_kill((signed int)task.userRegs.ecx);
		break;

					//FS syscalls
		case 0x0200 :
			//kprint("FS:Open\n");
			*((signed int*)&(task.userRegs.ebx))=syscall_open((char*)(task.userRegs.ecx+task.mem.base));

		break;

		case 0x0201 :
			//kprint("FS:Close\n");
			*((signed int*)&(task.userRegs.ebx))=syscall_close(task.userRegs.ecx);
		break;

		case 0x0202 :
			//kprint("FS:Seek\n");
			*((signed int*)&(task.userRegs.ebx))=syscall_seek(task.userRegs.ecx,task.userRegs.edx,task.userRegs.ebx);
		break;

		case 0x0203 :
			//kprint("FS:Read\n");
			task.userRegs.ebx=syscall_read(task.userRegs.ecx,task.userRegs.ebx,(char*)(task.userRegs.edx+task.mem.base));
		break;

		case 0x0204 :
			//kprint("FS:Write\n");
			task.userRegs.ebx=syscall_write(task.userRegs.ecx,task.userRegs.ebx,(char*)(task.userRegs.edx+task.mem.base));
		break;

		case 0x0205 :
			//kprint("FS:Touch\n");
			task.userRegs.ebx=syscall_touch((char*)(task.userRegs.ecx+task.mem.base));
		break;

		case 0x0206 :
			//kprint("FS:Remv");
			task.userRegs.ebx=syscall_remv((char*)(task.userRegs.ecx+task.mem.base));
		break;

		default:
			kprint("Task %d chiama Syscall Indefinita (0x%x)\n",task.pid,task.userRegs.eax);
		}

	task_setcorrente(&task);
	}

