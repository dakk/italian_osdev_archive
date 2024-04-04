
#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#define MAX_IN_SEM 10
typedef struct {
    int value;
    int sleeping_tasks[10];
} sem_t;

void sem_up(sem_t *);
void sem_down(sem_t *);
sem_t* sem_create();
void sem_delete();
void proc1();
void proc2();
void littledelay();
#endif
