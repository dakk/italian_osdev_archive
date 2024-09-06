
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

void sem_delete(sem_t *sem) { // è tutto temporaneo qua dentro
	int i;
	for (i=0; i<MAX_IN_SEM; i++) { //Se non sono alla fine del semaforo
		if (sem->sleeping_tasks[i]>-1)   //Se il valore dell'elemento del vettore dei processi sleeping e' >-1 (quindi ha un task)
			sig_wakeup(sem->sleeping_tasks[i]);  //Sveglia il task
	}
	free(sem);  //Libera il semaforo
}

void sem_down(sem_t *sem) {
	int i;
	asm("cli"); //Disabilito gli interrupt
	if (sem->value>0) { //Se il valore e' >0 posso accedere alla risorsa
		sem->value--; //Decremento il valore
	} else { //Altrimenti addormento il processo
		for (i=0; i<MAX_IN_SEM; i++) {
			if (sem->sleeping_tasks[i]==-1) {//Se l'elemento corretne e' vuoto
				sem->sleeping_tasks[i]=get_current_task(); //Ci inserisco il pid del semaforo da addormentare
				sig_sleep(get_current_task()); //Addormento processo
                                asm("cli");
				sem_down(sem);
				asm("sti"); //Riabilito gli interrupt
				return;
			}
		}
		// non ci sono slot disponibili per memorizzare il task che deve sleeppare
	}
	asm("sti");
	return;
}

// questa funz risveglia solo un task, in questo modo il task prende da solo il controllo della zona critica e quando
// la libererà toccherà al task successivo
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
