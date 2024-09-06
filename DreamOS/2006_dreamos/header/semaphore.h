/*
 * Dreamos
 * semaphore.h
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
  * Prima versione: 24/04/2004
  */

#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#define MAX_IN_SEM 10
typedef struct {
    int value;
    int sleeping_tasks[10];
} sem_t;

void sem_up(sem_t *);
void sem_down(sem_t *);
sem_t* sem_create();
void sem_delete();
void proc1();
void proc2();
void littledelay();
#endif
