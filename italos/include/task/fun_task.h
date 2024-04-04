#ifndef _FUNC_TASK_H_
#define _FUNC_TASK_H_

#include <stddef.h>

void scheduler(unsigned int *stack);

pid_t add_task(void (*func)(), char *nome);

void init_scheduler();

pid_t get_current_task();

void kill(pid_t id);

void suicide();


void attiva_scheduler();

void disattiva_scheduler();

void sleep(size_t cent_sec);

void addormenta(pid_t pid);

void sveglia(pid_t pid);
 
char* get_task_name(pid_t id, char* nome);

#endif
