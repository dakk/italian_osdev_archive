/*
 * Dreamos
 * task.c
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
  * Prima versione: 29/02/2004
  * Thanks to: Mius
  */

  /*
   * Questo file contiene le funzioni per gestire e creare i task
   */

#include <task.h>
#include <mydef.h>
#include <stdlib.h>
#include <stdio.h>
#include <video.h>
#include <scheduler.h>
#include <tss.h>

pid_t next_pid;
task_t* pcur_task;


int new_task(void (*func)(), char *nome)
{
    /*
     * Questa funzione si occupa di creare nuovi processi.
     */
    asm("cli");
    task_t processo;
    processo.name=nome;
    processo.eip = (dword)func;  //Carico il campo eip con l'indirizzo della funzione main. <-- si puo eliminare
    processo.esp =(dword) malloc(STACK_SIZE)+ STACK_SIZE-100;  //Carico lo Stack
    if(processo.esp == NULL) {
        printf("Impossibile Creare Processo");
        processo.name = NULL;
        processo.esp = NULL;
    }
    else {
        processo.regs = (tss_t *)processo.esp;
        new_tss(processo.regs, func, suicide);
        processo.PID = next_pid++;   //Prendo il Pid
        processo.stato = READY; //Stato del processo
        processo.func = func; //Save the function to fork
        processo.forked = 0; //Let it fork if it's not already forked
        printf("Creato nuovo task '%s' con PID: %d\n", nome, processo.PID);
        pcur_task = &processo;
        add_task(pcur_task, processo.PID);
        //create_pagedir(processo.PID);
    }
        asm("sti");
    return processo.PID;
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
    _kputs("\nCiao Sono un task di debug\n Tu immagino sei un utente\n\033\005Lo sai sto crescendo\033\007\n");
    new_task(task_debug2, "Provino");
//    while(1);
}

void task_debug2()
{
printf("mah che dire provo se giro\n");
//while(1);
}

int get_task_status(int pid) {
        return array_task[pid].stato;
}

void switch_task(task_t *task_cur)
{
/*Si puo rimuovere? e' inutilizzata. ^Inuyasha^*/
    asm(
           "mov %0, %%esp\n\t" //Assegno il nuovo stack
           "popa\n\t" //Carico tutti i General purpose
           "sti\n\t"
           "iret\n\t" //Cambio processo uscendo dall'interrupt
           :
           : "a"(task_cur->esp));
}

int fork()
{
	/* This function will create a new task of the same function
	 * which calls fork() and returns current PID (parent) of the task if it's forked
	 * or 0 if it's already forked (child) */

	task_t *ctask = &array_task[cur_task]; //Current task
  if (!array_task[ctask->PID-1].forked) { //The previous task didn't fork
		ctask->forked = 1; //Now i fork to make a child
		new_task(ctask->func, ctask->name); //Make a new task
		return ctask->PID; //Return the current PID
	}
	array_task[ctask->PID-1].forked = 0; //I'm the child and now the previous task can fork a new child
	return 0; //I'm the child
}
