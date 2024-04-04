#include <signal.h>
#include <scheduler.h>
#include <task.h>
#include <8253.h>


void sig_sleep(pid_t processo)
{
    asm("cli");
    array_task[processo].stato = SLEEPING;
    preSchedule();
    maxQuants();
    asm("sti");
    while(array_task[processo].stato == SLEEPING);
}

void sig_wakeup(pid_t processo)
{
    array_task[processo].stato = READY;
    preSchedule();
}

void sig_kill(pid_t pidnumber)
{
    array_task[pidnumber].stato = DEAD;
    array_task[pidnumber].name = NULL;
    array_task[pidnumber].esp = NULL;
    array_task[pidnumber].eip = NULL;
    array_task[pidnumber].PID = 0;
    array_task[pidnumber].regs = NULL;
}
