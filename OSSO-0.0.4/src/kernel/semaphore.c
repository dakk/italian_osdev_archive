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


/******************************* O S S O ***********************************
 * file : kernel/semaphore.c                                               *
 * Semaphores                                                              *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: semaphore.c,v 1.17 2001/04/10 23:10:26 iurlano Exp $ 
 ***************************************************************************
 *
 * $Log: semaphore.c,v $
 * Revision 1.17  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.16  2001/04/09 12:01:45  marcopex
 * Eventest3 finally works!! (it seems...)
 *
 * Revision 1.15  2001/04/05 21:06:13  iurlano
 * Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
 *
 * Revision 1.14  2001/04/05 14:10:54  iurlano
 * xconfig changes
 *
 * Revision 1.13  2001/04/03 21:34:44  iurlano
 * Dynamic semaphores work in progress
 * Little changes in make xconfig
 * Added and changed in the code the value of THREAD_INVALID from 0 to -1
 *
 * Revision 1.12  2001/04/03 17:14:26  iurlano
 * work on dynamic semaphores
 *
 * Revision 1.11  2001/04/03 11:42:13  iurlano
 * Added a #define for Dynamic Semaphores
 *
 * Revision 1.10  2001/04/03 07:56:14  iurlano
 * Dynamic Semaphores. Alpha code.
 *
 * Revision 1.9  2001/03/07 21:18:04  iurlano
 * Trying another way for colorgcc
 * Added a new type in xconfig parameter file: define
 *
 * Revision 1.8  2001/03/06 22:52:11  iurlano
 * Added lock mechanism to P and V. To be tested
 * Added miniman xconfig Make target
 *
 * Revision 1.7  2001/03/05 23:02:40  iurlano
 * Some corrections
 * Little modifies to Makefile
 *
 * Revision 1.6  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.5  2000/11/09 14:30:32  iurlano
 * Added pci.c module for accessing pci with configuration mechanism 1
 *    without BIOS
 *
 * Revision 1.4  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.3  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.2  2000/06/12 08:05:26  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:13  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.10  2000/01/04 00:56:50  iurlano
 * Bug fixes
 *
 * Revision 1.7  1999/12/19 23:33:50  iurlano
 * Added Non block P
 * Added recmsg and non block recmsg
 * Added integer semaphore
 *
 * Revision 1.5  1999/11/25 12:42:03  iurlano
 * Added boolean P and V
 * Changed Init_Semaphore
 *
 ***************************************************************************/

#include <semaphore.h>
#include <panic.h>
#include <scheduler.h>
#include <process.h>
#include <errors.h>
#include <printk.h>

#include <coda_pronti.h>
#include <lock.h>

#include "../Config_Global.h"
#include "../Config_Kernel.h"


/* Experiment */
dword Test_And_Set(dword * address,dword value) {
  dword ritorno;
  __asm__ __volatile__ ("xchgl %%eax,%0":"=a" (ritorno):"m" (address),"a" (value));
  return ritorno;
}

/* Initialize a semaphore ( for mutual exclusion or synchronization ) */
void Init_Semaphore(Semaphore * Sem, dword value) {
#ifdef SMP_Support
  Sem->lock=1;
#endif
  Sem->Status=value;
#ifdef DYNAMIC_SEMAPHORE
  Sem->Head_Thread=THREAD_INVALID;
  Sem->Tail_Thread=THREAD_INVALID;
#else
  Sem->head=0; /* Queue pointers */
  Sem->tail=0;
#endif
}

void Init_Bool_Semaphore(Bool_Semaphore * Sem,dword value) {
  if (!(value==0 || value==1)) panic("Error initializing bool semaphore\n");
#ifdef SMP_Support
  Sem->lock=1;
#endif
  Sem->Status=value;
#ifdef DYNAMIC_SEMAPHORE
  Sem->Head_Thread=THREAD_INVALID;
  Sem->Tail_Thread=THREAD_INVALID;
#else
  Sem->head=0;
  Sem->tail=0;
#endif
}


/* Decrements a Semaphore if its value is greater than zero. Otherwise Suspends
   the process and put its descriptor in the Semaphore's queue */
void P(Semaphore * Sem) {
  dword flags;
  /* Saves Flags ( To restore the correct Interrupt Flag value) */
  __asm__ __volatile__ ("pushfl\n"
	   "popl %%eax":"=a" (flags));
  cli(); /* Currently working only on uniprocessor systems */
#ifdef SMP_Support
  lock(&(Sem->lock));
#endif

  if (Sem->Status>0) {
    Sem->Status--;
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
  } else {

#ifdef DYNAMIC_SEMAPHORE
    if (Sem->Head_Thread==THREAD_INVALID) {
      Sem->Head_Thread=Thread_In_Esecuzione;
      Threads[Thread_In_Esecuzione].next_thread_suspended_on_my_semaphore=THREAD_INVALID;
      Sem->Tail_Thread=Thread_In_Esecuzione;
    } else {
      Threads[Sem->Tail_Thread].next_thread_suspended_on_my_semaphore=Thread_In_Esecuzione;
      Sem->Tail_Thread=Thread_In_Esecuzione;
      Threads[Thread_In_Esecuzione].next_thread_suspended_on_my_semaphore=THREAD_INVALID;
    }
#else
    if (Sem->head==(Sem->tail+1)%SEM_MAX_PROCESSES) {
      panic("SEM_MAX_PROCESSES reached!");
    } else {
      Sem->Processi[Sem->tail]=Thread_In_Esecuzione;
      Sem->tail=(Sem->tail+1)%SEM_MAX_PROCESSES;
    }
#endif
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
    Processo_In_Esecuzione=NULL;
    Thread_In_Esecuzione=THREAD_INVALID;
    Schedule_Next_Thread();
  }

  /* Restores the flags
     sti() here won't work because it's not sure if interrupts were enabled
     before entering this function
  */
  __asm__ __volatile__ ("push %%eax\n"
	   "popfl\n"::"a" (flags));
}

/* P for Boolean Semaphores */
void Bool_P(Bool_Semaphore * Sem) {
  dword flags;
  __asm__ __volatile__ ("pushfl\n"
	   "popl %%eax":"=a" (flags));
  
  cli();
#ifdef SMP_Support
  lock(&(Sem->lock));
#endif
  if (Sem->Status>0) {
    Sem->Status=0;
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
  } else {
#ifdef DYNAMIC_SEMAPHORE
    printk("Bool P Suspending on semaphore %x with Thread_In_esec=%d,pid=%d\n",Sem,Thread_In_Esecuzione,Processo_In_Esecuzione->pid);
    printk("Sem->Head_Thread = %d\n",Sem->Head_Thread);
    if (Sem->Head_Thread==THREAD_INVALID) {
      Sem->Head_Thread=Thread_In_Esecuzione;
      Threads[Thread_In_Esecuzione].next_thread_suspended_on_my_semaphore=THREAD_INVALID;
      Sem->Tail_Thread=Thread_In_Esecuzione;
    } else {
      Threads[Sem->Tail_Thread].next_thread_suspended_on_my_semaphore=Thread_In_Esecuzione;
      Sem->Tail_Thread=Thread_In_Esecuzione;
      Threads[Thread_In_Esecuzione].next_thread_suspended_on_my_semaphore=THREAD_INVALID;
    }
#else   
    if (Sem->head==(Sem->tail+1)%SEM_MAX_PROCESSES) {
      panic("SEM_MAX_PROCESSES reached!");
    } else {
      Sem->Processi[Sem->tail]=Thread_In_Esecuzione;
      Sem->tail=(Sem->tail+1)%SEM_MAX_PROCESSES;
    }
#endif
#ifdef SMP_Support
      unlock(&(Sem->lock));
#endif
      Processo_In_Esecuzione=NULL;
      Thread_In_Esecuzione=THREAD_INVALID;

      Schedule_Next_Thread();
  }
  __asm__ __volatile__ ("push %%eax\n"
	   "popfl\n"::"a" (flags));
}

/* Works like P, but doesn't suspend the process, but returns E_SHOULD_BLOCK
   if it should suspend */
int Non_Block_P(Semaphore * Sem) {  
  dword flags;
  __asm__ __volatile__ ("pushfl\n"
	   "popl %%eax":"=a" (flags));
  cli();
#ifdef SMP_Support
  lock(&(Sem->lock));
#endif
    if (Sem->Status>0) {
    Sem->Status--;
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
    __asm__ __volatile__ ("push %%eax\n"
	     "popfl\n"::"a" (flags));

    return E_SUCCESS;
  } else {
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
    __asm__ __volatile__ ("push %%eax\n"
	     "popfl\n"::"a" (flags));

    return E_SHOULD_BLOCK;
  }
}

int Non_Block_Bool_P(Bool_Semaphore * Sem) {  
  dword flags;
  __asm__ __volatile__ ("pushfl\n"
	   "popl %%eax":"=a" (flags));

  cli();
#ifdef SMP_Support
  lock(&(Sem->lock));
#endif
  if (Sem->Status>0) {
    Sem->Status=0;
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
    __asm__ __volatile__ ("push %%eax\n"
	     "popfl\n"::"a" (flags));

    return E_SUCCESS; 
  } else {
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
    __asm__ __volatile__ ("push %%eax\n"
	     "popfl\n"::"a" (flags));
   
    return E_SHOULD_BLOCK; 
  }
}


/* This increments the semaphore's value if no process descriptor is in the
   semaphore's queue. Otherwise put the first process in the queue and puts
   it in the Ready processes' queue (CodaPronti)
*/
void V(Semaphore * Sem) {
  dword flags;
  __asm__ __volatile__ ("pushfl\n"
	   "popl %%eax":"=a" (flags):);

  cli();
#ifdef SMP_Support
  lock(&(Sem->lock));
#endif
#ifdef DYNAMIC_SEMAPHORE
  if (Sem->Head_Thread==THREAD_INVALID) {
#else
  if (Sem->head==Sem->tail) {
#endif
    Sem->Status++;
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
  } else {
#ifdef DYNAMIC_SEMAPHORE
    Thread_idx t;
    //    printk("V risveglia %d\n",Sem->Head_Thread);
    t=Threads[Sem->Head_Thread].next_thread_suspended_on_my_semaphore;
    Threads[Sem->Head_Thread].next_thread_suspended_on_my_semaphore=THREAD_INVALID;
    Add_Thread_To_CodaPronti(Sem->Head_Thread);

    Sem->Head_Thread=t;

    if (Sem->Head_Thread==THREAD_INVALID) Sem->Tail_Thread=THREAD_INVALID;
#else
    Add_Thread_To_CodaPronti(Sem->Processi[Sem->head]);
    /* Full queue condition has to be checked */
    Sem->head=(Sem->head+1)%SEM_MAX_PROCESSES;
#endif
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
  }
  __asm__ __volatile__ ("push %%eax\n"
	   "popfl\n"::"a" (flags));

}

void Bool_V(Bool_Semaphore * Sem) {
  dword flags;
  __asm__ __volatile__ ("pushfl\n"
	   "popl %%eax":"=a" (flags):);

  cli();
#ifdef SMP_Support
  lock(&(Sem->lock));
#endif
#ifdef DYNAMIC_SEMAPHORE
  if (Sem->Head_Thread==THREAD_INVALID) {
#else
  if (Sem->head==Sem->tail) {
#endif

    Sem->Status=1;
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
  } else {
#ifdef DYNAMIC_SEMAPHORE
    Thread_idx t;

    //    printk("Bool V risveglia %d\n",Sem->Head_Thread);
    t=Threads[Sem->Head_Thread].next_thread_suspended_on_my_semaphore;
    Threads[Sem->Head_Thread].next_thread_suspended_on_my_semaphore=THREAD_INVALID;
    Add_Thread_To_CodaPronti(Sem->Head_Thread);

    Sem->Head_Thread=t;

    if (Sem->Head_Thread==THREAD_INVALID) Sem->Tail_Thread=THREAD_INVALID;

#else
    Add_Thread_To_CodaPronti(Sem->Processi[Sem->head]);
    /* Full queue condition has to be checked */
    Sem->head=(Sem->head+1)%SEM_MAX_PROCESSES;
#endif
#ifdef SMP_Support
    unlock(&(Sem->lock));
#endif
  }
  __asm__ __volatile__ ("push %%eax\n"
	   "popfl\n"::"a" (flags));

}
