#ifndef _TASK_H_
#define _TASK_H_


#include <irq.h>
#include <cstddef>


#include <politic_sched.h>

enum state_t{
    ESECUZIONE,     //può essere eseguito
	ADDORMENTATO,   //è stato messo in sleep dall'utente
	MORTO   //è in lista per essere eliminato
};

namespace{
	const dword lunghezza_nome=50;
};

struct sleep_t{
	tick_t tick;
	size_t secondi;
};

struct task_descriptor{
	dword esp;
	void *tss;
	char nome[lunghezza_nome];
	pid_t process_id;
	state_t stato;
	biglietto_t biglietti;
	sleep_t dormo;
};

extern "C"{
	#include <fun_task.h>
};

#endif
