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
 * file : kernel/syscall.c                                                 *
 * A Rudimental System Call prototype                                      *
 ***************************************************************************

 ***************************************************************************
 * $Id: syscall.c,v 1.24 2001/08/15 20:06:36 iurlano Exp $
 ***************************************************************************
 *
 * $Log: syscall.c,v $
 * Revision 1.24  2001/08/15 20:06:36  iurlano
 * Added DMA management functions into the kernel and library
 *
 * Revision 1.23  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.22  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.21  2001/05/29 22:36:18  iurlano
 * Worked on xconfig tools. Now loads and saves and has new instructions
 * Changed keyboard driver to work with events subsystem
 *
 * Revision 1.20  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.19  2001/04/23 15:33:19  iurlano
 * work on FAT and fs protocol
 *
 * Revision 1.18  2001/04/19 11:19:03  iurlano
 * fixed bugs
 * fixed bochs problem (very strange)
 *
 * Revision 1.17  2001/04/17 18:04:12  iurlano
 * Fixed compilation code
 * Warning: This is a very unstable snapshot
 * There are a lot of strange problems
 *
 * Revision 1.16  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.15  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.14  2001/04/03 21:34:44  iurlano
 * Dynamic semaphores work in progress
 * Little changes in make xconfig
 * Added and changed in the code the value of THREAD_INVALID from 0 to -1
 *
 * Revision 1.13  2001/04/03 16:50:05  marcopex
 * Some little event testing & debugging
 *
 * Revision 1.12  2001/03/05 23:02:40  iurlano
 * Some corrections
 * Little modifies to Makefile
 *
 * Revision 1.11  2001/02/15 21:04:31  iurlano
 * Restore keyboard functionality (expecting MarcoPex to finish his work)
 * Some Bug fixes.
 *
 * Revision 1.10  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.9  2000/12/07 11:24:21  marcopex
 * try to compile...
 *
 * Revision 1.8  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.7  2000/12/05 16:36:01  iurlano
 * Thread managementwork in progress
 *
 * Revision 1.6  2000/11/24 18:25:54  iurlano
 * Added first try of a DMA memory management. Yet to be tested
 *
 * Revision 1.5  2000/10/23 23:33:44  iurlano
 * Remove ebx references from the libOSSO
 *
 * Revision 1.4  2000/08/25 13:39:38  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.3  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:17  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#include <kernel.h>
#include <sys.h>
#include <gdt.h>
#include <printk.h>
#include <syscalls.h>
#include <types.h>
#include <process.h>
#include <messages.h>
#include <memory.h>
#include <devices.h>
#include <chardevs.h>
#include <panic.h>
#include <pci.h>
#include <dma_memory.h>
#include <dma.h>

#include <coda_pronti.h>

#include <global_data.h>

extern int Last_Pid_Used;
extern Thread_idx CodaPronti[MAX_REFERENCES];
extern  int testa_CodaPronti,coda_CodaPronti;
extern dword free_pages;
extern Event_struct Public_Events [NUM_PUBLIC_EVENTS];

int Syscall(dword a,dword b,dword c,dword d,dword edi) {  
  int i;
  switch (a) {
  case SYS_PRINT:
    switch (c) {
    case SYS_PRINT_CHAR:
      PRINTC(d);
      break;
    case SYS_PRINT_INT:
      PRINTD(d);
      break;
    case SYS_PRINT_HEX:
      PRINTX(d);
      break;
    }
    return 0;
    break;
  case SYS_SIGNAL:
    /* security checks ! :-) */
    return Signal(c,d);
    break;
  case SYS_SIGNAL_HANDLER:
    Processo_In_Esecuzione->SIG_HANDLERS[c]=(void (*)(void))d;

    PRINTS("Sys Signal Handler for signal :");
    PRINTD(c);
    PRINTS(" at address ");
    PRINTX(d);
    PRINTC('\n');
    return 0;
    break;
  case SYS_SEND_MSG:
    sendmsg(b,d,(void *)c);
    return 0;
    break;
  case SYS_RECV_MSG:{
    return (dword)recmsg(0,(msginfo *)c);
  }
  case SYS_GETPID:
    return Processo_In_Esecuzione->pid;
    break;
  case SYS_GETPPID:
    return Processo_In_Esecuzione->father;
    break;
  case SYS_GETB_MSG:
    return (dword)getbmsg();
    break;
  case SYS_FREE_MSG:
    return (dword)freemsg((void*)c);
    break;
  case SYS_MALLOSSO:
    return (dword)mallosso(c);
    break;
  case SYS_FREEOSSO:
    freosso((void*)c);
    return 0;
    break;
  case SYS_FORK:
    return (dword)fork();
    break;
  case SYS_CREATE_PROCESS_ELF:
    return (Create_Process_From_ELF((void*)c,d))->pid;
    break;
  case SYS_CREATE_THREAD:
    {
      int jj;
      /*
      for (jj=0;jj<MAX_THREADS_PER_PROCESS;jj++) {
	if ((Processo_In_Esecuzione->threads[jj]=THREAD_INVALID)) {
	  Processo_In_Esecuzione->threads[jj]=(Create_Thread(Processo_In_Esecuzione,c));
	  Threads[Processo_In_Esecuzione->threads[jj]].tss.ECX=d;
	  if (Processo_In_Esecuzione->threads[jj]==THREAD_INVALID) 
	    panic("Adding thread id = 0 to codapronti\n");
	  Add_Thread_To_CodaPronti(Processo_In_Esecuzione->threads[jj]);
	  return jj;
	}
      }
      */
      jj=Add_New_Thread_To_Process(Processo_In_Esecuzione,c,edi);
      if (jj!=-1) Threads[Processo_In_Esecuzione->threads[jj]].tss.ECX=d;
      Add_Thread_To_CodaPronti(Processo_In_Esecuzione->threads[jj]);
      return jj;
    }
    break;
  case SYS_GET_CH_BUFF:
    return (dword) getcharbuffer(c);
  case SYS_REL_CH_BUFF:
    return (dword) releasecharbuffer(c);
  case SYS_READ:
    return (dword) read(b,(void *)c,d);
  case SYS_WRITE:
    return (dword) write(b,(void *)c,d);
  case SYS_GET_MEM_FREE_PAGES:
    return (free_pages);
  case SYS_GET_MEM_INFO:
    return ((dword)getmeminfo());

  case SYS_EXIT:
    Exit_Process();
    panic ("Exit Process returned!");
    break;
  case SYS_EXIT_THREAD:
    Exit_Thread();
    break;
  case SYS_GET_DMA_MEM:
    return (dword)DMA_Memory_Get_Block((dword*)c);
  case SYS_FREE_DMA_MEM:
    return DMA_Memory_Free_Block((void*)c);
  case SYS_REQ_DMA_CHANNEL:
    return DMA_Request_Channel(c);
  case SYS_FREE_DMA_CHANNEL:
    return DMA_Free_Channel(c);
  case SYS_START_DMA_OP:
    return StartDMA(b,c,d,edi);
  case SYS_INIT_SEM:
    Init_Semaphore((Semaphore *)c,d);
    return 0;
  case SYS_INIT_BOOL_SEM:
    Init_Bool_Semaphore((Bool_Semaphore *)c,d);
    return 0;
  case SYS_P_SEM:
    P((Semaphore *)c);
    return 0;
  case SYS_BOOL_P_SEM:
    Bool_P((Bool_Semaphore *)c);
    return 0;
  case SYS_V_SEM:
    V((Semaphore *)c);
    return 0;
  case SYS_BOOL_V_SEM:
    Bool_V((Bool_Semaphore *)c);
    return 0;
  case SYS_NBLK_P_SEM:
    return Non_Block_P((Semaphore *) c);
  case SYS_NBLK_BOOL_P_SEM:
    return Non_Block_Bool_P((Bool_Semaphore *)c);
  case DRV_PRINT_PROC_INFO:
    cli();
    printk("Last Pid %d\n",Last_Pid_Used);
    printk("Running process : %d (Tid = %d)\nCodaPronti == :\n",
	   Processo_In_Esecuzione->pid,Thread_In_Esecuzione);
    printk("CODA PRONTI : Testa %d coda %d\n",testa_CodaPronti,coda_CodaPronti);
    for (i=testa_CodaPronti;i!=coda_CodaPronti;i=(i+1)%MAX_REFERENCES){
      printk("tid %d (pid %x), active == %d\n",CodaPronti[i],Threads[CodaPronti[i]].pid,
	     Threads[CodaPronti[i]].active);
    }
    sti();
    return 0;
    break;
  case DRV_REGISTER:
    KRegister_Driver(b,c,d);
    return 0;
    break;
  case DRV_GET_CLASS:
    KGet_Device_Class(c,(DeviceClass *)d);
    break;
  case PCI_READ_BYTE:
    return PCI_Read_Byte(c,d);
    break;
  case PCI_READ_WORD:
    return PCI_Read_Word(c,d);
    break;
  case PCI_READ_DWORD:
    return PCI_Read_Dword(c,d);
    break;
  case DRV_SCROLL_UP:
    {
      word t;
      t=get_videomem_start_offset();
      if (t>0) {
	if (t>=800) set_videomem_start_offset(t-800);
	else set_videomem_start_offset(0);
      }
    }
    break;
  case DRV_SCROLL_DOWN:
    {
      extern int abs_y;
      word t;
      t=get_videomem_start_offset();
      if (t<(abs_y-24)*80) {
	if (t<=(abs_y-34)*80) set_videomem_start_offset(t+800);
	else set_videomem_start_offset((abs_y-24)*80);
      }
    }
    break;

  case SYS_GET_HW_SEM:
    {
      return (dword) &(Public_Events[c].semaphore);
    }
    break;
    /* EVENTS */
  case SYS_GETEVENT:
    {
      return (dword) getevent( (char *) c);
    }
    break;
  case SYS_CLEAREVENT:
    {
      return (dword) clearevent( (EVENT) c);
    }
    break;
  case SYS_ESIGNAL:
    {
      return (dword) signal((PID)c,(EVENT)d);
    }
    break;
  case SYS_EWAIT:
    {
      return (dword) wait((EVENT)c,(PID *)d);
    }
    break;
  case SYS_SETEVENT:
    {
      return (dword) setevent((EVENT)c,(ATTRIBUTES)d);
    }
    break;
  case SYS_GETMPSPID:
    {
      return (dword) Get_MPS_Pid();
    }
    break;
  case SYS_SETMPSPID:
    {
      return (dword) Set_MPS_Pid(c);
    }
    break;


  default:
    PRINTS("!!!!!!!!!!!!!!!!  WRONG SYSCALL  !!!!!!!!!!!!!!!!\n");
    return -1;
  }
  return -1;
}

