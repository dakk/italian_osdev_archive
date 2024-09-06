/*
 * Dreamos
 * signal.c
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
  * Prima versione: 21/04/2004
  */

#include <signal.h>
#include <scheduler.h>
#include <task.h>
#include <8253.h>


void sig_sleep(pid_t processo)
{
//Asleep a process
    asm("cli");
    array_task[processo].stato = SLEEPING;
    preSchedule();
    maxQuants();
    asm("sti");
    while(array_task[processo].stato == SLEEPING);
}

void sig_wakeup(pid_t processo)
{
//WakeUp a process.
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
		array_task[pidnumber].func = NULL;
}

int _kipc_signal(dword pid, dword sig) {
  /* Send a signal to a specified task
   * Signals are defined in signal.h and functions in ipc.c */
  
  if (array_task[pid].stato == DEAD) return -1;
  switch(sig) {
  case SIG_KILL:
    sig_kill(pid);
    return 1;
  case SIG_SLEEP:
    sig_sleep(pid);
    return 1;
  case SIG_WAKEUP:
    sig_wakeup(pid);
    return 1;
  }
  return -1;
}
