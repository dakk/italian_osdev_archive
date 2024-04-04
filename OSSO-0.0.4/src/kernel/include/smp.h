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
 * file : include/smp.h                                                    *
 * Intel Simmetric Multi Processing header                                 *
 ***************************************************************************


 ***************************************************************************
 * $Id: smp.h,v 1.4 2001/05/15 21:44:44 iurlano Exp $
 ***************************************************************************
 *
 * $Log: smp.h,v $
 * Revision 1.4  2001/05/15 21:44:44  iurlano
 * work on smp (detection works)
 *
 * Revision 1.3  2001/05/15 14:35:11  iurlano
 * work on smp
 *
 * Revision 1.2  2001/05/14 21:33:13  iurlano
 * First work on smp systems (thanks to smpbochs)
 *
 * Revision 1.1  2001/05/14 15:45:57  iurlano
 * Work on SMP
 *
 *
 ***************************************************************************/



#ifndef __SMP__DEFS
#define __SMP__DEFS

#include <types.h>

#define MAX_PROCESSORS 4

typedef struct __SMP_Floating_Pointer {
  char signature[4];
  dword physical_address;
  byte length;
  byte revision;
  byte checksum;
  byte feature_1;
  byte feature_2;
} SMP_Floating_Pointer;

typedef struct __SMP_Configuration {
  char signature[4];
  word length;
  byte revision;
  byte checksum;
  char oem_id[8];
  char product_id[12];
  dword oem_table_pointer;
  word oem_table_size;
  word entries;
  dword local_APIC;
  word extended_table_length;
  byte extended_table_checksum;
} SMP_Configuration;

#define SMP_Entry_Type_Processor                  0
#define SMP_Entry_Type_Bus                        1
#define SMP_Entry_Type_IO_APIC                    2
#define SMP_Entry_Type_IO_Interrupt_Assignment    3
#define SMP_Entry_Type_Local_Interrupt_Assignment 4

typedef struct __SMP_Entry_Processor {
  byte Entry_Type;
  byte local_APIC_id;
  byte local_APIC_ver;
  byte CPU_flags;
  dword CPU_Signature;
  dword feature_flags;
  dword reserved1;
  dword reserved2;
} SMP_Entry_Processor;

typedef struct __SMP_Entry_Bus {
  byte Entry_Type;
  byte bus_id;
  char bus_type[6];
} SMP_Entry_Bus;

typedef struct __SMP_Entry_IO_APIC {
  byte Entry_Type;
  byte IO_APICE_id;
  byte IO_APIC_ver;
  byte IO_APIC_flags;
  dword APIC_address;
} SMP_Entry_IO_APIC;

typedef struct __SMP_Entry_IO_Interrupt {
  byte Entry_Type;
  byte IO_interrupt_type;
  word IO_interrupt_flag;
  byte source_bus_id;
  byte source_bus_irq;
  byte destination_IO_APIC_id;
  byte destination_IO_APIC_int;
} SMP_Entry_IO_Interrupt;

typedef struct __SMP_Entry_Local_Interrupt {
  byte Entry_Type;
  byte Local_interrupt_type;
  word Local_interrupt_flag;
  byte source_bus_id;
  byte source_bus_irq;
  byte destination_Local_APIC_id;
  byte destination_Local_APIC_int;
} SMP_Entry_Local_Interrupt;

int Check_SMP(void);

#define IMCR_Port_Address  0x22
#define IMCR_Port_Value    0x23

#define IMCR_Register_Number 0x70

extern int BootStrap_Processor;
extern int Num_Processors;
extern SMP_Entry_Processor Processors[MAX_PROCESSORS];

#define Local_APIC_Base_Address 0xFEE00000

/* Local APIC registers, from base, as index of a dword array */
//                                    Index      address
#define APIC_REG_Reserved1             0x00   // 0x0000
#define APIC_REG_Reserved2             0x04   // 0x0010
#define APIC_REG_ID                    0x08   // 0x0020
#define APIC_REG_Version               0x0C   // 0x0030
#define APIC_REG_Reserved3             0x10   // 0x0040
#define APIC_REG_Reserved4             0x14   // 0x0050
#define APIC_REG_Reserved5             0x18   // 0x0060
#define APIC_REG_Reserved6             0x1C   // 0x0070
#define APIC_REG_Task_Priority         0x20   // 0x0080
#define APIC_REG_Arbitration_Priority  0x24   // 0x0090
#define APIC_REG_Processor_Priority    0x28   // 0x00A0
#define APIC_REG_EOI                   0x2C   // 0x00B0
#define APIC_REG_Reserved7             0x30   // 0x00C0
#define APIC_REG_Logical_Dest          0x34   // 0x00D0
#define APIC_REG_Dest_Format           0x38   // 0x00E0
#define APIC_REG_Spurius_IV            0x3C   // 0x00F0

#define APIC_REG_ISR                   0x40   // 0x0100

#define APIC_REG_TMR                   0x60   // 0x0180

#define APIC_REG_IRR                   0x80   // 0x0200

#define APIC_REG_Error_Status          0xA0   // 0x0280

#define APIC_REG_Command_Low           0xC0   // 0x0300
#define APIC_REG_Command_High          0xC4   // 0x0310
#define APIC_REG_Local_Vector_Timer    0xC8   // 0x0320

#define APIC_REG_Performance_Counter   0xD0   // 0x0340
#define APIC_REG_Local_Vector_LINT0    0xD4   // 0x0350
#define APIC_REG_Local_Vector_LINT1    0xD8   // 0x0360
#define APIC_REG_Local_Vector_Error    0xDC   // 0x0370
#define APIC_REG_Initial_Timer_Count   0xE0   // 0x0380
#define APIC_REG_Current_Timer_Count   0xE4   // 0x0390
#define APIC_REG_Timer_Divide_Config   0xF8   // 0x03E0


#endif

