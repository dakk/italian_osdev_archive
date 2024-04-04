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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/process.h,v $
 * Data Structures for process management
 ***************************************************************************/


/***************************************************************************
 * $Id: process.h,v 1.5 2001/08/09 10:37:53 iurlano Exp $
 ***************************************************************************
 *
 * $Log: process.h,v $
 * Revision 1.5  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.4  2001/06/26 21:21:21  iurlano
 * Started to implement vfs support in lib/fs.c
 *
 * Revision 1.3  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.16  2001/04/18 21:39:43  iurlano
 * Fixed a very insidious memory allocation bug in (getbmsg())
 * little change in xconfig
 * fixed stability problems
 * added two programs for testing message passing
 * Added freemsg()
 * no memory leak now on message passing
 * the shell seems to work!!
 * A strange problem: bochs reports freepage page already freed where the real pc and vmware do not!
 *
 * Revision 1.15  2001/04/05 21:06:13  iurlano
 * Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
 *
 * Revision 1.14  2001/04/03 21:34:44  iurlano
 * Dynamic semaphores work in progress
 * Little changes in make xconfig
 * Added and changed in the code the value of THREAD_INVALID from 0 to -1
 *
 * Revision 1.13  2001/04/02 15:26:15  iurlano
 * Working version with only 40 max processes, due to memory conflicts
 *
 * Revision 1.12  2001/03/15 10:25:24  iurlano
 * Started semaphorization of processes structures
 *
 * Revision 1.11  2001/03/12 15:09:54  marcopex
 * Finished (maybe) the compilation of Events
 *
 * Revision 1.10  2001/03/02 17:56:13  iurlano
 * changed an ld flag in the kernel Makefile. COULD BE A PROBLEM!
 * some reordering
 *
 * Revision 1.9  2001/03/01 12:56:06  iurlano
 * Added some functions
 *
 * Revision 1.8  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.7  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.6  2000/12/05 16:36:01  iurlano
 * Thread managementwork in progress
 *
 * Revision 1.5  2000/12/04 22:24:15  iurlano
 * Thread work just started
 *
 * Revision 1.4  2000/12/04 17:47:55  marcopex
 * Event handling started
 *
 * Revision 1.3  2000/11/30 22:47:15  iurlano
 * Changed gdt handling.
 * Removed LDT for each task and added a global LDT for USER LEVEL tasks
 *
 * Revision 1.2  2000/10/16 13:38:29  iurlano
 * Translation to english
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/


#ifndef __PROCESS_DEFS
#define __PROCESS_DEFS

#include <sys.h>
#include <types.h>
#include <signals.h>
#include <paging.h>
#include <constants.h>
#include <messages.h>
#include <semaphore.h>
#include <events.h>
/* data structure definitions */

typedef
struct __task
{
  Thread_idx threads[MAX_THREADS_PER_PROCESS]; /* Index in the Threads array */
  //  process processo;
  PID pid;
  PID father;
  PDE * PageDir;
  unsigned int level;
  unsigned int active;
  dword status;
  dword last_used_page;
  /* Signal handling */
  dword signals_pending;
  Signal_Handler SIG_HANDLERS[SIGNAL_NUMBER];
  Signal_Handler SigWrapper;
  dword signals_queue[MAX_SIGNALS_PENDING];
  dword sig_head,sig_tail;
  /* Messages */  
  MSG_HEADER * msgqueue;
  dword msg_head,msg_tail;

  Event_struct * Events_List; /* L'inizializzazione a NULL!!! */

  /* Mutual exclusion semaphores */
  Bool_Semaphore Mutex;

  Bool_Semaphore Msg_Sem;
  Bool_Semaphore Sig_Sem;    /* May be obsolete */
  Bool_Semaphore Mem_Sem;    /* For memory map */
  Bool_Semaphore Events_Sem; /* For event list */

  Semaphore Messages;

} task;

typedef task * pid_t;

typedef struct __Thread {
  TSS_32 tss;
  word tss_des; /* GDT index of tss descriptor */
  int active;
  int status;
  PID pid; /* Process who owns this thread */
  pid_t task;
  dword user_stack;
  dword kernel_stack;
  Thread_idx next_thread_suspended_on_my_semaphore;
} Thread;



typedef 
struct __sleeping_process{
  unsigned int time;          /* time to sleep */
  pid_t * process;   /* process identifier */
  struct __sleeping_process *next;
} sleeper;


/* Definition of active processes */

extern task Processi[MAX_PROCESSES];

extern Bool_Semaphore Processi_Mutex;

extern int Numero_Processi;

extern int Numero_CodaPronti;

/* Process' queues */

extern sleeper sleepers[MAXQUEUE][2]; /* sleeping queue */

extern pid_t Processo_In_Esecuzione;

/* Threads */

#define THREAD_INVALID -1

extern Thread Threads[MAX_THREADS]; /* -1 is invalid */
extern Thread_idx Thread_In_Esecuzione;

/* Process creation */

pid_t Create_Process_From_ELF(void * buffer,dword level);
Thread_idx Create_Thread(pid_t Processo,dword start_address,dword start_value);

int Add_New_Thread_To_Process(pid_t Processo,dword start_address,dword start_value); /* Returns the index in the process' array of threads */
int Add_Thread_To_Process(Thread_idx tid,pid_t Processo); /* Returns the index in the process' array of threads */

pid_t Get_Free_Process_Slot(void);
Thread_idx Get_Free_Thread_Slot(void);
dword physical_address(dword address,PDE * pagedir);

pid_t Get_Pid_t_From_Pid(PID pid); /* returns NULL if not found */

int Destroy_Process(pid_t processo);
int Destroy_Thread(Thread_idx tid);
void Exit_Process(void);
void Exit_Thread(void);

void Signalk(Thread_idx tid,dword segnale);
int  Signal(PID Processo,dword segnale);
int  fork(void);


#define MAX_CMD_LINE 20
typedef struct __ps_info {
  dword pid;
  dword ppid;
  dword level;
  dword status;
  char command_line[MAX_CMD_LINE];
} ps_info;


#endif


