#include <irq.h>
#include <video.h>
#include <mem.h>
#include <cstring>
#include <task.h>
#include <cstdlib>
#include <politic_sched.h>
#include <hardware_task.h>
#include <debug.h>

namespace {
	const size_t max_task = 1000;
	task_descriptor tasks[max_task];
	pid_t current_task = 0;
	size_t num_task = 1;
	bool active = false;
};

volatile tick_t tick;

void scheduler(dword * stack)
{
	if (!active)
		return;		//se lo scheduler non è attivo
	pid_t one_task;
	tasks[current_task].esp = *stack;
	one_task = find_next_task();
//      debug << "\nè stato scelto il task " << one_task << endl;
	switch (tasks[one_task].stato) {
	case ESECUZIONE:
		current_task = one_task;
		*stack = tasks[current_task].esp;
		break;
	case ADDORMENTATO:
		if (tasks[one_task].dormo.tick) {
			if ((tasks[one_task].dormo.tick +
			     tasks[one_task].dormo.secondi) <= tick) {
				tasks[one_task].stato = ESECUZIONE;
				current_task = one_task;
				*stack = tasks[current_task].esp;
			} else {
				scheduler(stack);
			}
		} else {
			scheduler(stack);
		}
		break;
	case MORTO:
		del_tss(tasks[one_task].tss);
		libera_biglietti(tasks[one_task].biglietti);
		debug << "\n\nIl task " << one_task <<
			" è stato ucciso senza problemi\n\n";
		scheduler(stack);
		break;
	}
}

void attiva_scheduler()
{
	active = true;
}

void disattiva_scheduler()
{
	active = false;
}

void init_scheduler()
{
	active = true;
	current_task = 0;
	init_sched_politic();
	prendi_biglietti(0, 20);
	kout << "scheduler attivato!!!" << endl;
	return;
}

extern "C" pid_t add_task(void (*func) (), char *nome)
{
	asm("cli");
	//kout << endl <<  num_task << " task creato" << endl;
	tasks[num_task].tss = (void *) make_tss(func, suicide);
	tasks[num_task].esp = (dword) tasks[num_task].tss;
	//imposto iul nome del task
	strncpy(tasks[num_task].nome, nome, lunghezza_nome);
	//ne imposto lo stato
	tasks[num_task].stato = ESECUZIONE;
	tasks[num_task].process_id = num_task;
	tasks[num_task].biglietti =
		prendi_biglietti(tasks[num_task].process_id, 20);
	//tasks[num_task].dortick = 0;o
	//incremento il numero di task disponibili              
	++num_task;
	asm("sti");
	return num_task - 1;
}

pid_t get_current_task()
{
	return current_task;
}

void kill(pid_t id)
{
	tasks[id].stato = MORTO;
	while (1);
}

char *get_task_name(pid_t id, char *nome)
{
	strcpy(nome, tasks[id].nome);
	return nome;
}


void suicide()
{
	kill(get_current_task());
}

void sleep(size_t centesimi)
{
	asm("cli");
	tasks[get_current_task()].stato = ADDORMENTATO;
	tasks[get_current_task()].dormo.tick = tick;
	tasks[get_current_task()].dormo.secondi = centesimi;
	asm("sti");
	while (tick == tasks[get_current_task()].dormo.tick);
}

void addormenta(pid_t pid)
{
	asm("cli");
	tasks[pid].stato = ADDORMENTATO;
	tasks[pid].dormo.tick = 0;
	if (pid == get_current_task()) {
		dword mytick = tick;
		asm("sti");
		while (tick == mytick);
	} else {
		asm("sti");
	}
}

void sveglia(pid_t pid)
{
	tasks[pid].stato = ESECUZIONE;
}
