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
 * file : kernel/kernel.c                                                  *
 * Main code ed entry point (_start)                                       *
 ***************************************************************************


 ***************************************************************************
 * $Id: kernel.c,v 1.37 2001/08/15 20:06:36 iurlano Exp $ 
 ***************************************************************************
 *
 * $Log: kernel.c,v $
 * Revision 1.37  2001/08/15 20:06:36  iurlano
 * Added DMA management functions into the kernel and library
 *
 * Revision 1.36  2001/05/14 21:33:13  iurlano
 * First work on smp systems (thanks to smpbochs)
 *
 * Revision 1.35  2001/05/14 15:45:57  iurlano
 * Work on SMP
 *
 * Revision 1.34  2001/04/24 17:20:09  iurlano
 * fat now works.
 * Warning: instable snapshot
 * random errors launchi and forking processes
 * now the shell launches commands (only in the root dir)
 *
 * Revision 1.33  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.32  2001/04/09 15:23:59  iurlano
 * BOCHS work
 *
 * Revision 1.31  2001/04/09 14:10:53  iurlano
 * Added BOCHS feature
 * Corrected a bug iin process.h
 *
 * Revision 1.30  2001/04/05 21:06:13  iurlano
 * Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
 *
 * Revision 1.29  2001/04/05 14:10:54  iurlano
 * xconfig changes
 *
 * Revision 1.28  2001/04/04 11:11:08  iurlano
 * *** empty log message ***
 *
 * Revision 1.27  2001/04/03 21:34:44  iurlano
 * Dynamic semaphores work in progress
 * Little changes in make xconfig
 * Added and changed in the code the value of THREAD_INVALID from 0 to -1
 *
 * Revision 1.26  2001/04/03 07:56:14  iurlano
 * Dynamic Semaphores. Alpha code.
 *
 * Revision 1.25  2001/04/02 15:26:15  iurlano
 * Working version with only 40 max processes, due to memory conflicts
 *
 * Revision 1.24  2001/03/23 21:05:49  iurlano
 *
 * I've finally found the bug! it is in process.c in init_process_structures
 * the problem is the for that initializes the mutexes of the processes
 * I can't figure why it is wrong, yet.
 *
 * Revision 1.23  2001/03/16 19:22:59  iurlano
 * Added something to the make xconfig prog
 * Now OSSO runs on plex86
 *
 * BIG PROBLEM. OSSO gives random errors about finding INIT and relocating
 *   and doesn't start
 *
 * Revision 1.22  2001/03/05 23:02:40  iurlano
 * Some corrections
 * Little modifies to Makefile
 *
 * Revision 1.21  2001/02/15 21:04:31  iurlano
 * Restore keyboard functionality (expecting MarcoPex to finish his work)
 * Some Bug fixes.
 *
 * Revision 1.20  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.19  2000/12/07 14:09:43  iurlano
 * Debug in progress
 *
 * Revision 1.18  2000/12/07 12:33:56  iurlano
 * Thread debug phase
 *
 * Revision 1.17  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.16  2000/12/05 16:36:01  iurlano
 * Thread managementwork in progress
 *
 * Revision 1.15  2000/12/04 17:47:55  marcopex
 * Event handling started
 *
 * Revision 1.14  2000/11/30 22:47:15  iurlano
 * Changed gdt handling.
 * Removed LDT for each task and added a global LDT for USER LEVEL tasks
 *
 * Revision 1.13  2000/11/28 15:14:52  iurlano
 * Minor changes
 *
 * Revision 1.12  2000/11/26 23:34:59  iurlano
 * work in progress on DMA memory
 *
 * Revision 1.11  2000/11/24 18:25:54  iurlano
 * Added first try of a DMA memory management. Yet to be tested
 *
 * Revision 1.10  2000/11/06 23:46:45  iurlano
 * some minor changes about init loading
 *
 * Revision 1.9  2000/11/03 18:00:29  iurlano
 * *** empty log message ***
 *
 * Revision 1.8  2000/11/02 23:34:13  iurlano
 * Dynamic linking seems to work!!
 * Some problems at boot stage with files with extension
 *
 * Revision 1.7  2000/10/29 21:09:02  iurlano
 * Working on ELF and Dynamic Loader
 *
 * Revision 1.6  2000/10/25 19:59:39  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
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
 * Revision 1.1.1.1  2000/05/30 08:11:11  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#define __PRINT_MODULE_NAME__ "Kernel"

#include <kernel.h>
#include <sys.h>
#include <sysinfo.h>
#include <interrupts.h>
#include <hd.h>
#include <parttable.h>
#include <string.h>
#include <gdt.h>
#include <printk.h>
#include <process.h>
#include <paging.h>
#include <memory.h>
#include <panic.h>
#include <semaphore.h>
#include <syscalls.h>
#include <PnP.h>
#include <fixedmemory.h>
#include <process_status.h>
#include <chardevs.h>
#include <dma_memory.h>
#include <events.h>
#include <coda_pronti.h>
#include <scheduler.h>
#include <smp.h>
#include <dma.h>


#include "../Config_Global.h"
#include "../Config_Kernel.h"


//process * kernel_process;
pid_t pid_kernel;
Thread_idx kernel_thread;

extern Partition Partitions[4];
extern dword free_pages;
extern char *LD_Elf_Ptr;

extern int testa_CodaPronti;
extern int coda_CodaPronti;


dword Dummy_tss_des;
TSS_32 Dummy_tss;
int isdebug=1;
char data[512];

int main(void)
{
  int k;
  char *modpointer;
  pid_t test_pr;
  TSS_32 *kerneltss;
  int dim, ciclomoduli, nmoduli, namelen; /* used for loading modules */


  check_memory_size();

#ifdef BOCHS_INVESTIGATE 
  {
    dword * ptr;
    //printk("BOCHS!!!\n");
    ptr=(dword*)0x1900;
    *ptr=(dword)(&Numero_Processi);
    ptr=(dword*)0x1908;
    *ptr=(dword)&(Processi[0]);

  }
#endif

 
  get_initial_cursor_pos();

  /* until here, BIOS data area is valid */
  cli();
  init_gdt(256);
  __asm__ __volatile__ ("movl $16,%%eax\n"
			"movw %%ax,%%ds\n"
			"movw %%ax,%%es\n"
			"movw %%ax,%%fs\n"
			"movw %%ax,%%gs\n":::"ax");
  /*
    printk("WELCOME TO :\n\n");
    printk("      OOOOOOOOOOOO    SSSSSSSSSSSSSSSSSSSSSSSSOOOOOOOOOOOOOOOO\n");
    printk("      O          O    S                           O          O\n");
    printk("      O          O    S             SSSSSSSSSS    O          O\n");
    printk("      O          O    S             S             O          O\n");
    printk("      O          O    S             S             O          O\n");
    printk("      O          O    SSSSSSSSSS    S             O          O\n");
    printk("      O          O             S    SSSSSSSSSS    O          O\n");
    printk("      O          O             S             S    O          O\n");
    printk("      O          O             S             S    O          O\n");
    printk("      O          O    SSSSSSSSSS             S    O          O\n");
    printk("      O          O                           S    O          O\n");
    printk("      OOOOOOOOOOOOOOOOSSSSSSSSSSSSSSSSSSSSSSSS    OOOOOOOOOOOO\n\n");
  */
  printk("Calling Init functions\n");

  init_Events();
  init_IDT_table();
  init_Process_Structures();

  Processo_In_Esecuzione=NULL;
  Thread_In_Esecuzione=THREAD_INVALID;
  /* KERNEL TASK INIT */


  //  get_BIOS_geometry();
  meminit();
  DMA_Memory_Init();
  InitDMA();
  init_paging();

  printk("Preparing Kernel Task\n");

  Dummy_tss_des=prenote_gdt_entry();
  //  k=prenote_gdt_entry();

  pid_kernel=Get_Free_Process_Slot();

  //memset(kernel_process,0,sizeof(*kernel_process));

  cli();
  //  kernel_process->ldt_des=k;
  for (k=0;k<MAX_THREADS_PER_PROCESS;k++) pid_kernel->threads[k]=THREAD_INVALID;
  kernel_thread=Get_Free_Thread_Slot();
  printk("Kernel Thread = %d Kernel Pid = %d\n",kernel_thread,pid_kernel->pid);
  pid_kernel->threads[0]=kernel_thread;
  Threads[kernel_thread].tss_des=KERNEL_TSS_SEGMENT_DES;


  /*
    GDT[kernel_process->ldt_des]=
    PackDescriptor((dword) (&(kernel_process->LDT[0])),
    PROCESS_LDT_ENTRIES*sizeof(SegmentDes),
    SD_Type_LDT,0,SD_Present,SD_Size_32);
  */
  GDT[Threads[kernel_thread].tss_des]=PackDescriptor((dword) &(Threads[kernel_thread].tss),
						     sizeof(Threads[kernel_thread].tss),
						     SD_Type_Available_TSS_32,0,
						     SD_Present,SD_Size_32);

  GDT[Dummy_tss_des]=PackDescriptor((dword) &Dummy_tss,
				    sizeof(Dummy_tss),
				    SD_Type_Available_TSS_32,3,
				    SD_Present,SD_Size_32);
  kerneltss=&(Threads[kernel_thread].tss);

  kerneltss->SS1=0;
  kerneltss->ESP1=0;
  kerneltss->SS2=0;
  kerneltss->ESP2=0;
  //  kernel_process->tss.LDT=(kernel_process->ldt_des)<<3;
  kerneltss->LDT=0;


  kerneltss->CS=KERNEL_CODE_SEGMENT_SEL;
  kerneltss->DS=KERNEL_DATA_SEGMENT_SEL;
  kerneltss->ES=KERNEL_DATA_SEGMENT_SEL;
  kerneltss->FS=KERNEL_DATA_SEGMENT_SEL;
  kerneltss->GS=KERNEL_DATA_SEGMENT_SEL;
  kerneltss->SS=KERNEL_DATA_SEGMENT_SEL;
  kerneltss->ESP=(dword)KERNEL_STACK_TOP;
  kerneltss->SS0=KERNEL_DATA_SEGMENT_SEL;
  kerneltss->ESP0=(dword)KERNEL_STACK_TOP;

  pid_kernel->signals_pending=0;
  pid_kernel->sig_head=0;  pid_kernel->sig_tail=0;

  kerneltss->CR3=(dword)Kernel_Page_Dir;
  Processo_In_Esecuzione=pid_kernel;
  Thread_In_Esecuzione=kernel_thread;
  Threads[kernel_thread].task=pid_kernel;
  pid_kernel->active=1;
  pid_kernel->level=SuperVisor_Level;

  Init_Bool_Semaphore(&(pid_kernel->Msg_Sem),1);
  Init_Bool_Semaphore(&(pid_kernel->Sig_Sem),1);
  Init_Bool_Semaphore(&(pid_kernel->Mem_Sem),1);
  Init_Semaphore(&(pid_kernel->Messages),0);

  Processo_In_Esecuzione->last_used_page=0x8000000;
  Processo_In_Esecuzione->status=PROCESS_RUN;
  __asm__ __volatile__ ("ltr %%ax\n"::"a" (KERNEL_TSS_SEGMENT_SEL));

  /* Setting cr0 */
  /* 10000000000000000000000000110001 b */
  __asm__ __volatile__ ("mov %%eax,%%cr0"::"a" (0x80000031));

  /* INTERRUPT ENABLING */

  unmask_IRQ(IRQ_1|IRQ_E|IRQ_F);

  mask_IRQ(IRQ_0);
  set_IRQ_mask(1);

  //  printk("\nFree Memory : %u (%ukb)\n",get_free_memory(),
  // 	 get_free_memory()/1024);

  //  Check_PnP();
  //  Check_BIOS32();
  initchardevs();

  /*  HD TEST CODE  */

  /*
    check_hd();

    if (!read_sector(0,0,data)) printk("EEERRRORRE\n");
    printk("Inspecting bootable flag : %x\n",data[255]);

    read_partition_table((byte *)data);
    print_partition_table();

    printk("Reading fat parameters from first partition\n");

    get_FAT_parameters(Partitions[0].relative_start_sector);
    print_root_dir(Partitions[0].relative_start_sector);

  */

  //cli();
  //  printk("Main Address : %x\n",main);

  /* TEST DI INVALID OPCODE */
  // __asm__ ("UD2");
  /* TEST DI DIVISION BY ZERO*/
  //pid_A=1/0;
  //    cli();

  printk("Memory size : %uk\n",memory_size>>10);

  cli();
  testa_CodaPronti=0;
  coda_CodaPronti=0;
#ifdef SMP_ENABLE
  if (Check_SMP()) {
    printk("SMP Found %d processors (Bootstrap is %d)\n",Num_Processors,BootStrap_Processor);
  }
#endif
  printk("Kernel initialization finished. Launching modules ...\n");



  /*
    {
    char * dada;
    char pio[80]="VEdiAMO una pochetta se funziona.\n";
    int i;
    i=0;
    dada=0x10000;
    while (pio[i]) dada[i]=pio[i++];
    pio[i]='\0';
    dada=0x10000;

    printk("Scritto : %s\n",dada);
    }
  */

  //  printk("Free pages : %d\n",free_pages);
  //  printk("Reading modules ....\n");
  // modpointer=(char *)0x50000;
  // printk("I've found  %d modules\n",(int)*modpointer);
  printk(" * Address for address of modules in 0x%x\n",
	 GENERAL_PARAMETERS_ADDRESS);
  modpointer=(char *)(*((void **)GENERAL_PARAMETERS_ADDRESS));
  printk(" * Searching for modules in 0x%x\n",modpointer);
  nmoduli=*((int*)modpointer);
  printk(" * Number of modules: %d\n",nmoduli);
  modpointer+=4;
  printk(" * Modules start: %x\n",modpointer);
  printk("Searching for libOSSO.so ... ");
  k=0;
  for(ciclomoduli=0; ciclomoduli<nmoduli; ciclomoduli++){
    //int i;
    //    printk("  Launching module ");
    namelen=*modpointer;
    modpointer+=4;
    dim=*((word*)(modpointer+namelen));
    /*      printk("Scanning '"); */
    /*      for (i=0;i<11;i++) _printk("%c",modpointer[i]); */
    /*      _printk("'\n"); */
    //  printk("DBG> modulo: <%s>\n",modpointer);
    if ((strcasecmp("libOSSO.so",modpointer))==0) {
      //      _printk("libOSSO.so found\n");
      modpointer+=namelen+4;
      _printk("libOSSO.so found at 0x%x\n",modpointer);
      LD_Elf_Ptr=(char*)modpointer;
      k=1;
      break;
    }
    modpointer+=namelen+4;
    modpointer+=dim;
  }

  printk("Size Of Processi %d bytes (%dk)\n",sizeof(Processi),sizeof(Processi)/1024);
  printk("Size Of a Process %d bytes (%dk)\n",sizeof(Processi[0]),sizeof(Processi[0])/1024);
  printk("Size Of Threads %d bytes (%dk)\n",sizeof(Threads),sizeof(Threads)/1024);
  printk("Size Of a Thread %d bytes (%dk)\n",sizeof(Threads[0]),sizeof(Threads[0])/1024);
  printk("Size Of a Semaphore %d bytes (%dk)\n",sizeof(Semaphore),sizeof(Semaphore)/1024);
  /*
    cli();
    __asm__ ("hlt");
    while (1) ;
  
  */

  if (k==0) _printk("libOSSO.so not found!!!\n");
  k=0;
  modpointer=((char *)(*((void **)GENERAL_PARAMETERS_ADDRESS)))+4;
  printk("Searching for INIT ... ");
  for(ciclomoduli=0; ciclomoduli<nmoduli; ciclomoduli++){
    //    printk("  Launching module ");
    namelen=*modpointer;
    modpointer+=4;
    dim=*((word*)(modpointer+namelen));
    //  printk("DBG> modulo: <%s>\n",modpointer);
    if ((strcasecmp("init",modpointer))==0) {
      //      _printk("INIT found\n");
      modpointer+=namelen+4;
      _printk("INIT found at 0x%x\n",modpointer);
      test_pr=Create_Process_From_ELF(modpointer,SuperVisor_Level);
      if (test_pr==NULL) {
	printk("Error launching INIT. OSSO halted\n");
	cli();
	__asm__ __volatile__ ("hlt");
	while (1) ;
      }
      printk("Launching INIT with pid %d (tid == %d)\n",test_pr->pid,(test_pr->threads)[0]);
      Add_Thread_To_CodaPronti((test_pr->threads)[0]);
      k=1;
      break;
    }
    modpointer+=namelen+4;
    modpointer+=dim;
  }
  if (k==0) printk("INIT not found!!!\n");

  /*
    printk("Sizeof TSS : %d\n",sizeof(TSS_32));
    printk("Sizeof task : %d\n",sizeof(task));
    printk("Sizeof Processi : %d\n",sizeof(Processi));
    printk("Sizeof Semaforo : %d\n",sizeof(Semaphore));
  */
  //  printk("Free pages after processes' creation : %d\n",free_pages);
  //  printk("Enabling interrupts\n");
  printk("Entering Infinite loop\n");
  set_IRQ_mask(0);

  /* Kernel now becomes the idle process */
  while (1) {
    sti();
    if (coda_CodaPronti==testa_CodaPronti) {
      /* Idle is the only thread active */
      __asm__ __volatile__ ("hlt");
    } else Schedule_Next_Thread();
  }
  panic("Endless loop has ended!!\n");
  return 0;
}

void _start(void) {
  //reset_free_mem();
  main();
  panic("Endless loop has ended 2 !!\n");
}
