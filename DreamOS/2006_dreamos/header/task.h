/*
 * Dreamos
 * task.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

 /*
  * Autore Ivan Gualandri
  * Prima versione: 28/02/2004
  * Thanks to: Mius
  */

  /*
   * Contiene la struttura dati e la definizione dei tipi per la gestione dei task
   */

#ifndef __TASK__H
#define __TASK__H

#include <mydef.h>
#include <paging.h>

#define STACK_SIZE 0x1000 //Dimensione dello stack
enum state{
    DEAD,
    READY,
    SLEEPING
};

typedef enum state state;

struct task_t {
        char *name;                 //Nome del processo
        unsigned short PID;      //Identificativo del processo
        long int esp;         //Registro esp del processore -- o forse ebp?
        dword eip;         //Indirizzo funzione principale del processo.
        enum state stato;                 //Stato del processo
        struct registri *regs;
        unsigned int pdir;/*Base della pageDir*/
        void (*func)(); //Function to fork
        int forked; //Is it already forked?
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
typedef unsigned short int pid_t;        //Definisco i tipi task_t e pid_t

int new_task(void (*func)(), char *);   //Create a new task
void load_task(task_t*);                      //Load a Task
void task_debug();
void provatask_pointer();
void task_debug2();
void kill(pid_t);
int fork();
int get_task_status(int) ;
void switch_task(task_t*);
#endif
