#ifndef _POLITIC_SCHEDULER_H_
#define _POLITIC_SCHEDULER_H_


struct biglietto_t{
	size_t inizio;
	size_t num;
	pid_t proprietario;
};

void init_sched_politic();

biglietto_t prendi_biglietti(pid_t pid, size_t num);

void libera_biglietti(biglietto_t ticket);

pid_t find_next_task();


#endif
