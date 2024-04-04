#ifndef __TASK__H
#define __TASK__H

#include <mydef.h>

#define STACK_SIZE 0x1000
enum state{
    DEAD,
    READY,
    SLEEPING
};

typedef enum state state;

struct task_t {
        char *name;
        unsigned short PID;
        long int esp;
        dword eip;        
        enum state stato;
        struct registri *regs;
    };

struct registri {
        dword edi;   //+0
        dword esi;   //+1
        dword ebp;  //+2
        dword null;  //+3
        dword ebx;  //+4
        dword edx;  //+5
        dword ecx;  //+6
        dword eax;  //+7
        dword eip;  //+8
        dword cs;   //+9
        dword eflags;  //+10
        dword fine;
    };

typedef struct task_t task_t;
typedef unsigned short int pid_t;

void new_task(void (*func)(), char *);
void load_task(task_t*);
void task_debug();
void provatask_pointer();
void task_debug2();
void kill(pid_t);
void switch_task(task_t*);
#endif
