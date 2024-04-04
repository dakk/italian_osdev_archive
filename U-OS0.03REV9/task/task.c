#include <task.h>
#include <mydef.h>
#include <stdlib.h>
#include <stdio.h>
#include <video.h>
#include <scheduler.h>
#include <tss.h>

pid_t next_pid;
task_t* pcur_task;


void new_task(void (*func)(), char *nome)
{
    asm("cli");
    task_t processo;
    processo.name=nome;
    processo.eip = (dword)func;
    processo.esp =(dword) malloc(STACK_SIZE)+ STACK_SIZE-100;
    processo.regs = (tss_t *)processo.esp;
    new_tss(processo.regs, func, suicide);
    processo.PID = next_pid++;
    processo.stato = READY;
    printf("Created new task '%s' PID: %d\n", nome, processo.PID);
    pcur_task = &processo;
    add_task(pcur_task, processo.PID);
    asm("sti");
}

void provatask_pointer()
{
    task_t *task_pointer;
    task_pointer = malloc(sizeof(task_t));
    task_pointer->name="Prova Task_pointer";
    printf("%s\n", task_pointer->name);
}

void task_debug()
{
    _kputs("\nThis is a DebugTask\n");
    new_task(task_debug2, "Debug");
//    while(1);
}

void task_debug2()
{
printf("task_debug2\n");
//while(1);
}

void switch_task(task_t *task_cur)
{
    asm(
           "mov %0, %%esp\n\t"
           "popa\n\t"
           "sti\n\t"
           "iret\n\t"
           :
           : "a"(task_cur->esp));
}

