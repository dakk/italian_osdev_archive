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
 * file : kernel/interrupts.c                                              *
 * Interrupts initialization code & interrupt handlers                     *
 ***************************************************************************
 
 
 ***************************************************************************
 * $Id: interrupts.c,v 1.20 2001/08/23 19:21:11 iurlano Exp $
 ***************************************************************************
 *
 * $Log: interrupts.c,v $
 * Revision 1.20  2001/08/23 19:21:11  iurlano
 * Changed IDE to publish correctly each partition, whole disk included.
 * Added mount point support to vfs. Extended its protocol.
 * Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
 * Now it is possible to mount a partition and actually see it int the FS!!
 * Works!
 * Changed fs.c in libOSSO to use the vfs for dir and file access.
 * Changed mount utility to inspect partitions and mount points state.
 * Added Documentation for FAT driver
 *
 * Revision 1.19  2001/08/13 23:55:28  iurlano
 * FDC Work in progress, but needs DMA
 * DMA work started.
 * Some warnings fixed in kernel code
 *
 * Revision 1.18  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.17  2001/06/24 22:18:12  iurlano
 * Optimized process creation functions
 * Added programs to the test section in xconfig
 * Added some functions to kernel-loader.c
 * Page faults now terminate the running process
 * Bugfixed memory leak in program execution from the shell
 *
 * Revision 1.16  2001/06/17 20:52:44  iurlano
 * *** empty log message ***
 *
 * Revision 1.15  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.14  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.13  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.12  2001/04/23 14:43:05  iurlano
 * work on FAT module
 * work on FS protocol
 *
 * Revision 1.11  2001/04/19 15:17:58  iurlano
 * Fixed some bug
 * changed ide driver
 * changed fat driver (cabled the pid of the ide driver)
 * changed shell( now accept 3 commands mem, info, ls) TRY IT
 * VERY STABLE SNAPSHOT
 *
 * Revision 1.10  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.9  2001/03/16 19:22:59  iurlano
 * Added something to the make xconfig prog
 * Now OSSO runs on plex86
 *
 * BIG PROBLEM. OSSO gives random errors about finding INIT and relocating
 *   and doesn't start
 *
 * Revision 1.8  2001/02/15 21:04:31  iurlano
 * Restore keyboard functionality (expecting MarcoPex to finish his work)
 * Some Bug fixes.
 *
 * Revision 1.7  2000/12/07 12:50:50  marcopex
 * *** empty log message ***
 *
 * Revision 1.6  2000/12/07 12:33:56  iurlano
 * Thread debug phase
 *
 * Revision 1.5  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.4  2000/12/04 17:47:55  marcopex
 * Event handling started
 *
 * Revision 1.3  2000/11/30 22:47:15  iurlano
 * Changed gdt handling.
 * Removed LDT for each task and added a global LDT for USER LEVEL tasks
 *
 * Revision 1.2  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:09  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.2  2000/02/18 10:05:52  iurlano
 * Aggiunto codice PCI iniziale
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.22  2000/01/16 22:43:32  iurlano
 * Aggiunta tabella dei drivers e relative funzioni.
 * da testare
 *
 * Revision 1.21  2000/01/14 23:43:49  iurlano
 * aggiunti prototipi di drivers per seriale e ide
 *
 * Revision 1.20  2000/01/13 13:43:46  iurlano
 * *** empty log message ***
 *
 * Revision 1.19  2000/01/06 17:00:03  iurlano
 * Aggiunto albero directory 'drivers'
 * Aggiunte alcune syscalls
 *
 * Revision 1.18  2000/01/05 00:04:48  iurlano
 * Other bugfixes....
 *
 * Revision 1.17  2000/01/04 18:10:16  iurlano
 * Interrupts reordering and bug fix
 *
 * Revision 1.16  1999/12/30 15:33:00  iurlano
 * Sistemati processi
 *
 * Revision 1.15  1999/11/29 23:24:28  iurlano
 * Secondo tentativo di gestione di segnali.
 * Sembra funzionare
 *
 * Corretto bug nella gestione delle interruzioni.
 * Aggiunta syscall per settare un signal handler
 *
 * Revision 1.14  1999/11/28 23:21:33  iurlano
 * primo tentativo di una gestione dei segnali
 * Aggiunto signal.c
 *
 * Revision 1.13  1999/11/25 11:22:29  iurlano
 * Pulizia e riordino
 *
 * Revision 1.12  1999/11/23 10:41:51  iurlano
 * Pulizia e riordino...
 *
 * Revision 1.11  1997/01/24 04:35:31  iurlano
 * Completata prima fase di Create_Process_From_ELF che fa
 * caricamento processi da formato elf in esecuzione
 *
 * Revision 1.10  1999/11/18 23:15:22  iurlano
 * Aggiunti interrupt handlers in assembler.
 *
 * Revision 1.9  1997/01/17 05:25:24  iurlano
 * Corretto parte di memory.c per la gestione delle pagine.
 * Tolto malloc() dal codice sostituendola con allocazioni statiche.
 *   E' da togliere da fat.c che per ora e' inutilizzabile.
 * Cambiata allocazione dei registri per syscall.
 * Aggiunte alcune syscall
 *
 * Revision 1.8  1999/11/04 17:43:42  iurlano
 * Aggiunto handler in assembler di syscall. Non funziona
 *
 * Revision 1.7  1999/10/27 21:58:06  iurlano
 * Aggiunto modulo per Pnp (e' vuoto per ora)
 *
 * Revision 1.6  1999/10/21 15:39:00  iurlano
 * Aggiustamento commenti
 * Aggiunta header e .c per la paginazione
 *
 * Revision 1.5  1999/10/20 08:05:55  iurlano
 * Aggiunta stampa a video dell'indirizzo dell'istruzione che genera
 * errore nel caso di divisione per zero o Opcode invalido
 *
 * Revision 1.3  1999/10/14 15:38:37  iurlano
 * Aggiunta costante per il flag NT
 * Aggiunte funzioni per gestione di EFLAGS
 * TEST di Task Switch. Non funziona.
 *
 * Revision 1.2  1999/10/14 14:24:52  iurlano
 * Task Switch Test in corso
 *
 ***************************************************************************/



#include <kernel.h>
#include <sys.h>
#include <io.h>
#include <printk.h>
#include <gdt.h>
#include <process.h>
#include <fixedmemory.h>
#include <semaphore.h>
#include <panic.h>
#include <events.h>


//#define DEBUG_PRINT_IRQ

//extern void IRQ1(void);      /* Defined in keyboard.c */
//extern void scheduler(void); /* Defined in scheduler.c */

/* EXTERNAL DATA STRUCURES */
/* Array of public events */
extern Event_struct Public_Events [NUM_PUBLIC_EVENTS];
extern Bool_Semaphore Events_Sem; /* Semaphore for concurrent use of public events */


extern void Syscall(void); /* Defined in syscall.c */

void Int_0(word SEG)
 {
  PRINTS("Division By Zero at 0x");
  PRINTX(SEG);
  PRINTS(" : 0x");
  PRINTX((dword)__builtin_return_address (0));
  PRINTC('\n');
  sti();
  while (1) {
    asm("hlt");
  };
}

void Int_1(word SEG) {
  PRINTS("INTERRUPT: Process ");
  PRINTD(Processo_In_Esecuzione->pid);
  PRINTS(" Thread ");
  PRINTD(Thread_In_Esecuzione);
  PRINTS(" caused Single Step at 0x");
  PRINTX(SEG);
  PRINTS(" : 0x");
  PRINTX((dword)__builtin_return_address (0));
  PRINTC('\n');
  while (1) ;
  //sti();
  //asm("hlt");
}

void Int_2(void) {
  PRINTS("INTERRUPT: NMI !!!!!\n");
  while (1) ;
}

void Int_3(void) {
  PRINTS("INTERRUPT: Breakpoint\n");
  while (1) ;
}


void Int_4(void) {
  PRINTS("INTERRUPT: Overflow\n");
  while (1) ;
}

void Int_5(void) {
  PRINTS("INTERRUPT: Bound Range Exceeded\n");
  while (1) ;
}

void Int_6(word SEG) {
  PRINTS("INTERRUPT: Process ");
  PRINTD(Processo_In_Esecuzione->pid);
  PRINTS(" Thread ");
  PRINTD(Thread_In_Esecuzione);
  PRINTS(" Invalid OPCODE at 0x");
  PRINTX(SEG);
  PRINTS(" : 0x");
  PRINTX((dword)__builtin_return_address (0));
  PRINTC('\n');

  sti();
  while (1) ;
}

void Int_7(void) {
  PRINTS("INTERRUPT: Coprocessor Not Available\n");
  //  while (1) ;
  __asm__("clts");
  //sti();
  
}

void Int_8(void) {
  PRINTS("INTERRUPT: Double Fault\n");
  while (1) ;
}

void Int_9(void) {
  PRINTS("INTERRUPT: Coprocessor Segment Overrun\n");
  while (1) ;
}

void Int_A(void) {
  PRINTS("INTERRUPT: INVALID TASK STATE SEGMENT\n");
  while (1) ;
}

void Int_B(void) {
  PRINTS("INTERRUPT: SEGMENT NOT PRESENT\n");
  while (1) ;
}


void Int_C(void) {
  PRINTS("INTERRUPT: STACK FAULT\n");
  while (1) ;
}

unsigned int aa,bb;
void Int_D(dword offset,word seg) {
  dword error;
  PRINTS("INTERRUPT: Process ");
  PRINTD(Processo_In_Esecuzione->pid);
  PRINTS(" caused GENERAL PROTECTION VIOLATION at 0x");
  PRINTX(seg);
  PRINTS(" : 0x");
  PRINTX(offset); PRINTS(" (Error = 0x");
  error=(dword)__builtin_return_address (0);
  PRINTX(error);PRINTS(")");
  PRINTS("\n");
  while (1) ;
}

void Int_E(dword offset,dword seg) {
  dword addr;
  dword error;
//  dword pde;

  PRINTS("INTERRUPT: Process ");
  if (Processo_In_Esecuzione!=NULL)
    PRINTD(Processo_In_Esecuzione->pid);
  else PRINTD(0);
  PRINTS(" (tid == ");
  PRINTD(Thread_In_Esecuzione);
  PRINTS(") caused a Page Fault at 0x");
  PRINTX(seg);
  PRINTS(" : 0x");
  PRINTX(offset);
  PRINTC('\n');
  error=(dword)__builtin_return_address (0);
  if (error &1) PRINTS("Present=1, ");
  else PRINTS("Present=0, ");
  if (error &2) PRINTS("Write, ");
  else PRINTS("Read, ");
  if (error &4) PRINTS("User, ");
  else PRINTS("Supervisor, ");
  __asm__ __volatile__ ("movl %%cr2,%%eax":"=a" (addr):);
  PRINTS("Address=0x");
  PRINTX(addr);
  PRINTS("\n");
  /*
    pde=addr>>22;
    printk("Page dir entry : present=%d, %s, %s, Table base addr=%x\n",Processo_In_Esecuzione->PageDir[pde].Present,Processo_In_Esecuzione->PageDir[pde].Read_Write==1?"Write":"Read",Processo_In_Esecuzione->PageDir[pde].User==1?"User":"Supervisor",Processo_In_Esecuzione->PageDir[pde].Page_Table_Base_Addr<<12);

  */

  Exit_Process();
  sti();
  while (1) ;
}

void Int_F(void) {
  PRINTS("INTERRUPT: UNKNOWN ?????\n");
  while (1) ;
}

void Int_10(void) {
  PRINTS("INTERRUPT: Coprocessor Error\n");
  while (1) ;
}

void Int_11(void) {
  PRINTS("INTERRUPT: ALIGNMENT CHECK\n");
  while (1) ;
}

void Int_12(void) {
  PRINTS("INTERRUPT: MACHINE CHECK EXCEPTION\n");
  while (1) ;
}

void Int_13(void) {
  PRINTS("INTERRUPT: Int 13\n");
  while (1) ;
}


void Int_14(void) {
  PRINTS("INTERRUPT: Int 14\n");
  while (1) ;
}

void Int_15(void) {
  PRINTS("INTERRUPT: Int 15\n");
  while (1) ;
}

void Int_16(void) {
  PRINTS("Int 16\n");
  while (1) ;
}

void Int_17(void) {
  PRINTS("Int 17\n");
  while (1) ;
}

void Int_18(void) {
  PRINTS("Int 18\n");
  while (1) ;
}

void Int_19(void) {
  PRINTS("Int 19\n");
  while (1) ;
}

void Int_1A(void) {
  PRINTS("Int 1A\n");
  while (1) ;
}

void Int_1B(void) {
  PRINTS("Int 1B\n");
  while (1) ;
}

void Int_1C(void) {
  PRINTS("Int 1C\n");
  while (1) ;
}

void Int_1D(void) {
  PRINTS("Int 1D\n");
  while (1) ;
}

void Int_1E(void) {
  PRINTS("Int 1E\n");
  while (1) ;
}

void Int_1F(void) {
  PRINTS("Int 1F\n");
  while (1) ;
}


void IRQ1(void) {
  //  printk("IRQ 1\n");
  V(&(Public_Events[0x1].semaphore));
  eoi();
  //  printk("EOI 1\n");
}

void IRQ2(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ 2\n");
#endif
  V(&(Public_Events[0x2].semaphore));
  eoi();
}

void IRQ3(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ 3\n");
#endif
  V(&(Public_Events[0x3].semaphore));
  eoi();
}

void IRQ4(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ 4\n");
#endif
  V(&(Public_Events[0x4].semaphore));
  eoi();
}


void IRQ5(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ 5\n");
#endif
  V(&(Public_Events[0x5].semaphore));
  eoi();
}

void IRQ6(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ 6\n");
#endif
  V(&(Public_Events[0x6].semaphore));
  eoi();
}

void IRQ7(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ 7\n");
#endif
  V(&(Public_Events[0x7].semaphore));
  eoi();
}

void IRQ8(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ 8\n");
#endif
  V(&(Public_Events[0x8].semaphore));
  eoi();
}

void IRQ9(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ 9\n");
#endif
  V(&(Public_Events[0x9].semaphore));
  eoi();
}

void IRQA(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ A\n");
#endif
  V(&(Public_Events[0xA].semaphore));
  eoi();
}

void IRQB(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ B\n");
#endif
  V(&(Public_Events[0xB].semaphore));
  eoi();
}

void IRQC(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ C\n");
#endif
  V(&(Public_Events[0xC].semaphore));
  eoi();
}

void IRQD(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ D\n");
#endif
  V(&(Public_Events[0xD].semaphore));
  eoi();
}

void IRQE(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ E\n");
#endif
  V(&(Public_Events[0xE].semaphore));
  eoi();
}

void IRQF(void) {
#ifdef DEBUG_PRINT_IRQ
  _color_printk(Yellow,"IRQ F\n");
#endif
  V(&(Public_Events[0xF].semaphore));
  eoi();
}

void Undefined_Interrupt(void) {
  panic("Interrupt not defined!");
}

void set_IRQ_mask(word mask) {
  outb(mask & 0xFF,0x21);
  outb(mask >> 8,0xA1);
}  

word get_IRQ_mask(void) {
  return (((word)inb(0x21)) | ((word)(inb(0xA1))<<8));
}

void unmask_IRQ(word irq) {
  word newmask;
  newmask=get_IRQ_mask();
  newmask=newmask & (!irq);
  set_IRQ_mask(newmask);
}

void mask_IRQ(word irq) {
  word newmask;
  newmask=get_IRQ_mask();
  newmask|=irq;
  set_IRQ_mask(newmask);
}

//InterruptDesc * IDT; /* Pointer to IDT */
InterruptDesc IDT[IDT_ENTRIES]; /* IDT not a pointer. It should reside in the
				   kernel memory area in the 1st MB 
				   This is needed to maintain Real Mode 
				   interrupt table */

/* Setting contents of IDT */
void init_IDT_entry(InterruptDesc * IDT,int i,void (*handler)(void)) {
  IDT[i].offset0_15=(unsigned long int )handler & 0xFFFF;
  IDT[i].offset16_31=(unsigned long int )handler >> 16;
  IDT[i].zero=0; /* Not used for interrupt gate */
  IDT[i].flag=0x8E; /* Interrupt gate, present in memory */
  IDT[i].selector0_15=8; /* Codice 4GB */
}

extern void system_call_handler(void);
extern void irq0_handler(void);
extern void irq1_handler(void);
extern void irq2_handler(void);
extern void irq3_handler(void);
extern void irq4_handler(void);
extern void irq5_handler(void);
extern void irq6_handler(void);
extern void irq7_handler(void);
extern void irq8_handler(void);
extern void irq9_handler(void);
extern void irqA_handler(void);
extern void irqB_handler(void);
extern void irqC_handler(void);
extern void irqD_handler(void);
extern void irqE_handler(void);
extern void irqF_handler(void);


void init_IDT_table(void) {
  int i;
  //  IDT=(InterruptDesc *)IDT_BASE_ADDRESS; // Uncomment if IDT is a pointer
  /* Setting interrupt routines */
  
  /* Exceptions */
  init_IDT_entry(IDT,0,(void (*)(void))Int_0);
  init_IDT_entry(IDT,1,(void (*)(void))Int_1);
  init_IDT_entry(IDT,2,Int_2);
  init_IDT_entry(IDT,3,Int_3);
  init_IDT_entry(IDT,4,Int_4);
  init_IDT_entry(IDT,5,Int_5);  
  init_IDT_entry(IDT,6,(void (*)(void))Int_6);
  init_IDT_entry(IDT,7,Int_7);
  init_IDT_entry(IDT,8,Int_8);
  init_IDT_entry(IDT,9,Int_9);
  init_IDT_entry(IDT,0xA,Int_A);
  init_IDT_entry(IDT,0xB,Int_B);
  init_IDT_entry(IDT,0xC,Int_C);
  init_IDT_entry(IDT,0xD,(void (*)(void))Int_D);  
  init_IDT_entry(IDT,0xE,(void (*)(void))Int_E);
  init_IDT_entry(IDT,0xF,Int_F);
  init_IDT_entry(IDT,0x10,Int_10);
  init_IDT_entry(IDT,0x11,Int_11);
  init_IDT_entry(IDT,0x12,Int_12);
  init_IDT_entry(IDT,0x13,Int_13);
  init_IDT_entry(IDT,0x14,Int_14);
  init_IDT_entry(IDT,0x15,Int_15);  
  init_IDT_entry(IDT,0x16,Int_16);
  init_IDT_entry(IDT,0x17,Int_17);
  init_IDT_entry(IDT,0x18,Int_18);
  init_IDT_entry(IDT,0x19,Int_19);
  init_IDT_entry(IDT,0x1A,Int_1A);
  init_IDT_entry(IDT,0x1B,Int_1B);
  init_IDT_entry(IDT,0x1C,Int_1C);
  init_IDT_entry(IDT,0x1D,Int_1D);  
  init_IDT_entry(IDT,0x1E,Int_1E);
  init_IDT_entry(IDT,0x1F,Int_1F);

  /* IRQs */
  init_IDT_entry(IDT,0x20,irq0_handler);
  init_IDT_entry(IDT,0x21,irq1_handler);
  init_IDT_entry(IDT,0x22,irq2_handler);
  init_IDT_entry(IDT,0x23,irq3_handler);
  init_IDT_entry(IDT,0x24,irq4_handler);
  init_IDT_entry(IDT,0x25,irq5_handler);  
  init_IDT_entry(IDT,0x26,irq6_handler);
  init_IDT_entry(IDT,0x27,irq7_handler);
  init_IDT_entry(IDT,0x28,irq8_handler);
  init_IDT_entry(IDT,0x29,irq9_handler);
  init_IDT_entry(IDT,0x2A,irqA_handler);
  init_IDT_entry(IDT,0x2B,irqB_handler);
  init_IDT_entry(IDT,0x2C,irqC_handler);
  init_IDT_entry(IDT,0x2D,irqD_handler);  
  init_IDT_entry(IDT,0x2E,irqE_handler);
  init_IDT_entry(IDT,0x2F,irqF_handler);
  for (i=0x30;i<0x50;i++)
    init_IDT_entry(IDT,i,Undefined_Interrupt);
  init_IDT_entry(IDT,0x40,system_call_handler);
  for (i=0;i<0x50;i++)
    IDT[i].flag|=0x60;

  /* Loading IDT in IDT register */
  load_idt((char *)IDT,0x50);
  
  printk("IDT loaded at %x\n",IDT);
  
  /* end */
}
