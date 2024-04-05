/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: scheduler.cpp		|
	| Date: 16/09/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
	| Thanks to Silvio Abruzzo		|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <task/scheduler.h>

#include <hardware/cpu/irq.h>
#include <hardware/driver/pit.h>

#include <kservices/kout.h>

#include <stdlib.h>
#include <string.h>
#include <new>

#include <kdef.h>



/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace task;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////

TaskDescriptor Scheduler::TaskDescriptorTable[256];
int Scheduler::current_task;


void nullfunc(){
	while(1);
};

int tasksize = 0;

//Interrupt del timer
void pitisr(){
};

//La funzione del task idle
void idle(){
	while(1);
}

void Scheduler::Init(){

	//Creo idle
	CreateNewTask(idle, "idle");
	//Questa funzione verrà sostituita con il thread del kernel
	CreateNewTask(nullfunc, "Kernel Main Thread");
	//Faccio in modo che Kernel Main thread venga sostituito con il thrad principale
	current_task = 1;

	//Abilito il pit
	hardware::driver::PIT *schedpit = new hardware::driver::PIT();
	hardware::cpu::IRQ::Add(pitisr, 0);
	hardware::cpu::IRQ::EnableIRQ(0);
	//Frequenza
	schedpit->setFrequency(100);
	schedpit->Enable();
}

/****************************************
*** Inizio funzioni per creare i task ***
****************************************/

int Scheduler::getFreePID(){
	static int FreePID = -1;
	
	FreePID++;
	tasksize++;

	return FreePID;
}

TSS *Scheduler::CreateNewTSS(void (*func)()){
	//Alloco la memoria per lo stack
	TSS *newtss = (TSS *) ((dword) malloc(0x5000) + 0x5000 - 100);
	newtss->eip = (dword) func;
	newtss->cs = 8;
	newtss->eflags = 0x202;
	//TODO: Aggiungere fine
	//mytss->fine = (dword) end_func;
	
	//Restituisco il nuovo TSS
	return newtss;

}

int Scheduler::CreateNewTask(void (*taskfunc)(), char *taskname){
	asm("cli");

		//Cerco un PID libero
		int new_task_pid = getFreePID();

		//Setto il nome del processo controllando la dimensione della stringa
		strncpy(TaskDescriptorTable[new_task_pid].task_name, taskname, 64);

		//Preparo lo stack
		TaskDescriptorTable[new_task_pid].tss = (void *) CreateNewTSS(taskfunc);
		TaskDescriptorTable[new_task_pid].esp = (dword) TaskDescriptorTable[new_task_pid].tss;

		//Setto lo stato del processo a READY
		TaskDescriptorTable[new_task_pid].task_status = READY;

	asm("sti");

	return new_task_pid;
}

/****************************************
*** Fine funzioni per creare i task ***
****************************************/

int Scheduler::getNextTask(){
	current_task++;
	if (current_task == tasksize) current_task = 0; //TODO: Tornare a 0
	return current_task;
}

void Scheduler::Schedule(dword *esp){	
	//kservices::KOut::Write("S");
	//kservices::KOut::WriteLine((dword) current_task);
	TaskDescriptorTable[current_task].esp = *esp;

	current_task = getNextTask();

	*esp = TaskDescriptorTable[current_task].esp;
	//kservices::KOut::WriteLine((dword) current_task);
}

bool Scheduler::getTaskName(int pid, char * dest, int n){
	int i=0;

	if (pid >= tasksize) return false;

	while((TaskDescriptorTable[pid].task_name[i]!='\0') && (i<n)){
		dest[i++] = TaskDescriptorTable[pid].task_name[i];
	}

	dest[i] = '\0';

	return true;
}
