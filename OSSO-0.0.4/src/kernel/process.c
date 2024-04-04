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
 * file : include/process.c                                                *
 * Data Structures and functions for multitasking                          *
 ***************************************************************************

 ***************************************************************************
 * $Id: process.c,v 1.56 2001/08/09 10:37:53 iurlano Exp $
 ***************************************************************************
 *
 * $Log: process.c,v $
 * Revision 1.56  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.55  2001/06/26 21:21:21  iurlano
 * Started to implement vfs support in lib/fs.c
 *
 * Revision 1.54  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.53  2001/06/24 22:18:12  iurlano
 * Optimized process creation functions
 * Added programs to the test section in xconfig
 * Added some functions to kernel-loader.c
 * Page faults now terminate the running process
 * Bugfixed memory leak in program execution from the shell
 *
 * Revision 1.52  2001/06/24 16:10:15  iurlano
 * Optimized process image creation routines
 * Added some programs to xconfig test section
 *
 * Revision 1.51  2001/06/21 09:46:08  iurlano
 * bugfix. Hoping that freepage bug has gone
 *
 * Revision 1.50  2001/06/18 08:50:55  iurlano
 * Modified exit process. Now works correctly with bochs, but not with vmware
 *
 * Revision 1.49  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.48  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
 *
 * Revision 1.47  2001/04/19 11:19:03  iurlano
 * fixed bugs
 * fixed bochs problem (very strange)
 *
 * Revision 1.46  2001/04/12 20:05:00  iurlano
 * memory leak fixed
 *
 * Revision 1.45  2001/04/11 15:26:32  iurlano
 * Thread and processes termination work
 * Memory leak
 *
 * Revision 1.44  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.43  2001/04/09 14:10:53  iurlano
 * Added BOCHS feature
 * Corrected a bug iin process.h
 *
 * Revision 1.42  2001/04/09 12:01:45  marcopex
 * Eventest3 finally works!! (it seems...)
 *
 * Revision 1.41  2001/04/08 19:19:53  iurlano
 * corrected fork()
 *
 * Revision 1.40  2001/04/08 16:32:58  iurlano
 * Added dep target to each Makefile
 * Now dependencies are automatically remade after a make clean
 *
 * Revision 1.39  2001/04/06 10:27:02  marcopex
 * Continue event testing...
 *
 * Revision 1.38  2001/04/03 21:34:44  iurlano
 * Dynamic semaphores work in progress
 * Little changes in make xconfig
 * Added and changed in the code the value of THREAD_INVALID from 0 to -1
 *
 * Revision 1.37  2001/04/03 17:14:26  iurlano
 * work on dynamic semaphores
 *
 * Revision 1.36  2001/04/03 07:56:14  iurlano
 * Dynamic Semaphores. Alpha code.
 *
 * Revision 1.35  2001/04/02 15:26:15  iurlano
 * Working version with only 40 max processes, due to memory conflicts
 *
 * Revision 1.34  2001/04/02 14:35:34  iurlano
 * Tuning memory areas
 *
 * Revision 1.33  2001/04/02 13:16:07  iurlano
 * *** empty log message ***
 *
 * Revision 1.32  2001/03/31 15:07:50  iurlano
 * bugfixes
 *
 * Revision 1.31  2001/03/23 21:05:49  iurlano
 *
 * I've finally found the bug! it is in process.c in init_process_structures
 * the problem is the for that initializes the mutexes of the processes
 * I can't figure why it is wrong, yet.
 *
 * Revision 1.30  2001/03/16 19:22:59  iurlano
 * Added something to the make xconfig prog
 * Now OSSO runs on plex86
 *
 * BIG PROBLEM. OSSO gives random errors about finding INIT and relocating
 *   and doesn't start
 *
 * Revision 1.29  2001/03/15 10:25:24  iurlano
 * Started semaphorization of processes structures
 *
 * Revision 1.28  2001/03/12 15:09:54  marcopex
 * Finished (maybe) the compilation of Events
 *
 * Revision 1.27  2001/03/02 17:56:13  iurlano
 * changed an ld flag in the kernel Makefile. COULD BE A PROBLEM!
 * some reordering
 *
 * Revision 1.26  2001/03/01 12:56:06  iurlano
 * Added some functions
 *
 * Revision 1.25  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.24  2000/12/12 22:29:00  iurlano
 * Threadified fork(). To be tested but seems to be working
 * Added support for coloured printk() in kernel.
 *
 * Revision 1.23  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.22  2000/12/07 15:00:10  marcopex
 * *** empty log message ***
 *
 * Revision 1.21  2000/12/07 14:09:43  iurlano
 * Debug in progress
 *
 * Revision 1.20  2000/12/07 12:33:56  iurlano
 * Thread debug phase
 *
 * Revision 1.19  2000/12/07 11:24:21  marcopex
 * try to compile...
 *
 * Revision 1.18  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.17  2000/12/05 20:59:47  iurlano
 * Thread work in progress. Yet to make first complete subsystem
 *
 * Revision 1.16  2000/12/05 16:36:01  iurlano
 * Thread managementwork in progress
 *
 * Revision 1.15  2000/12/04 22:24:15  iurlano
 * Thread work just started
 *
 * Revision 1.14  2000/11/30 22:47:15  iurlano
 * Changed gdt handling.
 * Removed LDT for each task and added a global LDT for USER LEVEL tasks
 *
 * Revision 1.13  2000/11/26 23:34:59  iurlano
 * work in progress on DMA memory
 *
 * Revision 1.12  2000/11/02 11:59:37  iurlano
 * ELF dynamic linking work in progress
 *
 * Revision 1.11  2000/11/01 23:23:52  iurlano
 * Work on dynamic linking
 *
 * Revision 1.10  2000/10/30 23:02:29  iurlano
 * Working on ELF dynamic linking
 *
 * Revision 1.9  2000/10/30 14:25:00  iurlano
 * First dynamically linked program working
 *
 * Revision 1.8  2000/10/29 21:09:02  iurlano
 * Working on ELF and Dynamic Loader
 *
 * Revision 1.7  2000/10/25 19:59:39  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
 *
 * Revision 1.6  2000/10/23 23:33:44  iurlano
 * Remove ebx references from the libOSSO
 *
 * Revision 1.5  2000/10/23 15:21:24  iurlano
 * Working on ELF support
 *
 * Revision 1.4  2000/08/25 13:39:38  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.3  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.2  2000/07/27 10:35:10  iurlano
 * Translation in progess
 *
 * Revision 1.1.1.1  2000/05/30 08:11:13  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#define __PRINT_MODULE_NAME__ "Process"

#include <types.h>
#include <sys.h>
#include <process.h>
#include <gdt.h>
#include <paging.h>
#include <elf.h>
#include <memory.h>
#include <printk.h>
#include <panic.h>
#include <errors.h>
#include <scheduler.h>
#include <process_status.h>
#include <coda_pronti.h>
#include <semaphore.h>
#include <string.h>

#include "../Config_Kernel.h"

task Processi[MAX_PROCESSES]; // Array of existing processes

Bool_Semaphore Processi_Mutex;

Thread Threads[MAX_THREADS]; /* -1 is invalid */

Bool_Semaphore Threads_Mutex;

Thread_idx Thread_In_Esecuzione; // Running Thread
pid_t Processo_In_Esecuzione;    // Running process

sleeper sleepers[MAXQUEUE][2]; /* sleeping queue */

/* Pointer to the buffer containing LD, the dynamic linker */
char *LD_Elf_Ptr=NULL;

int Numero_Processi;

int Threads_Number;

extern dword Dummy_tss_des;

int Last_Pid_Used=3; // Leaves the first three for the kernel

void init_Process_Structures(void) {
  int i,x;
  Numero_Processi=0;
  Numero_CodaPronti=0;
  Init_Bool_Semaphore(&Processi_Mutex,1);
  Init_Bool_Semaphore(&Threads_Mutex,1);
  Bool_P(&Processi_Mutex);
  Threads_Number=0;
  for (i=0;i<MAX_PROCESSES;i++) {
    Init_Bool_Semaphore(&(Processi[i].Mutex),1);
    Init_Bool_Semaphore(&(Processi[i].Msg_Sem),1);
    Init_Bool_Semaphore(&(Processi[i].Sig_Sem),1);
    Init_Bool_Semaphore(&(Processi[i].Events_Sem),1);
    Init_Bool_Semaphore(&(Processi[i].Mem_Sem),1);
    for (x=0;x<MAX_THREADS_PER_PROCESS;x++)
      Processi[i].threads[x]=THREAD_INVALID;
  }
  Bool_V(&Processi_Mutex);
  for (i=0;i<MAX_THREADS;i++) {
    Threads[i].active=0;
    Threads[i].next_thread_suspended_on_my_semaphore=THREAD_INVALID;
  }
}

pid_t Get_Pid_t_From_Pid(PID pid) {
  int i;
  Bool_P(&Processi_Mutex);
  for (i=0;i<MAX_PROCESSES;i++) {
    if (Processi[i].pid==pid) {
      Bool_V(&Processi_Mutex);
      return &(Processi[i]);
    }
  }
  Bool_V(&Processi_Mutex);
  return NULL;
}


pid_t Get_Free_Process_Slot(void) {
  int t;

  Bool_P(&Processi_Mutex);
  for (t=0;(t<MAX_PROCESSES) && (Processi[t].active==1);t++);
  if (t<MAX_PROCESSES) {
    Processi[t].active=1;
    Numero_Processi++;
    Processi[t].pid=++Last_Pid_Used;
    Bool_V(&Processi_Mutex);
    return &(Processi[t]);
  }
  Bool_V(&Processi_Mutex);
  panic("MAX_PROCESSES Reached!!");
  return NULL;
}

Thread_idx Get_Free_Thread_Slot(void) {
  int t;

  Bool_P(&Threads_Mutex);
  for (t=0;(t<MAX_THREADS) && (Threads[t].active==1);t++);
  if (t<MAX_THREADS) {
    Threads[t].active=1;
    Threads_Number++;
    //Threads[t].pid=++Last_Pid_Used;

    Bool_V(&Threads_Mutex);
    return (Thread_idx)t;
  }
  Bool_V(&Threads_Mutex);
  panic("MAX_THREADS Reached!!");
  return THREAD_INVALID;
}

dword physical_address(dword address,PDE * pagedir) {
  dword pte,pde;
  PTE * gt;
  pde=((dword)address)>>22;
  pte=(((dword)address)>>12)&0x3FF;
  gt=(PTE *)(pagedir[pde].Page_Table_Base_Addr<<12);
  return ((gt[pte].Page_Base_Addr<<12)|(address&0xFFF));
}

void mem_alloc_and_copy(byte * source,byte * dest, dword size,
			PDE * Pagedir,dword level) {
  dword i;
  dword pde,pte;
  dword paggina;

  PTE * gt;

  //  printk("Mem alloc and copy(%x,%x,%d,%x,%d)\n",source,dest, size,
  //	 Pagedir,level);

  //  printk("Copying %x bytes from %x to %x ...\n",size,source,dest); 
  
#ifdef NEW_MEM_ALLOC_AND_COPY
  dword current;
  dword rest,todo;
  dword j;
  byte * write;

  todo=size;
  current=(dword)dest;
  i=0;
  while (todo>0) {
    pde=current>>22;
    pte=(current>>12)&0x3FF;
    if (Pagedir[pde].Present==0) {
      paggina=(dword)getpagetable();
      //      printk("Got pagedir Entry = %x\n",paggina);
      Pagedir[pde].Page_Table_Base_Addr=(dword)(paggina)>>12;
      Pagedir[pde].Present=1;
      Pagedir[pde].Read_Write=1;
      if (level==User_Level)
	Pagedir[pde].User=1;
      else Pagedir[pde].User=0;
    }
    gt=(PTE *)(Pagedir[pde].Page_Table_Base_Addr<<12);
    if (gt[pte].Present==0) {
      paggina=(dword)getpage();
      //      printk("Got page = %x for address %x\n",paggina,dest+i);
      gt[pte].Page_Base_Addr=(dword)(paggina)>>12;
      gt[pte].Present=1;
      gt[pte].Read_Write=1;
      if (level==User_Level)
	gt[pte].User=1;
      else gt[pte].User=0;
    }
    rest=DIM_PAGE - (current%DIM_PAGE);
    if (rest>todo) rest=todo;
    j=current+rest;
    write=(byte*)physical_address(current,Pagedir);
    for (;current<j;current++) {
      *write=source[i++];
      write++;
    }
    todo-=rest;
  }
#else
  for (i=0; i<size;i++) {
    pde=((dword)dest+i)>>22;
    pte=(((dword)dest+i)>>12)&0x3FF;
    if (Pagedir[pde].Present==0) {
      paggina=(dword)getpagetable();
      //      printk("Got pagedir Entry = %x\n",paggina);
      Pagedir[pde].Page_Table_Base_Addr=(dword)(paggina)>>12;
      Pagedir[pde].Present=1;
      Pagedir[pde].Read_Write=1;
      if (level==User_Level)
	Pagedir[pde].User=1;
      else Pagedir[pde].User=0;
    }
    gt=(PTE *)(Pagedir[pde].Page_Table_Base_Addr<<12);
    if (gt[pte].Present==0) {
      paggina=(dword)getpage();
      //      printk("Got page = %x for address %x\n",paggina,dest+i);
      gt[pte].Page_Base_Addr=(dword)(paggina)>>12;
      gt[pte].Present=1;
      gt[pte].Read_Write=1;
      if (level==User_Level)
	gt[pte].User=1;
      else gt[pte].User=0;
    }
    //    if (physical_address((dword)dest+i,Pagedir)==0x9040000) {
    //      printk("pde = %x pte = %x base = %x",pde<<22,pte<<12,gt[pte].Page_Base_Addr);
    //    }
    
    //printk("Writing at address : %x\n",physical_address((dword)dest+i,Pagedir));
    *((char*)physical_address((dword)dest+i,Pagedir))=source[i];
  }

#endif
}



void mem_alloc_and_fill(byte value,byte * dest, dword size,
			PDE * Pagedir,dword level) {
  dword i;
  dword pde,pte;
  dword paggina;
  PTE * gt;
  //printk("Filling %x bytes at %x with %x ...\n",size,dest,value); 
#ifdef NEW_MEM_ALLOC_AND_COPY
  dword current;
  dword rest,todo;
  dword j;
  byte * write;

  todo=size;
  current=(dword)dest;
  i=0;
  while (todo>0) {
    pde=current>>22;
    pte=(current>>12)&0x3FF;
    if (Pagedir[pde].Present==0) {
      paggina=(dword)getpagetable();
      //      printk("Got pagedir Entry = %x\n",paggina);
      Pagedir[pde].Page_Table_Base_Addr=(dword)(paggina)>>12;
      Pagedir[pde].Present=1;
      Pagedir[pde].Read_Write=1;
      if (level==User_Level)
	Pagedir[pde].User=1;
      else Pagedir[pde].User=0;
    }
    gt=(PTE *)(Pagedir[pde].Page_Table_Base_Addr<<12);
    if (gt[pte].Present==0) {
      paggina=(dword)getpage();
      //      printk("Got page = %x for address %x\n",paggina,dest+i);
      gt[pte].Page_Base_Addr=(dword)(paggina)>>12;
      gt[pte].Present=1;
      gt[pte].Read_Write=1;
      if (level==User_Level)
	gt[pte].User=1;
      else gt[pte].User=0;
    }
    rest=DIM_PAGE - (current%DIM_PAGE);
    if (rest>todo) rest=todo;
    j=current+rest;
    write=(byte*)physical_address(current,Pagedir);
    for (;current<j;current++) {
      *write=value;
      write++;
    }
    todo-=rest;
  }
#else
  for (i=0; i<size;i++) {
    pde=((dword)dest+i)>>22;
    pte=(((dword)dest+i)>>12)&0x3FF;
    if (Pagedir[pde].Present==0) {
      paggina=(dword)getpagetable();
      Pagedir[pde].Page_Table_Base_Addr=(dword)(paggina)>>12;
      Pagedir[pde].Present=1;
      Pagedir[pde].Read_Write=1;
      if (level==User_Level)
	Pagedir[pde].User=1;
      else Pagedir[pde].User=0;
    }
    gt=(PTE *)(Pagedir[pde].Page_Table_Base_Addr<<12);
    if (gt[pte].Present==0) {
      paggina=(dword)getpage();
      gt[pte].Page_Base_Addr=(dword)(paggina)>>12;
      gt[pte].Present=1;
      gt[pte].Read_Write=1;
      if (level==User_Level)
	gt[pte].User=1;
      else gt[pte].User=0;

    }
    
    *((char*)physical_address((dword)dest+i,Pagedir))=value;
  }

#endif
}


int Add_New_Thread_To_Process(pid_t Processo,dword start_address,dword start_value) {
  return Add_Thread_To_Process(Create_Thread(Processo,start_address,start_value),Processo);
}

int Add_Thread_To_Process(Thread_idx tid,pid_t Processo) {
  int jj;
  if (tid==THREAD_INVALID) panic("Adding invalid thread id to codapronti\n");
  printk("Adding new thread (%d) to process %d semaphore = %d\n",tid,Processo->pid,Processo->Mutex.Status);
  Bool_P(&(Processo->Mutex));
  for (jj=0;jj<MAX_THREADS_PER_PROCESS;jj++) {
    if (Processo->threads[jj]==THREAD_INVALID) {
      Processo->threads[jj]=tid;
      Bool_V(&(Processo->Mutex));
      //      Add_Thread_To_CodaPronti(Processo->threads[jj]);
      return jj;
    }
  }
  Bool_V(&(Processo->Mutex));
  return -1; /* It differs from THREAD_INVALID 
		it's the index in the process thread table
	     */
}

Thread_idx Create_Thread(pid_t Processo,dword start_address,dword start_value) {
  Thread_idx tid;
  TSS_32 *p;
  dword tt;

  tid=Get_Free_Thread_Slot();
  if (tid==THREAD_INVALID) panic("Thread descriptors finished");
  //  printk("Creating thread with tid = %d (Start address %x)\n",tid,start_address);
  p=&(Threads[tid].tss);
  Threads[tid].tss_des=prenote_gdt_entry();
  Threads[tid].task=Processo;  
  Threads[tid].pid=Processo->pid;  

  if (Processo->level==User_Level) {
    GDT[Threads[tid].tss_des]=PackDescriptor((dword) p,sizeof(*p),
					     SD_Type_Available_TSS_32,3,
					     SD_Present,SD_Size_32);

    p->CS=USER_CODE_SEGMENT_SEL;
    p->DS=USER_DATA_SEGMENT_SEL;
    p->ES=USER_DATA_SEGMENT_SEL;
    p->FS=USER_DATA_SEGMENT_SEL;
    p->GS=USER_DATA_SEGMENT_SEL;
    p->SS=USER_DATA_SEGMENT_SEL;

    p->LDT=USER_LDT_SEGMENT_SEL; // Needed for user space processes 
  } else { /* SuperVisor level */
    GDT[Threads[tid].tss_des]=PackDescriptor((dword) p,sizeof(*p),
					     SD_Type_Available_TSS_32,0,SD_Present,
					     SD_Size_32);
    p->CS=KERNEL_CODE_SEGMENT_SEL;
    p->DS=KERNEL_DATA_SEGMENT_SEL;
    p->ES=KERNEL_DATA_SEGMENT_SEL;
    p->FS=KERNEL_DATA_SEGMENT_SEL;
    p->GS=KERNEL_DATA_SEGMENT_SEL;
    p->SS=KERNEL_DATA_SEGMENT_SEL;
    
    p->LDT=0;
  }
  p->SS0=KERNEL_DATA_SEGMENT_SEL;

  p->SS1=0;
  p->ESP1=0;
  p->SS2=0;
  p->ESP2=0;

  p->link=0;

  p->ESI=0;
  p->EDI=0;
  p->EBP=0;
  
  p->EFLAG=0|EF_Interrupt|EF_IOPL_Mask;

  p->EAX=start_value;
  p->EDX=0;
  p->ECX=0;
  p->EBX=0;
  p->CR3=(dword)(Processo->PageDir);
  p->EIP=start_address;

  /* Da rimettere a posto*/
  Processo->last_used_page+=4096;
  tt=Linear_alloc(Processo,
		  THREAD_KERNEL_STACKS_SIZE,
		  SuperVisor_Level);
  if (tt==0) panic("[Create_Thread()]:Not enough memory for Linear_alloc");
  //printk("[Create_Thread()]:Supervisor level stack allocated at %x\n",tt);
  Threads[tid].kernel_stack=tt;
  p->ESP0=tt+THREAD_KERNEL_STACKS_SIZE-4;

  if (Processo->level==SuperVisor_Level) {
    p->ESP=tt+THREAD_KERNEL_STACKS_SIZE-4;
    Threads[tid].user_stack=0;
  } else {
    Processo->last_used_page+=4096;
    tt=Linear_alloc(Processo,THREAD_USER_STACKS_SIZE,User_Level);
    if (tt==0) panic("[Create_Thread()]:Not enough memory for Linear_alloc");
    p->ESP=tt+THREAD_USER_STACKS_SIZE-4;
    Threads[tid].user_stack=tt;
  }

  /* Fine Da rimettere a posto*/
  return tid;
}

pid_t Create_Process_From_ELF(void * buffer,dword level) {
  word k;
  PDE *pagedir;
  ELF_Header *Header, *LD_Header;
  ELF_Program_Header *PHeader, *LD_PHeader;
  pid_t Pid;
  dword hma; /* Higher memory address */
  int dynamic_linking; /* flag for dynamic linking check */
  dword base_address; /* Base Address of program */
  dword LD_base_address; /* Base Address of LD */
  dword dyn_vaddr;

  dynamic_linking=0;
  hma=0;
  LD_Header=NULL;
  dyn_vaddr=0;
  LD_base_address=0;
  base_address=0;
  //printk("Create_Process_From_ELF\n");
  Header=(ELF_Header *) buffer;
  PHeader=(ELF_Program_Header *)((dword)buffer+(Header->e_phoff));

  /* ELF header check */
  if (strncmp(((char *) buffer)+1,"ELF",3)!=0) {
    printk("BAD ELF Header!\n");
    return (pid_t)NULL;
  }
  if (Header->e_type != ET_EXEC) {
    printk("ERROR : Not an executable file!\n");
    return (pid_t)NULL;
  }

  if (Header->e_phoff==0) {
    printk("ERROR : File doesn't have a program header!\n");
    return (pid_t)NULL;
  }

  for (k=0;k<(Header->e_phnum);k++) {
    if (PHeader[k].p_type==PT_INTERP) {
      dynamic_linking=1;
      if (LD_Elf_Ptr==NULL) {
	printk("Dynamically linked program, but Dynamic Loader not found. Aborting\n");
	return (pid_t)NULL;
      }
    }
  }



  Pid=Get_Free_Process_Slot();

  //printk("program header sections : %d\n",Header->e_phnum);
  /* Paging Initialization for the first MBs of physical memory */
  
  //  init_PDE(pagedir);

  pagedir=getpagedirectory();
  
  for (k=0;k<0x20;k++) {
    pagedir[k]=Kernel_Page_Dir[k];
  }

  for (k=0x20;k<1024;k++) {
    pagedir[k].Present=0;
  }
  
  //printk("File Type : %d\n",Header->e_type);

  /* Parsing ELF Program header */

  /* Loading everything found in program header, 
     not sure if it is right for both statically 
     and dinamycally linked programs
  */

  for (k=0;k<(Header->e_phnum);k++) {
    if (PHeader[k].p_type==PT_LOAD) { // Added for test
      mem_alloc_and_copy((byte *)((dword)buffer+PHeader[k].p_offset),
			 (byte *)PHeader[k].p_vaddr,PHeader[k].p_filesz,
			 pagedir,level);
      if (PHeader[k].p_filesz<PHeader[k].p_memsz)
	mem_alloc_and_fill(0,(byte *)(PHeader[k].p_vaddr+PHeader[k].p_filesz),
			   (PHeader[k].p_memsz-PHeader[k].p_filesz),
			   pagedir,level);
      if (PHeader[k].p_vaddr+PHeader[k].p_memsz>hma)
	hma=PHeader[k].p_vaddr+PHeader[k].p_memsz;
    }
  }

  hma=((hma>>12)+2)<<12;

  if (dynamic_linking==0) {
    /* Statically linked ELF object, go straight */
  } else {
    ELF_Symbol_Table_entry *sym;
    ELF_Dynamic_Entry *dyn=NULL;
    //    int dynidx,lth;
    int jmprelidx;
    int rel_type;

    /* Dynamically linked ELF object, LD is needed */
    base_address=-1;
    for (k=0;k<(Header->e_phnum);k++) {
      if ((PHeader[k].p_type==PT_LOAD) && (PHeader[k].p_vaddr<base_address))
	base_address=PHeader[k].p_vaddr; 
    }
    if (base_address==-1) {
      printk("Error Computing base address!!\n");
      return (pid_t)-1;
    }
    base_address&=0xFFFFF000; /* Aligning to page boundary */

    //    printk("Program base address = %x\n",base_address);

    LD_Header=(ELF_Header *) LD_Elf_Ptr;
    LD_PHeader=(ELF_Program_Header *)((dword)LD_Elf_Ptr+(LD_Header->e_phoff));
    LD_base_address=hma&0xFFFFF000;

    /*
      printk("LD_Header = %x; LD_PHeader = %x\n",LD_Header,LD_PHeader);
      printk("Linker base address = %x\n",LD_base_address);
    */
    for (k=0;k<(LD_Header->e_phnum);k++) {
      if (LD_PHeader[k].p_type==PT_DYNAMIC) {
	dyn=(ELF_Dynamic_Entry*)(LD_PHeader[k].p_offset+LD_Elf_Ptr);
	dyn_vaddr=LD_PHeader[k].p_vaddr+LD_base_address;
      }
      mem_alloc_and_copy((char*)((dword)LD_Elf_Ptr+LD_PHeader[k].p_offset),
			 (char *)LD_PHeader[k].p_vaddr+LD_base_address,LD_PHeader[k].p_filesz,
			 pagedir,level);
      if (LD_PHeader[k].p_filesz<LD_PHeader[k].p_memsz)
	mem_alloc_and_fill(0,(char *)(LD_base_address+LD_PHeader[k].p_vaddr
				      +LD_PHeader[k].p_filesz),
			   (LD_PHeader[k].p_memsz-LD_PHeader[k].p_filesz),
			   pagedir,level);
      if (LD_base_address+LD_PHeader[k].p_vaddr+LD_PHeader[k].p_memsz>hma)
	hma=LD_base_address+LD_PHeader[k].p_vaddr+LD_PHeader[k].p_memsz;
    }
    rel_type=DT_REL;
    k=0;
    //    printk("Address of dynamic section : %x(%x)\n",dyn,dyn_vaddr);
    while (dyn[k].d_tag!=DT_NULL) {
      //printk("%d: %x\n",k,dyn[k].d_tag);
      switch (dyn[k].d_tag) {
      case DT_PLTREL:
	rel_type=dyn[k].d_un.d_val;
	break;
      case DT_SYMTAB:
	sym=(ELF_Symbol_Table_entry*)(dyn[k].d_un.d_ptr+(dword)LD_Elf_Ptr);
	break;
      case DT_JMPREL:
	jmprelidx=k;
	break;
      }
      k++;
    }  
    //    printk("Rel Type : %s\n",rel_type==DT_REL?"DT_REL":"DT_PLTREL");

    /* Passing parameters to the dynamic linker of libOSSO.so */
    //    printk("dyn_vaddr = %x\n",dyn_vaddr);
    
  }

  //printk("Program entry point at %x\n",Header->e_entry);

  hma=((hma>>12)+2)<<12;  
  //  printk("Bool_P\n");
  //  printk("Bool_P(&(Pid->Mutex)) in Create_Process_From_Elf\n"); 
  Bool_P(&(Pid->Mutex));
  //  printk("dopo Bool_P(&(Pid->Mutex)) in Create_Process_From_Elf\n"); 
  //printk("Fine Bool_P\n");
  Pid->father=Processo_In_Esecuzione->pid;

  Pid->last_used_page=hma; /* It's already on page boundary */
  if (((Pid->last_used_page)&0xFFF)!=0)
    printk("ERROR: Last Used Page not on page boundary (%x)!\n",hma);
  Pid->signals_pending=0;
  Pid->SigWrapper=NULL;
  for (k=0;k<SIGNAL_NUMBER;k++) Pid->SIG_HANDLERS[k]=NULL;
  Pid->sig_head=0;
  Pid->sig_tail=0;

  Init_Bool_Semaphore(&(Pid->Msg_Sem),1);
  Init_Bool_Semaphore(&(Pid->Sig_Sem),1);
  Init_Bool_Semaphore(&(Pid->Mem_Sem),1);
  Init_Semaphore(&(Pid->Messages),0);

  Pid->msg_head=0;
  Pid->msg_tail=0;
  Pid->msgqueue=(MSG_HEADER*)getpage();
  Pid->level=level;
  Pid->status=PROCESS_READY;
  Pid->PageDir=pagedir;

  for (k=0;k<MAX_THREADS_PER_PROCESS;k++) Pid->threads[k]=THREAD_INVALID;

  //  printk("Process Thread creation...\n");
  if (dynamic_linking==0) {
    Pid->threads[0]=Create_Thread(Pid,Header->e_entry,0);
  } else {
    TSS_32 *p;
    Pid->threads[0]=Create_Thread(Pid,LD_Header->e_entry+LD_base_address,0);
    p=&(Threads[Pid->threads[0]].tss);
    p->EAX=LD_base_address;
    p->EDX=base_address;
    p->ECX=dyn_vaddr;
    p->EBX=0;
  }
  
  Bool_V(&(Pid->Mutex));

  return Pid;
}

extern Thread_idx kernel_thread;

int Destroy_Thread(Thread_idx tid) {
  int jj;
  if (tid==kernel_thread)
    panic("Cannot destroy kernel thread!!!\n");
  Remove_Thread_From_CodaPronti(tid);
  if (tid==Thread_In_Esecuzione) {
    panic("Destroy Thread called with the active thread\n");
  }

  Bool_P(&(Processi[Threads[tid].pid].Mutex));
  for (jj=0;jj<MAX_THREADS_PER_PROCESS;jj++) {
    if (Processi[Threads[tid].pid].threads[jj]==Thread_In_Esecuzione) {
      Processi[Threads[tid].pid].threads[jj]=THREAD_INVALID;
      break;
    }
  }
  Bool_V(&(Processi[Threads[tid].pid].Mutex));
  if (Threads[Thread_In_Esecuzione].kernel_stack!=0) {
    Linear_free(Threads[tid].task,Threads[tid].kernel_stack,THREAD_KERNEL_STACKS_SIZE);
    Threads[Thread_In_Esecuzione].kernel_stack=0;
  }
  if (Threads[tid].user_stack!=0) {
    Linear_free(Threads[tid].task,Threads[tid].user_stack,THREAD_USER_STACKS_SIZE);
    Threads[Thread_In_Esecuzione].user_stack=0;
  }

  Threads[tid].active=0;
  Threads_Number--;
  free_gdt_entry(Threads[tid].tss_des);

  return E_SUCCESS;
}
 
int Destroy_Process(pid_t processo) {
  PDE * pagedir;
  PTE * pinko;
  int t,i;
  dword oldflags;

  //  printk("  Bool_P(&(processo->Mutex)) in destroy process\n");
  Bool_P(&(processo->Mutex));
  //  printk("dopo Bool_P(&(processo->Mutex)) in destroy process\n");

  if ((processo->active)==0) {
    Bool_V(&(processo->Mutex));
    return E_PID_DEAD;
  }

  oldflags=get_EFLAGS();
  cli();
  processo->active=0;

  for (i=0;i<MAX_THREADS_PER_PROCESS;i++) {
    if ((processo->threads[i]==Thread_In_Esecuzione)) panic("Destroy_Process called with thread in esecuzione\n");
    if ((processo->threads[i]!=THREAD_INVALID)&&(processo->threads[i]!=Thread_In_Esecuzione)) Destroy_Thread(processo->threads[i]);
  }

  if (i!=E_SUCCESS) printk("Process not found!\n");

  //printk("Entrato in Destroy Process\n");

  pagedir=(PDE *)(processo->PageDir);

  /* Destroy Message Queue */

  Bool_P(&(processo->Msg_Sem));
  for (i=processo->msg_head;i!=processo->msg_tail;i=(i+1)%MAX_MESSAGES)
    freepage((page *)(processo->msgqueue[i].pagina_fisica));
  freepage((page*)(processo->msgqueue));
  Bool_V(&(processo->Msg_Sem));
  Bool_P(&(processo->Mem_Sem));
  /* Da rifare i conti, perche' sono di sicuro scazzati */
  // for (t=0;t<0x20;t++) /* 0x20 == 128MB */
  // if (pagedir[t].Present==1) freepage (pagedir[t].Page_Table_Base_Addr<<12);
  for (t=0x20;t<1024;t++) { /* 0x20 == 128MB */
    if (pagedir[t].Present==1) {

      pinko=(PTE *)((pagedir[t].Page_Table_Base_Addr<<12));

      for (i=0;i<1024;i++) {
	if (pinko[i].Present==1) {
	  freepage((page *)((pinko[i].Page_Base_Addr)<<12));
	  pinko[i].Present=0;
	}
      }
    
      freepage((page *)(pinko));
      //printk("P\n");
    }
    
  }
  Bool_V(&(processo->Mem_Sem));
  processo->status=PROCESS_DEAD;
  freepage((page *)pagedir);
  //printk("PP\n");
  //  free_gdt_entry(processo->processo.ldt_des);  
  Numero_Processi--;
  set_EFLAGS(oldflags);

  Bool_V(&(processo->Mutex));
  return E_SUCCESS;

}

int fork(void) {
  //  return E_NOT_IMPLEMENTED_YET;

  pid_t newprocess;
  PDE * pagedir,* new_pagedir;
  PTE * pagetable,* new_pagetable;
  int i,t,k;
  TSS_32 * p;
  //  Thread_idx tid;
  PID old;
  dword oldflags;
  byte * indirizzo_src, * indirizzo_dst;
  
  //  printk("Entered in fork(Free Pages = %d)\n",free_pages);
  oldflags=get_EFLAGS();
  cli();
  newprocess=Get_Free_Process_Slot();

  old=newprocess->pid;


    
  *newprocess=*Processo_In_Esecuzione;

  newprocess->pid=old;

  for (i=0;i<MAX_THREADS_PER_PROCESS;i++)
    newprocess->threads[i]=THREAD_INVALID;
  //  p->ldt_des=prenote_gdt_entry();

  //newprocess->pid=++Last_Pid_Used;
  newprocess->father=Processo_In_Esecuzione->pid;
  newprocess->signals_pending=0;
  newprocess->sig_head=0;
  newprocess->sig_tail=0;

  newprocess->msg_head=0;
  newprocess->msg_tail=0;
  newprocess->msgqueue=(MSG_HEADER*)getpage();

  /* EVENTS */
  /* Da considerare la copia degli eventi */
  newprocess->Events_List=NULL;

  Init_Bool_Semaphore(&(newprocess->Mutex),1);
  Init_Bool_Semaphore(&(newprocess->Msg_Sem),1);
  Init_Bool_Semaphore(&(newprocess->Sig_Sem),1);
  Init_Bool_Semaphore(&(newprocess->Mem_Sem),1);
  Init_Bool_Semaphore(&(newprocess->Events_Sem),1);
  Init_Semaphore(&(newprocess->Messages),0);

  //printk("Fork() : Preparing process copy\n");

  pagedir=(PDE*)Processo_In_Esecuzione->PageDir;
  new_pagedir=getpagedirectory();
  newprocess->PageDir=new_pagedir;
    for (k=0;k<0x20;k++) {
    new_pagedir[k]=Kernel_Page_Dir[k];    
  }
  //init_PDE(new_pagedir);
  for (i=0x20;i<1024;i++) {
    if (pagedir[i].Present==1) {
      new_pagedir[i]=pagedir[i];
      pagetable=(PTE *)((pagedir[i].Page_Table_Base_Addr)<<12);
      new_pagetable=getpagetable();
      //printk("Getting a pagetable...\n");
      new_pagedir[i].Page_Table_Base_Addr=((dword)new_pagetable)>>12;
      for (t=0;t<1024;t++) {
	if (pagetable[t].Present!=0) {
	  new_pagetable[t]=pagetable[t];
	  indirizzo_dst=(byte*)getpage();
	  //printk("Getting a page...\n");
	  new_pagetable[t].Page_Base_Addr=((dword)indirizzo_dst)>>12;
	  indirizzo_src=(byte*)((pagetable[t].Page_Base_Addr)<<12);
	  for (k=0;k<4096;k++) {
	    indirizzo_dst[k]=indirizzo_src[k];
	  }
	} else new_pagetable[t].Present=0;
      }

    } else new_pagedir[i].Present=0;

  }

  //printk("Fork() : process copy done\n");


  //  newprocess->threads[0]=Create_Thread(newprocess,(dword)(&&figlio));
  //  p=&(Threads[newprocess->threads[0]].tss);
  {
    Thread_idx tid;
    tid=Get_Free_Thread_Slot();
    newprocess->threads[0]=tid;
    if (tid==THREAD_INVALID) panic("Thread descriptors finished");
    Threads[tid]=Threads[Thread_In_Esecuzione];
    Threads[tid].tss_des=prenote_gdt_entry();
    Threads[tid].task=newprocess;  
    Threads[tid].pid=newprocess->pid;
    p=&(Threads[tid].tss);
    if (newprocess->level==User_Level) {
      GDT[Threads[tid].tss_des]=PackDescriptor((dword) p,sizeof(*p),
					       SD_Type_Available_TSS_32,3,SD_Present,
					       SD_Size_32);
    } else { /* SuperVisor level */
      GDT[Threads[tid].tss_des]=PackDescriptor((dword) p,sizeof(*p),
					       SD_Type_Available_TSS_32,0,SD_Present,
					       SD_Size_32);
    }

  }
  p->CR3=(dword)(newprocess->PageDir);
  p->CS=KERNEL_CODE_SEGMENT_SEL;
  p->DS=KERNEL_DATA_SEGMENT_SEL;
  p->ES=KERNEL_DATA_SEGMENT_SEL;
  p->FS=KERNEL_DATA_SEGMENT_SEL;
  p->GS=KERNEL_DATA_SEGMENT_SEL;
  p->SS=KERNEL_DATA_SEGMENT_SEL;
  p->link=0;
  
  p->EIP=(dword)(&&figlio);
  __asm__ __volatile__ ("":"=a" (p->EAX),
	   "=b" (p->EBX),
	   "=c" (p->ECX),
	   "=d" (p->EDX):);
  __asm__ __volatile__ ("mov %%ebp, %%eax\n"
	   "mov %%esp, %%ebx\n"
	   "mov %%esi, %%ecx\n"
	   "mov %%edi, %%edx\n"
	   :"=a" (p->EBP),
	   "=b" (p->ESP),
	   "=c" (p->ESI),
	   "=d" (p->EDI):);

  set_EFLAGS(oldflags);
  __asm__ __volatile__ ("pushfl\n"
	   "pop %%eax\n":"=a"(p->EFLAG):);

  newprocess->status=PROCESS_STOPPED;
  Add_Thread_To_CodaPronti(newprocess->threads[0]);
  return newprocess->pid;
 figlio:
  return 0;

}

dword fake_stack[1024];


void Exit_Thread() {
  int jj;
  __asm__ __volatile__ ("mov %%eax,%%esp"::"a" (&(fake_stack[1000])));

  if (Thread_In_Esecuzione==kernel_thread)
    panic("Cannot exit from kernel thread!!!\n");;
  if (Thread_In_Esecuzione==THREAD_INVALID)
    panic("EXIT_THREAD: Exit THREAD requested with no running thread\n");
  printk("EXIT_THREAD req for tid = %d pid = %d\n",Thread_In_Esecuzione,
	 Threads[Thread_In_Esecuzione].task->pid);
  Bool_P(&(Processo_In_Esecuzione->Mutex));
  for (jj=0;jj<MAX_THREADS_PER_PROCESS;jj++) {
    if (Processo_In_Esecuzione->threads[jj]==Thread_In_Esecuzione) {
      Processo_In_Esecuzione->threads[jj]=THREAD_INVALID;
      break;
    }
  }
  Bool_V(&(Processo_In_Esecuzione->Mutex));
  if (jj==MAX_THREADS_PER_PROCESS) panic("Thread not found in process' list!!\n");
  Remove_Thread_From_CodaPronti(Thread_In_Esecuzione);
  Threads_Number--;
  Threads[Thread_In_Esecuzione].active=0;


  /* Setting up fake stack */
  //  __asm__ __volatile__ ("mov %%eax,%%esp"::"a" (&(FAKESTACK[49])));

  if (Threads[Thread_In_Esecuzione].kernel_stack!=0) {
    Linear_free(Threads[Thread_In_Esecuzione].task,Threads[Thread_In_Esecuzione].kernel_stack,THREAD_KERNEL_STACKS_SIZE);
    Threads[Thread_In_Esecuzione].kernel_stack=0;
  }
  if (Threads[Thread_In_Esecuzione].user_stack!=0) {
    Linear_free(Threads[Thread_In_Esecuzione].task,Threads[Thread_In_Esecuzione].user_stack,THREAD_USER_STACKS_SIZE);
    Threads[Thread_In_Esecuzione].user_stack=0;
  }

  free_gdt_entry(Threads[Thread_In_Esecuzione].tss_des);

  Thread_In_Esecuzione=THREAD_INVALID;
  Processo_In_Esecuzione=NULL;
  Schedule_Next_Thread();
  panic ("Exit Thread returned!");
}


void Exit_Process(void) {
  pid_t processo;
  PDE * pagedir;
  PTE * pinko;
  int i,t;
  // This should close all the resources

  __asm__ __volatile__ ("mov %%eax,%%esp"::"a" (&(fake_stack[1000])));

  cli();

  //  printk("Exit Process():\n");
  processo=Processo_In_Esecuzione;

  pagedir=(PDE *)processo->PageDir;

  /* Destroy Message Queue */

  //  printk("Fake Stack:\n");
  //__asm__("mov %%eax,%%esp\n"::"a" (((dword)fake_stack)+512));
  //  printk("Fake Stack Created:\n");

  for (i=0;i<MAX_THREADS_PER_PROCESS;i++) {
    if ((processo->threads[i]!=THREAD_INVALID) && (processo->threads[i]!=Thread_In_Esecuzione)) {
      //printk("Destroy thread with %d\n",processo->threads[i]);
      Destroy_Thread(processo->threads[i]);
      processo->threads[i]=THREAD_INVALID;
    }
  }

  //  printk("Bool_P(&(processo->Msg_Sem) in exit_process\n");

  Bool_P(&(processo->Msg_Sem));
  for (i=processo->msg_head;i!=processo->msg_tail;i=(i+1)%MAX_MESSAGES)
    freepage((page*)processo->msgqueue[i].pagina_fisica);
  freepage((page*)processo->msgqueue);
  Bool_V(&(processo->Msg_Sem));

  //  printk("Bool_P(&(processo->Mem_Sem) in exit_process\n");
  //  Bool_P(&(processo->Mem_Sem));
  //  printk("dopo Bool_P(&(processo->Mem_Sem) in exit_process\n");
  /* Da rifare i conti, perche' sono di sicuro scazzati */

  Remove_Thread_From_CodaPronti(Thread_In_Esecuzione);

  if (Threads[Thread_In_Esecuzione].kernel_stack!=0) { 
    Linear_free(Threads[Thread_In_Esecuzione].task,Threads[Thread_In_Esecuzione].kernel_stack,THREAD_KERNEL_STACKS_SIZE);
    Threads[Thread_In_Esecuzione].kernel_stack=0;
  }
  if (Threads[Thread_In_Esecuzione].user_stack!=0) {
    Linear_free(Threads[Thread_In_Esecuzione].task,Threads[Thread_In_Esecuzione].user_stack,THREAD_USER_STACKS_SIZE);
    Threads[Thread_In_Esecuzione].user_stack=0;
  }

  for (t=31;t<1024;t++) {
    if (pagedir[t].Present==1) {
      pinko=(PTE *)((pagedir[t].Page_Table_Base_Addr)<<12);
      for (i=0;i<1024;i++) {
	if (pinko[i].Present==1) {
	  freepage((page *)((pinko[i].Page_Base_Addr)<<12));
	  pinko[i].Present=0;
	}
      }
      freepage((page *)(pinko));
      pagedir[t].Present=0;
    }
  }
  freepage((page *)pagedir);
  //  Bool_V(&(processo->Mem_Sem));

  //asm__ ("lldt %%ax"::"a" (0));
  //asm__ ("ltr %%ax\n"::"a" (Dummy_tss_des<<3));
  //  free_gdt_entry(processo->processo.ldt_des);


  //  printk("End Exit Process:\n");
  //sti();
  processo->active=0;
  Numero_Processi--;
  

  Threads_Number--;
  Threads[Thread_In_Esecuzione].active=0;
  free_gdt_entry(Threads[Thread_In_Esecuzione].tss_des);

  Processo_In_Esecuzione=NULL;
  Thread_In_Esecuzione=THREAD_INVALID;

  Schedule_Next_Thread();
  panic("Exit Process: Unreachable point 2 reached!");
}


