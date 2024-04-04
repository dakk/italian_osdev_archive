#include <scheduler.h>
#include <task.h>
#include <stdio.h>
#include <mydef.h>
#include <signal.h>

task_t array_task[MAX_TASK];
pid_t cur_task;
pid_t next_pid;

int attiva;

void init_scheduler()
{
    int i=0;
    next_pid=0;
    cur_task=0;
    attiva=FALSE;
    while(i<MAX_TASK) {
        array_task[i].stato = DEAD;
        i++;
    }
}

pid_t request_pid() { return next_pid++;}
pid_t get_current_task(){return cur_task;}
void add_task(task_t *task, int pid){array_task[pid] = *task;}
void preSchedule() {attiva=TRUE;}

void printlist()
{
    int i;
    i=0;
    printf("%d\n", next_pid);
    printf("PID  PROCESSO\n");
    while(i<next_pid+5) {
        printf("%d  %s   %d\n", array_task[i].PID, array_task[i].name, array_task[i].stato);
        i++;
    }

}

void schedule(dword *stack)
{
    if(attiva==TRUE) {
        attiva=FALSE;

        array_task[cur_task].esp=*stack;
        cur_task=(cur_task+1)%MAX_TASK;
        if (cur_task==0)
                cur_task++;
                while (array_task[cur_task].stato!=READY) {
                        cur_task = (cur_task+1)%MAX_TASK;
                if (cur_task==0)
                   cur_task++;
                }
        *stack = array_task[cur_task].esp;
    }
}

void suicide()
{
    sig_kill(get_current_task());
    while(TRUE);
}

void idle_task(){
    while(TRUE);
}
