/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


/*************************** lib O S S O  ***********************************
 * file : lib/include/semaphores.h
 * Header for semaphores functions
 ***************************************************************************

 ***************************************************************************
 * $Id: semaphores.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: semaphores.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2001/04/18 21:39:43  iurlano
 * Fixed a very insidious memory allocation bug in (getbmsg())
 * little change in xconfig
 * fixed stability problems
 * added two programs for testing message passing
 * Added freemsg()
 * no memory leak now on message passing
 * the shell seems to work!!
 * A strange problem: bochs reports freepage page already freed where the real pc and vmware do not!
 *
 * Revision 1.3  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:20  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.1  2000/01/06 17:00:04  iurlano
 * Aggiunto albero directory 'drivers'
 * Aggiunte alcune syscalls
 *
 *
 ***************************************************************************/

#ifndef __SEMAPHORES_LIB
#define __SEMAPHORES_LIB

#include <types.h>
#include <../Config_Kernel.h>

#include <../kernel/include/constants.h>

typedef dword SemaphoreAddress;
typedef dword Bool_SemaphoreAddress;



typedef struct __Semaphore {
  dword lock;
  dword Status;
#ifdef DYNAMIC_SEMAPHORE
  dword Head_Thread,Tail_Thread;
#else
  dword Processi[SEM_MAX_PROCESSES]; // Queue of sleeping processes 
  dword head,tail;
#endif
} Semaphore;

typedef struct __Bool_Semaphore {
  dword lock;
  dword Status;
#ifdef DYNAMIC_SEMAPHORE
  dword Head_Thread,Tail_Thread;
#else
  dword Processi[SEM_MAX_PROCESSES]; /* Queue of sleeping processes */
  dword head,tail;
#endif
} Bool_Semaphore;



void Init_Semaphore(SemaphoreAddress Sem,dword value);
void Init_Bool_Semaphore(Bool_SemaphoreAddress Sem,dword value);

void P(SemaphoreAddress Sem);
void V(SemaphoreAddress Sem);

void Bool_P(Bool_SemaphoreAddress Sem);
void Bool_V(Bool_SemaphoreAddress Sem);

int Non_Block_P(SemaphoreAddress Sem);
int Non_Block_Bool_P(Bool_SemaphoreAddress Sem);


#endif
