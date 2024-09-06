/*
 * Dreamos
 * scheduler.h
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
#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <mydef.h>
#include <task.h>
/**
 * Variabili
 */

#define MAX_TASK 100

extern pid_t next_pid;
extern pid_t cur_task;
extern task_t array_task[MAX_TASK];

/**
 * Prototipi di funzione
 */
void init_scheduler();          //Prepara l'ambiente per eseguire lo scheduler.
void printlist();                   //Stampa la coda dei processi
void add_task(task_t *task, int);

void schedule(dword *);                //Scheduler
void preSchedule();

void idle_task();
void suicide();

void assign_pdir(unsigned int);
unsigned int get_pdir();

pid_t request_pid();          //Get the pid of a Task
pid_t get_current_task();
#endif
