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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/sys.h,v $
 * System definitions and constants and some assembler macros
 ***************************************************************************/


/***************************************************************************/
/* $Id: sys.h,v 1.2 2001/04/28 12:15:53 steros Exp $ */
/***************************************************************************/
/*
 * $Log: sys.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.5  2001/04/11 15:26:32  iurlano
 * Thread and processes termination work
 * Memory leak
 *
 * Revision 1.4  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.3  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.2  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.7  1999/11/28 23:21:33  iurlano
 * first try of signal handling
 * Added signal.c
 *
 * Revision 1.4  1999/10/18 07:56:45  iurlano
 * First working multitask 
 * 
 * Revision 1.2  1999/10/14 14:24:42  iurlano 
 * Task Switch Test in progress
 */
/***************************************************************************/

#include <types.h>

#ifndef __SYS_DEFS
#define __SYS_DEFS

/* Definizione di interrupt gate */
typedef  
struct __InterruptDesc {
  word offset0_15;       /* Parola bassa dell'indirizzo della routine */
  word selector0_15;     /* selettore del segmento per la routine */
  byte zero;             /* Non usato per interrupt */
  byte flag;             /* Flags */
  word offset16_31;      /* Parola alta dell'indirizzo della routine */
} InterruptDesc __attribute__ ((packed));

typedef
struct __SegmentDes {
  word seg_length0_15;   /* low word of the segment length */
  word base_addr0_15;    /* low word of base address */
  byte base_addr16_23;	 /* low byte of high word of base addr. */
  byte flags;            /* segment type and misc. flags */
  byte access;           /* highest nibble of segment length and access flags*/
  byte base_addr24_31;   /* highest byte of base address */
} SegmentDes __attribute__ ((packed));

typedef
struct __TSS_32 { /* Exactly as intel documents */
  word link;
  word __pad0;
  dword ESP0;
  word SS0;
  word __pad1;
  dword ESP1;
  word SS1;
  word __pad2;
  dword ESP2;
  word SS2;
  word __pad3;
  dword CR3;
  dword EIP;
  dword EFLAG;
  dword EAX;
  dword ECX;
  dword EDX;
  dword EBX;
  dword ESP;
  dword EBP;
  dword ESI;
  dword EDI;
  word ES;
  word __pad4;
  word CS;
  word __pad5;
  word SS;
  word __pad6;
  word DS;
  word __pad7;
  word FS;
  word __pad8;
  word GS;
  word __pad9;
  word LDT;
  word __padA;
  word DEBUG_TRAP;
  word IO_MAP_BASE;
} TSS_32 __attribute__ ((packed));

/* Segment Desc Flag in Access  */

#define SD_OSAvail      0x10  /* Avail for OS */
#define SD_Size_32      0x40  /* Operands Size 0=16 bit 1=32 bit */
#define SD_4k_Size      0x80  /* =1 -> 4K sizes */

/* Segment Desc Flag in Flags  */

#define SD_App          0x10  /* 0=System 1=code or data */
#define SD_DPL_Mask     0x60  /* Descriptor privilege level */
#define SD_Present      0x80
#define SD_Type Mask    0xF

/* Segment Desc Types */

#define SD_Type_Data_RO          0x0
#define SD_Type_Data_RO_A        0x1
#define SD_Type_Data_RW          0x2
#define SD_Type_Data_RW_A        0x3
#define SD_Type_Data_ExD_RO      0x4 /* Expansion Down */
#define SD_Type_Data_ExD_RO_A    0x5
#define SD_Type_Data_ExD_RW      0x6
#define SD_Type_Data_ExD_RW_A    0x7

#define SD_Type_Exec             0x8
#define SD_Type_Exec_A           0x9
#define SD_Type_Exec_Read        0xA
#define SD_Type_Exec_Read_A      0xB
#define SD_Type_Exec_Conf        0xC /* Conforming */
#define SD_Type_Exec_Conf_A      0xD
#define SD_Type_Exec_Conf_Read   0xE
#define SD_Type_Exec_Conf_Read_A 0xF

/* System Descriptors Types (Da riguardare ) */

#define SD_Type_Reserved1          0x0
#define SD_Type_Available_TSS_16   0x1
#define SD_Type_LDT                0x2
#define SD_Type_Active_TSS_16      0x3
#define SD_Type_Call_Gate_16       0x4
#define SD_Type_Task_Gate          0x5
#define SD_Type_Interrupt_Gate_16  0x6
#define SD_Type_Trap_Gate_16       0x7

#define SD_Type_Reserved2          0x8
#define SD_Type_Available_TSS_32   0x9
#define SD_Type_Reserved3          0xA
#define SD_Type_Active_TSS_32      0xB
#define SD_Type_Call_Gate_32       0xC
#define SD_Type_Reserved4          0xD
#define SD_Type_Interrupt_Gate_32  0xE
#define SD_Type_Trap_Gate_32       0xF

/* Some macros */
#define sti() __asm__("sti");
#define cli() __asm__("cli");
#define nop() __asm__ ("nop")

#define iret() __asm__ ("iret")

#define pusha() __asm__("pusha");
#define popa() __asm__("popa");
 
/* End Of Interrupt (for both PICs) */
#define eoi()   __asm__("movb $0x20,%al \n"                    \
	                "outb %al,$0x20 \n");                  \
                __asm__("movb $0x20,%al \n"                    \
	                "outb %al,$0xA0 \n");

/* EFLAGS Defines */

#define EF_Trap          0x100
#define EF_Interrupt     0x200
#define EF_IOPL_Mask    0x3000
#define EF_NT           0x4000
#define EF_Resume      0x10000
#define EF_VM86        0x20000
#define EF_AC          0x40000
#define EF_VIF         0x80000
#define EF_VIP        0x100000
#define EF_ID         0x200000

/* CR0 BITS */

#define CR0_PE             0x1 /* Protection Enable */
#define CR0_MP             0x2 /* Monitor Coprocessor */
#define CR0_EM             0x4 /* Emulation (FPU) */
#define CR0_TS             0x8 /* Task Switched */
#define CR0_ET            0x10 /* Extension type */
#define CR0_NE            0x20 /* Numeric Error (FPU) */
#define CR0_WP         0x10000 /* Write Protect */
#define CR0_AM         0x40000 /* Alignment Mask */
#define CR0_NW      0x20000000 /* Not Write Throuhg */
#define CR0_CD      0x40000000 /* Cache Disable */
#define CR0_PG      0x80000000 /* Paging Enable */

/* User / Supervisor Levels */

#define SuperVisor_Level 0
#define User_Level       1

/* Functions */

SegmentDes PackDescriptor(dword Base_Address,dword Segment_Length,
			  int type,int dpl,int flags, int access);

void load_gdt(char * base,int numdes);
void load_idt(char * base,int numint);

dword get_DS(void);
dword get_ES(void);
dword get_FS(void);
dword get_GS(void);
dword get_SS(void);
void set_DS(word seg);
void set_ES(word seg);
void set_FS(word seg);
void set_GS(word seg);

void set_EFLAGS(dword a);
dword get_EFLAGS(void);

void call_tss(word tss_des);
void jump_tss(word tss_des);

#endif



