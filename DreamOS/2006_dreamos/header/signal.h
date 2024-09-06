/*
 * Dreamos
 * signal.h
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

#ifndef __SIGNAL_H
#define __SIGNAL_H

#include <mydef.h>
#include <task.h>

void sig_sleep(pid_t);
void sig_wakeup(pid_t);
void sig_kill(pid_t);
int _kipc_signal(dword pid, dword sig); //Send a signal to a task

enum {
	SIG_KILL=1,
	SIG_SLEEP,
	SIG_WAKEUP
};

#endif
