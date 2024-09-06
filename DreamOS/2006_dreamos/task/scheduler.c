/*
 * Dreamos
 * scheduler.c
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
   * Questo file contiene le funzioni per la schedulazione dei task
   */

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

pid_t request_pid() { return next_pid++;} //Assegna un nuovo pid al processo che ne fa richiesta.
pid_t get_current_task(){return cur_task;}
void add_task(task_t *task, int pid){array_task[pid] = *task;}
void preSchedule() {attiva=TRUE;}

void printlist()
{
//Semplice funzione per la stampa di una lista.
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
    //Seleziona il processo da eseguire per ora si tratta di un semplice Round Robin
    if(attiva==TRUE) {
        attiva=FALSE;

        array_task[cur_task].esp=*stack;
        cur_task=(cur_task+1)%MAX_TASK;
        if (cur_task==0)
                cur_task++;
                //ocho a questa funz qua sotto
                while (array_task[cur_task].stato!=READY) { // meglio con il diverso, solo i ready vanno fatti andare
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

void assign_pdir(pdbr task_cr3){
	array_task[cur_task].pdir = task_cr3;
}

unsigned int get_pdir(){
	return array_task[cur_task].pdir;
}
