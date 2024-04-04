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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/messages.c,v $
 * Implementation of communications functions
 ***************************************************************************

 ***************************************************************************
 * $Id: messages.c,v 1.9 2001/04/26 19:25:54 iurlano Exp $
 ***************************************************************************
 *
 * $Log: messages.c,v $
 * Revision 1.9  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.8  2001/04/23 14:43:05  iurlano
 * work on FAT module
 * work on FS protocol
 *
 * Revision 1.7  2001/04/19 15:17:58  iurlano
 * Fixed some bug
 * changed ide driver
 * changed fat driver (cabled the pid of the ide driver)
 * changed shell( now accept 3 commands mem, info, ls) TRY IT
 * VERY STABLE SNAPSHOT
 *
 * Revision 1.6  2001/04/18 21:39:43  iurlano
 * Fixed a very insidious memory allocation bug in (getbmsg())
 * little change in xconfig
 * fixed stability problems
 * added two programs for testing message passing
 * Added freemsg()
 * no memory leak now on message passing
 * the shell seems to work!!
 * A strange problem: bochs reports freepage page already freed where the real pc and vmware do not!
 *
 * Revision 1.5  2001/04/17 18:04:12  iurlano
 * Fixed compilation code
 * Warning: This is a very unstable snapshot
 * There are a lot of strange problems
 *
 * Revision 1.4  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.3  2000/12/05 20:59:47  iurlano
 * Thread work in progress. Yet to make first complete subsystem
 *
 * Revision 1.2  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:12  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.3  2000/03/20 23:12:56  iurlano
 * Correzione ricezione messaggi
 *
 * Revision 1.7  1999/12/20 17:33:52  iurlano
 * Aggiunta Fork()
 * aggiunte alcune syscalls
 *
 * Revision 1.6  1999/12/19 23:33:50  iurlano
 * Aggiunto P non bloccante.
 * Aggiunto recmsg bloccante e non.
 * Tolta funzione di rilevazione del memory size da sysinfo e messa in memory.c
 * Aggiunto un semaforo intero che conta i messaggi nella coda
 *
 * Revision 1.5  1999/12/17 22:46:06  iurlano
 *
 * I messaggi sembrano funzionare !!!!
 *
 * Revision 1.4  1999/12/17 17:26:57  iurlano
 * Aggiunte alcune syscall, tipo getpid
 * Lavoro su sistema di messaggi
 * Migliorata libreria
 *
 * Revision 1.3  1999/12/09 00:20:02  iurlano
 * Gran casino!
 *
 * Revision 1.2  1999/12/07 14:42:26  iurlano
 * Aggiunta funzione di terminazione dei processi
 *
 * Revision 1.1  1999/12/02 17:34:35  pecciant
 * Funzioni per messaggi
 *
 *
 ***************************************************************************/

#include <messages.h>
#include <memory.h>
#include <process.h>
#include <types.h>
#include <panic.h>
#include <semaphore.h>
#include <errors.h>
#include <printk.h>

/* Get a free buffer */
void * getbmsg(void){
  PDE * page_dir;   /* PDE of calling process */
  PTE * page_table;
  page * buffer;
  dword ritorno;
  dword k;
  pid_t MyName;
  MyName=Processo_In_Esecuzione;

  if((buffer=getpage())==NULL)return NULL;   

  Bool_P(&(MyName->Mem_Sem));

  page_dir=(PDE *)MyName->PageDir;
  MyName->last_used_page+=4096;
  ritorno=MyName->last_used_page;  
  k=(MyName->last_used_page)>>22;
  if (page_dir[k].Present==0) {
    void *p;
    printk("Get B Msg : Getting a page table\n");
    p=getpagetable();
    if (p==NULL) panic("Memory finished for messages !");
    page_dir[k].Page_Table_Base_Addr=(dword)(p)>>12;
    page_dir[k].Present=1;  
    page_dir[k].Read_Write=1;
    page_dir[k].User=1;
  }
  page_table=(PTE *)((dword)page_dir[k].Page_Table_Base_Addr<<12);
  k=((Processo_In_Esecuzione->last_used_page)>>12)&0x3FF;
  page_table[k].Page_Base_Addr=((dword)buffer)>>12;
  page_table[k].Present=1;
  page_table[k].Read_Write=1;
  page_table[k].User=1;

  Bool_V(&(MyName->Mem_Sem));
  return (void *)ritorno;
}

int freemsg(void * msg) {
  PDE * page_dir;   /* PDE of calling process */
  PTE * page_table;
  dword k;
  page_dir=(PDE *)Processo_In_Esecuzione->PageDir;
  k=((dword)msg)>>22;
  //  page_dir[k].Present=1;
  page_table=(PTE *)((dword)page_dir[k].Page_Table_Base_Addr<<12);
  k=(((dword)msg)>>12)&0x3FF;
  page_table[k].Present=0;
  freepage((page*)(page_table[k].Page_Base_Addr<<12));
  return 0;
}


int sendmsg(PID Destinatario,size_t Dimensione,void * MsgAddr){
  int t;
  pid_t dest,MyName;
  dword phys_addr;
  PTE * pte_page;
  PDE * pde_page;
  int pte,pde;

  MyName=Processo_In_Esecuzione;
  Bool_P(&Processi_Mutex);
  for (t=0;t<MAX_PROCESSES;t++)
    /* Processi[] needs some semaphore? */
    if (Processi[t].pid==Destinatario) {
      dest=&(Processi[t]);
      if (dest->active==0) {
	Bool_V(&Processi_Mutex);
	return E_PID_DEAD;
      }
      Bool_P(&(MyName->Mem_Sem));
      pde_page=(PDE*)(MyName->PageDir);
      pde=((dword)MsgAddr)>>22;
      pte=(((dword)MsgAddr)>>12)&0x3FF;
      pte_page=(PTE*)(pde_page[pde].Page_Table_Base_Addr<<12);
      phys_addr=(pte_page[pte].Page_Base_Addr)<<12;
      
      pte_page[pte].Present=0;

      Bool_V(&(MyName->Mem_Sem));
      Bool_P(&(dest->Msg_Sem));
      if (dest->msg_head==(dest->msg_tail+1)%MAX_MESSAGES) {
	panic("Max Messages Reached, I Should suspend");
      }
      dest->msgqueue[dest->msg_tail].mittente=MyName->pid;
      dest->msgqueue[dest->msg_tail].pagina_fisica=(byte*)phys_addr;
      dest->msgqueue[dest->msg_tail].dimensione=Dimensione;
      dest->msg_tail=(dest->msg_tail+1)%MAX_MESSAGES;
      Bool_V(&(dest->Msg_Sem));
      Bool_V(&Processi_Mutex);      
      V(&(dest->Messages));
      
      return E_SUCCESS;
    }
  Bool_V(&Processi_Mutex);
  return E_PID_NOT_FOUND;
}

void * recmsg(int flags, msginfo * mi){
  pid_t MyName;
  dword Physical;
  dword ritorno;
  PDE * page_dir;   /* PDE of calling process */
  PTE * page_table;
  dword k;

  Physical=0;
  MyName=Processo_In_Esecuzione;

  if (flags==NO_BLOCK) {
    if (Non_Block_P(&(MyName->Messages))==E_SHOULD_BLOCK)
      return NULL;
  } else P(&(MyName->Messages));
  Bool_P(&(MyName->Msg_Sem));
  if (MyName->msg_tail==MyName->msg_head) {
    /* No messages in queue. Should suspend */
    //    Bool_V(&(MyName->Msg_Sem));
    //    return NULL;
    panic("This should not happen NOW!! messages.c : recvmsg");
  } else {
    Physical=(dword)MyName->msgqueue[MyName->msg_head].pagina_fisica;
    mi->mittente=MyName->msgqueue[MyName->msg_head].mittente;
    mi->dimensione=MyName->msgqueue[MyName->msg_head].dimensione;
    MyName->msg_head=(MyName->msg_head+1)%MAX_MESSAGES;
  }
  Bool_V(&(MyName->Msg_Sem));

  Bool_P(&(MyName->Mem_Sem));
  page_dir=(PDE *)MyName->PageDir;
  MyName->last_used_page+=4096;
  ritorno=MyName->last_used_page;  
  k=(MyName->last_used_page)>>22;
  if (page_dir[k].Present==0) {
    void *p;
    p=getpagetable();
    if (p==NULL) panic("Memory finished for messages !");
    page_dir[k].Page_Table_Base_Addr=(dword)(p)>>12;
    page_dir[k].Present=1;  
    page_dir[k].Read_Write=1;
    page_dir[k].User=1;
  }
  page_table=(PTE *)((dword)page_dir[k].Page_Table_Base_Addr<<12);
  k=((MyName->last_used_page)>>12)&0x3FF;
  page_table[k].Page_Base_Addr=((dword)Physical)>>12;
  page_table[k].Present=1;
  page_table[k].Read_Write=1;
  page_table[k].User=1;

  Bool_V(&(MyName->Mem_Sem));
  return (void*)ritorno;
}

int detachmsg(void * buffer){
  freepage((page *)buffer);
  return E_SUCCESS;
}





