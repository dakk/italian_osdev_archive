/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: scheduler.h			|
	| Date: 27/06/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_DRIVER_SCHEDULER_H_
#define _HARDWARE_DRIVER_SCHEDULER_H_


/*
	**************************
	**	 Include	**
	**************************
*/

#include <kdef.h>

namespace task{

	enum TaskStatus{
		READY,
		WAITING,
		SLEEPING,
		RUNNING,
		TERMINATED
	};

	struct TSS{
		dword edi;
		dword esi;
		dword ebp;
		dword null;
		dword ecx;
		dword edx;
		dword ebx;
		dword eax;
		dword eip;
		dword cs;
		dword eflags;
		dword fine;
	};

	struct TaskDescriptor{
		char task_name[64];
		TaskStatus task_status;
		dword esp;
		void *tss; 
	};

	class Scheduler{
	public:
		static void Init();

		static void Schedule(dword *esp);
		static int CreateNewTask(void (*taskfunc)(), char *Taskname);
		static bool getTaskName(int pid, char * dest, int n);
	private:
		static int current_task;
		static TaskDescriptor TaskDescriptorTable[256];
		static int getNextTask();
		static int getFreePID();
		static TSS *CreateNewTSS(void (*func)());
	};	

}

#endif

