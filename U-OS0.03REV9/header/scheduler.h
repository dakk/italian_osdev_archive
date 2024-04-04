#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <mydef.h>
#include <task.h>

#define MAX_TASK 100

extern pid_t next_pid;
extern pid_t cur_task;
extern task_t array_task[MAX_TASK];

void init_scheduler();
void printlist();
void add_task(task_t *task, int);

void schedule(dword *);
void preSchedule();

void idle_task();
void suicide();

pid_t request_pid();
pid_t get_current_task();
#endif
