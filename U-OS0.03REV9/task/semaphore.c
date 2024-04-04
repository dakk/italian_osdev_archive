#include <semaphore.h>
#include <scheduler.h>
#include <signal.h>
#include <stdlib.h>

sem_t *sem_create() {
	sem_t *sem;
	int i;
	sem=(sem_t *)malloc(sizeof(sem_t));
	sem->value=1;
	for (i=0; i<MAX_IN_SEM; i++)
		sem->sleeping_tasks[i]=-1;
	return sem;
}

void sem_delete(sem_t *sem) {
	int i;
	for (i=0; i<MAX_IN_SEM; i++) {
		if (sem->sleeping_tasks[i]>-1)
			sig_wakeup(sem->sleeping_tasks[i]);
	}
	free(sem);
}

void sem_down(sem_t *sem) {
	int i;
	asm("cli");
	if (sem->value>0) {
		sem->value--;
	} else {
		for (i=0; i<MAX_IN_SEM; i++) {
			if (sem->sleeping_tasks[i]==-1) {
				sem->sleeping_tasks[i]=get_current_task();
				sig_sleep(get_current_task());
                                asm("cli");
				sem_down(sem);
				asm("sti");
				return;
			}
		}
	}
	asm("sti");
	return;
}

void sem_up(sem_t *sem) {
	int i;
	asm("cli");
	if (sem->value==0) {
		for (i=0; i<MAX_IN_SEM; i++) {
			if (sem->sleeping_tasks[i]>-1) {
				sig_wakeup(sem->sleeping_tasks[i]);
				break;
			}
		}
		sem->value++;
	} else {
		sem->value++;
	}
	asm("sti");
}
