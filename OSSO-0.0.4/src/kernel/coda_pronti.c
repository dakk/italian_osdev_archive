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
 * file : kernel/coda_pronti.c
 * codapronti management routines
 ***************************************************************************

 ***************************************************************************
 * $Id: coda_pronti.c,v 1.4 2001/04/15 14:15:43 iurlano Exp $
 ***************************************************************************
 *
 * $Log: coda_pronti.c,v $
 * Revision 1.4  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.3  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.2  2001/04/03 17:14:26  iurlano
 * work on dynamic semaphores
 *
 * Revision 1.1  2001/03/02 17:56:13  iurlano
 * changed an ld flag in the kernel Makefile. COULD BE A PROBLEM!
 * some reordering
 *
 *
 ***************************************************************************/

#include <types.h>
#include <constants.h>
#include <sys.h>
#include <panic.h>

#include <process.h>
#include <process_status.h>
#include <printk.h>

#include <errors.h>

int testa_CodaPronti=0,coda_CodaPronti=0;
int Numero_CodaPronti;

Thread_idx CodaPronti[MAX_REFERENCES]; // Queue of ready processes

/* return 0 OK, !=0 error */
int Add_Thread_To_CodaPronti(Thread_idx tid) {

  dword flags;
  __asm__ volatile("pushfl\n"
	   "popl %%eax":"=a" (flags):);
  cli();

  if (tid==THREAD_INVALID) panic("Add_Thread_To_CodaPronti: called with invalid_thread");

  //  printk("Add_Thread_To_CodaPronti(%d)\n",tid);
  if (testa_CodaPronti==(coda_CodaPronti+1)%MAX_REFERENCES) {
    panic("CodaPronti full!");
    return 1;
  }
  if (Threads[tid].active!=1) {
    printk("Add_Thread_To_CodaPronti: Thread %d not active",tid);
    panic("");
  }
  CodaPronti[coda_CodaPronti]=tid;
  coda_CodaPronti=(coda_CodaPronti+1)%MAX_REFERENCES;
  Threads[tid].status=PROCESS_READY;
  __asm__ volatile("push %%eax\n"
	   "popfl\n"::"a" (flags));


  return E_SUCCESS;
}

int Remove_Thread_From_CodaPronti(Thread_idx tid) {
  int i,j;

  dword flags;
  __asm__ volatile("pushfl\n"
	   "popl %%eax":"=a" (flags):);
  cli();

  if (tid==THREAD_INVALID) panic("Remove_Thread_To_CodaPronti: called with invalid_thread");
  for (i=testa_CodaPronti;i!=coda_CodaPronti;i=(i+1)%MAX_REFERENCES)
    if (CodaPronti[i]==tid) {
      for (j=(i+1)%MAX_REFERENCES;j!=coda_CodaPronti;j=(j+1)%MAX_REFERENCES,i=(i+1)%MAX_REFERENCES)
	CodaPronti[i]=CodaPronti[j];
      CodaPronti[j]=0;
      coda_CodaPronti=i;

      Threads[tid].status=PROCESS_STOPPED;
      __asm__ volatile("push %%eax\n"
	       "popfl\n"::"a" (flags));
      
      return E_SUCCESS;
    }

  __asm__ volatile("push %%eax\n"
	   "popfl\n"::"a" (flags));


  return E_PID_NOT_FOUND;
}

/* returns NULL error, !=NULL pid extracted */
Thread_idx Get_Thread_From_CodaPronti(void) {
  Thread_idx t;
  dword flags;
  __asm__ volatile("pushfl\n"
	   "popl %%eax":"=a" (flags):);
  cli();

  if (coda_CodaPronti==testa_CodaPronti)
    return THREAD_INVALID;
  t=CodaPronti[testa_CodaPronti];
  testa_CodaPronti=(testa_CodaPronti+1)%MAX_REFERENCES;
  Threads[t].status=PROCESS_STOPPED;
  __asm__ volatile("push %%eax\n"
	   "popfl\n"::"a" (flags));
  return t;
}




