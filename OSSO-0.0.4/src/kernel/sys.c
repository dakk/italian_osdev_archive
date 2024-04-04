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
 * file : kernel/sys.c                                                     *
 * Functions defined in sys.h for processor management                     *
 ***************************************************************************

 ***************************************************************************
 * $Id: sys.c,v 1.6 2001/04/11 15:26:32 iurlano Exp $  
 ***************************************************************************
 *
 * $Log: sys.c,v $
 * Revision 1.6  2001/04/11 15:26:32  iurlano
 * Thread and processes termination work
 * Memory leak
 *
 * Revision 1.5  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.4  2001/03/06 11:50:26  iurlano
 * Added functions for smp multitasking (not yet tested)
 *
 * Revision 1.3  2001/03/05 23:02:40  iurlano
 * Some corrections
 * Little modifies to Makefile
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
 * Revision 1.7  2000/01/04 00:56:50  iurlano
 * Correzioni
 *
 * Revision 1.6  1999/12/30 15:33:00  iurlano
 * Sistemati processi
 *
 * Revision 1.5  1999/11/25 11:22:32  iurlano
 * Pulizia e riordino
 *
 * Revision 1.4  1999/10/18 07:57:26  iurlano
 * Primo multitask funzionante
 * Aggiunto settaggio del segmento dati del kernel
 * 
 * Revision 1.3  1999/10/14 15:38:39  iurlano 
 * Aggiunta costante per il flag NT 
 * Aggiunte funzioni per gestione di EFLAGS 
 * TEST di Task Switch. Non funziona. 
 *
 * Revision 1.2  1999/10/14 14:24:53  iurlano 
 * Task Switch Test in corso 
 *
 ***************************************************************************/



#include <sys.h>
#include <process.h>
SegmentDes PackDescriptor(dword Base_Address,dword Segment_Length,
			  int type,int dpl,
			  int flags, int access)
{
  SegmentDes SD;
  SD.base_addr0_15=Base_Address & 0xFFFF;
  SD.base_addr16_23=(Base_Address >> 16) &0xFF;
  SD.base_addr24_31=(Base_Address >> 24) &0xFF;
  SD.flags=type | flags | (dpl<<5);
  SD.access=access|((Segment_Length >>16) &0xF);
  SD.access|=SD_OSAvail;
  SD.seg_length0_15=Segment_Length &0xFFFF;
  return SD;
}

void load_gdt(char * base,int numdes) {
  unsigned long int gdt_reg[2];
  gdt_reg[0]=numdes*sizeof(SegmentDes) <<16;
  gdt_reg[1]=(dword)base;
  __asm__ __volatile__ ("lgdt (%0)": :"g" ((char *)gdt_reg+2)); 
}

/* Loads IDT with the correct values for intnumb interrupts */
void load_idt(char * base,int numint) {
  unsigned long int idt_reg[2];
  idt_reg[0]=numint*sizeof(InterruptDesc) <<16;
  idt_reg[1]=(dword)base;
  __asm__ __volatile__ ("lidt (%0)": :"g" (((char *)idt_reg+2))); 
}


dword get_DS(void) {
  dword a;
  __asm__ ("xorl %%eax,%%eax\n"
	   "movw %%ds,%%ax\n":"=a" (a):);
  return a;
}

dword get_ES(void)  {
  dword a;
  __asm__ ("xorl %%eax,%%eax\n"
	   "movw %%es,%%ax\n":"=a"(a):);
  return a;
}

dword get_FS(void)  {
  dword a;
  __asm__ ("xorl %%eax,%%eax\n"
	   "movw %%fs,%%ax\n":"=a"(a):);
  return a;
}

dword get_GS(void)  {
  dword a;
  __asm__ ("xorl %%eax,%%eax\n"
	   "movw %%gs,%%ax\n":"=a"(a):);
  return a;
}

dword get_SS(void)  {
  dword a;
  __asm__ ("xorl %%eax,%%eax\n"
	   "movw %%ss,%%ax\n":"=a"(a):);
  return a;
}

void set_DS(word a) {
  //  register word b __asm__ ("ax");
  //  b=a;
  __asm__ ("movw %%ax,%%ds"::"a"(a));
}
void set_ES(word a) {
  //  register word b __asm__ ("ax");
  //  b=a;
  __asm__ ("movw %%ax,%%es"::"a"(a));
}
void set_FS(word a) {
  //  register word b __asm__ ("ax");
  //  b=a;
  __asm__ ("movw %%ax,%%fs"::"a"(a));
}

void set_GS(word a) {
  //  register word b __asm__ ("ax");
  //  b=a;
  __asm__ ("movw %%ax,%%gs"::"a"(a));
}

dword get_EFLAGS(void)  {
  dword a;
  __asm__ ("pushf\n"
	   "pop %%eax\n":"=a"(a));
  return a;
}


void set_EFLAGS(dword a) {
  //  register dword b __asm__ ("eax");
  //  b=a;
  __asm__ ("push %%eax\n"
	   "popf"::"a"(a));
}


void call_tss(word tss_des)
{
  dword tt[2];
  tt[0]=0;
  tt[1]=(tss_des)<<3;
  __asm__ ("lcall (%0)"::"g" (tt));
  //Should be  __asm__ ("lcall %%ss:(%%edi)"::"D" (tt));
  //But in flat 32 mode they are the same
}

void jump_tss(word tss_des)
{
  dword tt[2];
  tt[0]=0;
  tt[1]=(tss_des)<<3;
  __asm__ ("ljmp (%0)"::"g" (tt));  
}

