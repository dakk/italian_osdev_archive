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


/****************************** O S S O  ***********************************
 * file : kernel/signals.c
 * Signal handling routines (work in progress)
 ***************************************************************************

 ***************************************************************************
 * $Id: signals.c,v 1.5 2001/03/05 23:02:40 iurlano Exp $
 ***************************************************************************
 *
 * $Log: signals.c,v $
 * Revision 1.5  2001/03/05 23:02:40  iurlano
 * Some corrections
 * Little modifies to Makefile
 *
 * Revision 1.4  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.3  2000/11/28 15:14:52  iurlano
 * Minor changes
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:14  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.9  1999/12/21 17:42:01  iurlano
 * *** empty log message ***
 *
 * Revision 1.8  1999/12/21 13:59:09  iurlano
 * Aggiunto funzioni di terminazione e esplosione processi
 * Non mi ricordo...
 *
 * Revision 1.7  1999/12/20 17:33:53  iurlano
 * Aggiunta Fork()
 * aggiunte alcune syscalls
 *
 * Revision 1.6  1999/12/17 17:26:58  iurlano
 * Aggiunte alcune syscall, tipo getpid
 * Lavoro su sistema di messaggi
 * Migliorata libreria
 *
 * Revision 1.5  1999/12/10 15:09:06  iurlano
 * *** empty log message ***
 *
 * Revision 1.4  1999/12/10 13:34:54  iurlano
 * *** empty log message ***
 *
 * Revision 1.3  1996/09/12 05:04:44  iurlano
 * Riordino directory test e lib
 * adesso test compila con lib
 * prove di gestione di segnale a livello utente
 *
 * Revision 1.2  1999/11/29 23:24:28  iurlano
 * Secondo tentativo di gestione di segnali.
 * Sembra funzionare
 *
 * Corretto bug nella gestione delle interruzioni.
 * Aggiunta syscall per settare un signal handler
 *
 * Revision 1.1  1999/11/28 23:21:33  iurlano
 * primo tentativo di una gestione dei segnali
 * Aggiunto signal.c
 *
 *
 ***************************************************************************/


/* 
   Warning : Code obsolete as OSSO 0.1.0 (currently CVS only)
   It's being replaced by events.c 
*/


#include <signals.h>
#include <printk.h>
#include <process.h>
#include <gdt.h>
#include <panic.h>
#include <errors.h>

#include <coda_pronti.h>

void SIGNAL_TERM_HANDLER(void);
void SIGNAL_USER1_HANDLER(void);
void SIGNAL_USER2_HANDLER(void);
void SIGNAL_ZERO_DIVIDE_HANDLER(void);
void SIGNAL_OVERFLOW_HANDLER(void);

Signal_Handler Default_Sig_Handlers[SIGNAL_NUMBER]={
  SIGNAL_TERM_HANDLER,
  SIGNAL_USER1_HANDLER,
  SIGNAL_USER2_HANDLER,
  SIGNAL_ZERO_DIVIDE_HANDLER,
  SIGNAL_OVERFLOW_HANDLER
};


void Process_Signals(void) {
  dword segnale;
  pid_t My_name;
  My_name=Processo_In_Esecuzione;

  Bool_P(&(My_name->Sig_Sem));

  My_name->signals_pending--;
  printk("Process Signals!!\n");
  if (My_name->sig_head==My_name->sig_tail) {
    panic("No signal in queue\n");
  }
  segnale=My_name->signals_queue[My_name->sig_head];
  printk("Process %d receive Signal %d\n",My_name->pid,segnale);
  My_name->sig_head=(My_name->sig_head+1)%MAX_SIGNALS_PENDING;

  Bool_V(&(My_name->Sig_Sem));

  if (My_name->SIG_HANDLERS[segnale]!=0) 
    /*
    {
      dword g;
      dword tt[2];
      g=prenote_gdt_entry();
      ((dword*)(&GDT[g]))[0]=
	((dword)(Processo_In_Esecuzione->SIG_HANDLERS[segnale])&0xFFFF)|(USER_CODE_SEGMENT_SEL<<16);

      ((dword*)(&GDT[g]))[1]=
	((dword)(Processo_In_Esecuzione->SIG_HANDLERS[segnale])&0xFFFF0000)|(0xEC00);

      tt[0]=(dword)(Processo_In_Esecuzione->SIG_HANDLERS[segnale]);
      tt[1]=(g<<3)|3;
      __asm__ ("push %%ds\n"
	       "push %%es\n"
	       "push %%fs\n"
	       "push %%gs\n"
	       "mov %%dx,%%ds\n"
	       "mov %%dx,%%es\n"
	       "mov %%dx,%%fs\n"
	       "mov %%dx,%%gs\n"
	       "lcall %%ss:(%%eax)\n"
	       "pop %%gs\n"
	       "pop %%fs\n"
	       "pop %%es\n"
	       "pop %%ds\n"
	       ::"a" (tt),"d" (USER_DATA_SEGMENT_SEL));
    }
    */
    
    {
      /*
      dword pp;
      dword asp,ass;
      
      ass=USER_DATA_SEGMENT_SEL;
      pp=(Processo_In_Esecuzione->SIG_HANDLERS[segnale]);
      asp=mallosso(16000);
      __asm__ ("push %%ds\n"
	       "push %%es\n"
	       "push %%fs\n"
	       "push %%gs\n"
	       "mov %%dx,%%ds\n"
	       "mov %%dx,%%es\n"
	       "mov %%dx,%%fs\n"
	       "mov %%dx,%%gs\n"::"d" (USER_DATA_SEGMENT_SEL));
      __asm__ ("push %%ecx\n"
	       "push %%ebx\n"
	       "push %%eax\n"
	       "push %%edx\n"
	       "lret"::"a" (USER_CODE_SEGMENT_SEL), "d" (pp),"c"(USER_DATA_SEGMENT_SEL),"b"(asp));
    reprise:
      __asm__ ("pop %gs\n"
	       "pop %fs\n"
	       "pop %es\n"
	       "pop %ds\n");
	       freeosso(asp);
      */
      Processo_In_Esecuzione->SIG_HANDLERS[segnale]();
    } else Default_Sig_Handlers[segnale]();
  
}

void SIGNAL_TERM_HANDLER(void) {
  printk("SIGTERM Unhandled by the process. Terminating.\n");
  Exit_Process();
}

void SIGNAL_USER1_HANDLER(void) {

}

void SIGNAL_USER2_HANDLER(void) {

}

void SIGNAL_ZERO_DIVIDE_HANDLER(void) {

}

void SIGNAL_OVERFLOW_HANDLER(void) {

}


void Signalk(Thread_idx tid,dword segnale) {
  pid_t Processo;
  Processo=Threads[tid].task;
  switch  (segnale) {
  case SIGNAL_DESTROY: 
    printk("DESTROY sent to pid %d!\n",Processo->pid);
    Destroy_Thread(tid);
    return ;
  case SIGNAL_CONT:
    Add_Thread_To_CodaPronti(tid);
    return ;
  case SIGNAL_STOP:
    Remove_Thread_From_CodaPronti(tid);
    return ;
  }
    
  Bool_P(&(Processo->Sig_Sem));
  if (Processo->sig_head==(Processo->sig_tail+1)%MAX_SIGNALS_PENDING) {
    panic("Sig queue Full!"); 
  } else {
    Processo->signals_queue[Processo->sig_tail]=segnale;
    Processo->sig_tail=(Processo->sig_tail+1)%MAX_SIGNALS_PENDING;
    Processo->signals_pending++;
  }
  Bool_V(&(Processo->Sig_Sem));
}

int Signal(PID Processo,dword segnale) {
  int i;

  for (i=0;i<MAX_PROCESSES;i++) {
    if (Processi[i].pid==Processo) {
      if (Processi[i].active==1) {
	Signalk(Processi[i].threads[0],segnale);
	return E_SUCCESS;
      } else return E_PID_DEAD;
    } 
  }
  return E_PID_NOT_FOUND;
}
