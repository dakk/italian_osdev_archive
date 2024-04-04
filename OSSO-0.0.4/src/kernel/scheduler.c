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
 * file : kernel/scheduler.c                                               *
 * Scheduler and running processes manager                                 *
 ***************************************************************************


 ***************************************************************************
 * $Id: scheduler.c,v 1.15 2001/04/26 19:25:54 iurlano Exp $ 
 ***************************************************************************
 *
 * $Log: scheduler.c,v $
 * Revision 1.15  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.14  2001/04/24 17:20:09  iurlano
 * fat now works.
 * Warning: instable snapshot
 * random errors launchi and forking processes
 * now the shell launches commands (only in the root dir)
 *
 * Revision 1.13  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.12  2001/04/03 21:34:44  iurlano
 * Dynamic semaphores work in progress
 * Little changes in make xconfig
 * Added and changed in the code the value of THREAD_INVALID from 0 to -1
 *
 * Revision 1.11  2001/03/05 23:02:40  iurlano
 * Some corrections
 * Little modifies to Makefile
 *
 * Revision 1.10  2001/02/15 21:04:31  iurlano
 * Restore keyboard functionality (expecting MarcoPex to finish his work)
 * Some Bug fixes.
 *
 * Revision 1.9  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.8  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.7  2000/12/07 14:09:43  iurlano
 * Debug in progress
 *
 * Revision 1.6  2000/12/07 12:33:56  iurlano
 * Thread debug phase
 *
 * Revision 1.5  2000/12/05 20:59:47  iurlano
 * Thread work in progress. Yet to make first complete subsystem
 *
 * Revision 1.4  2000/12/05 16:36:01  iurlano
 * Thread managementwork in progress
 *
 * Revision 1.3  2000/08/25 13:39:38  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:13  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#include <sys.h>
#include <printk.h>
#include <process.h>
#include <process_status.h>
#include <gdt.h>
#include <timer.h>

#include <coda_pronti.h>

#define quantum (time_quantum*ms) /* time quantum definition */

static int stampe=19; /* if >0 then decides the number of invocation of the
			scheduler to print */

void Schedule_Next_Thread(void) {
  Thread_idx tid;
  dword oldflags;

  oldflags=get_EFLAGS();
  cli();

  tid=Get_Thread_From_CodaPronti();

  if (stampe>0) {
    /* 
       printk("Currently (Thread_In_Esecuzione == %d) (Processo in esecuzione %x (%d))\n",
       Thread_In_Esecuzione,Processo_In_Esecuzione,Processo_In_Esecuzione!=NULL
       ? Processo_In_Esecuzione->pid : 0);
       
       printk("Next (Thread_In_Esecuzione == %d) (Processo in esecuzione %x (%d))\n",
       tid,Threads[tid].task,(Threads[tid].task)!=0 ? Threads[tid].task->pid 
       : 0);
    */
    stampe--;
  }


  if (tid!=Thread_In_Esecuzione) {
    if (tid!=THREAD_INVALID) {
      if (Thread_In_Esecuzione!=THREAD_INVALID) 
	Add_Thread_To_CodaPronti(Thread_In_Esecuzione);
      Thread_In_Esecuzione=tid;
      Processo_In_Esecuzione=Threads[Thread_In_Esecuzione].task;
      Threads[tid].status=PROCESS_RUN;
      //printk("Scheduling to pid %d\n",pid->pid);
      jump_tss(Threads[tid].tss_des);
    } else {
      // panic("CodaPronti Empty!!\n");
      //printk("Processes' queue empty\n");
    }
  }
  set_EFLAGS(oldflags);
}

extern int isdebug;
/* Scheduler == IRQ0 */
void scheduler(void) { 
  Thread_idx my_name; 
  dword oldflags;

  oldflags=get_EFLAGS();
  cli();

  my_name=Thread_In_Esecuzione;

  setbinterval(0,0x8b/*((byte)quantum&0xFF)*/,
	       0xce/*((byte)quantum>>8)*/,3);
  
  eoi();
  Schedule_Next_Thread();

  set_EFLAGS(oldflags);
  /*
  if (my_name!=0) {
    if ((Threads[Thread_In_Esecuzione].task)->signals_pending>0) {
      Process_Signals();
    }
  }
  */
}


