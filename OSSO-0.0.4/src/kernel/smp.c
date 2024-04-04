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


/************************************ OSSO ***********************************
 *  File: kernel/smp.c                                                       *
 *  Simmetric Multi Processing support                                       *
 *****************************************************************************

 *****************************************************************************
 * $Id: smp.c,v 1.4 2001/05/16 10:45:50 iurlano Exp $
 *****************************************************************************
 *
 * $Log: smp.c,v $
 * Revision 1.4  2001/05/16 10:45:50  iurlano
 * work on smp
 *
 * Revision 1.3  2001/05/15 21:44:44  iurlano
 * work on smp (detection works)
 *
 * Revision 1.2  2001/05/15 14:35:11  iurlano
 * work on smp
 *
 * Revision 1.1  2001/05/14 21:33:13  iurlano
 * First work on smp systems (thanks to smpbochs)
 *
 *
 *****************************************************************************/

#include <types.h>

#include <smp.h>
#include <printk.h>

#include <panic.h>
#include <process.h>
#include <io.h>
#include <string.h>
#include <memory.h>

SMP_Entry_Processor Processors[MAX_PROCESSORS];

int BootStrap_Processor;
int Num_Processors;

SMP_Floating_Pointer * SMP_FP;
SMP_Configuration * SMP_Conf;

int IMCR_Present;

unsigned long int Read_IMCR(void) {
  outb(IMCR_Register_Number,IMCR_Port_Address);
  return inb(IMCR_Port_Value);
}

void Write_IMCR(byte value) {
  outb(IMCR_Register_Number,IMCR_Port_Address);
  outb(value,IMCR_Port_Value);
}

int Check_SMP(void) {
  dword address;

  Num_Processors=0;
  BootStrap_Processor=-1;

  address=0xF0000;
  SMP_FP=0;
  printk("Checking Multiprocessor ... \n");
  while (address<=0xFFFF0) {
    if (strncmp((char*)address,"_MP_",4)==0) {
      SMP_FP=(SMP_Floating_Pointer *) address;
      break;
    }
    address+=0x10;
  }
  if (SMP_FP!=0) {
    int i;
    printk("  Intel MP structure found at %x\n",SMP_FP);
    printk("  Configuration Table address: %x\n",SMP_FP->physical_address);
    printk("  Length: %x\n",SMP_FP->length);
    printk("  Revision: %x\n",SMP_FP->revision);
    printk("  Feature 1: %x\n",SMP_FP->feature_1);
    printk("  Feature 2: %x\n",SMP_FP->feature_2);
    if (SMP_FP->feature_2&128) IMCR_Present=1; else IMCR_Present=0;
    if (SMP_FP->physical_address!=0) {
      SMP_Conf=(SMP_Configuration *)SMP_FP->physical_address;
      printk("   Configuration table signature : ");
      for (i=0;i<4;i++) _printk("%c",SMP_Conf->signature[i]);
      _printk("\n");
      printk("    OEM id :");
      for (i=0;i<8;i++) _printk("%c",SMP_Conf->oem_id[i]);
      _printk("\n");
      printk("    Product id :");
      for (i=0;i<12;i++) _printk("%c",SMP_Conf->product_id[i]);
      _printk("\n");
      printk("    Number of Entries : %d\n",SMP_Conf->entries);
      address=(dword)(((byte*)SMP_Conf)+sizeof(*SMP_Conf));
      for (i=0;i<SMP_Conf->entries;i++) {
	switch (*((byte*)address)) {
	case SMP_Entry_Type_Processor: {
	  SMP_Entry_Processor *ssll;
	  printk("    Entry : %d Type Processor\n",i);
	  ssll=(SMP_Entry_Processor *)address;
	  printk("      APIC id   : %d\n",ssll->local_APIC_id);
	  printk("      APIC ver  : %d\n",ssll->local_APIC_ver);
	  printk("      CPU Flags : %s %s\n",(ssll->CPU_flags)&1?"Usable":"Unusable",(ssll->CPU_flags)&2?"Bootstrap":"Application");
	  if ((ssll->CPU_flags)&2) BootStrap_Processor=Num_Processors;
	  Processors[Num_Processors++]=*ssll;
	  address+=20;
	  break;
	}
	case SMP_Entry_Type_Bus: {
	  SMP_Entry_Bus *ssll;
	  int k;
	  ssll=(SMP_Entry_Bus *)address;
	  printk("    Entry : %d Type Bus\n",i);
	  printk("      ID : %d, Type ",ssll->bus_id);
	  for (k=0;k<6;k++) _printk("%c",ssll->bus_type[k]);
	  _printk("\n");
	  address+=8;
	  break;
	}
	case SMP_Entry_Type_IO_APIC:              
	  printk("    Entry : %d Type I/O APIC\n",i);
	  address+=8;
	  break;
	case SMP_Entry_Type_IO_Interrupt_Assignment:
	  printk("    Entry : %d Type I/O Interrupt\n",i);
	  address+=8;
	  break;
	case SMP_Entry_Type_Local_Interrupt_Assignment:
	  printk("    Entry : %d Type Local Interrupt\n",i);
	  address+=8;
	  break;
	default:
	  printk("    ERROR: Unkown entry type %x !!\n",*((byte*)address));
	}
      }
    }
    if (IMCR_Present)
      printk("  * IMCR register = %x\n",Read_IMCR());
    {
      dword * APIC_REGS;
      dword k,i;
      PTE * p;
      PDE * pd;

      pd=Kernel_Page_Dir;
      k=Local_APIC_Base_Address>>22;
      if (pd[k].Present==0) {
	//	printk("Getting Page Table (%d)\n",k);
	p=getpagetable();
	if (p==NULL) panic("Memory finished !");
	//	printk("Got %x\n",p);
	pd[k].Page_Table_Base_Addr=(dword)(p)>>12;
	pd[k].Present=1;
	pd[k].Read_Write=1;
	pd[k].User=0;
      }
      p=(PTE *)((dword)(pd[k].Page_Table_Base_Addr)<<12);
      i= ((Local_APIC_Base_Address)>>12)&0x3FF;
      //printk("Allocating PTE %d at %x base\n",i,(Local_APIC_Base_Address)>>12);
      p[i].Page_Base_Addr=(Local_APIC_Base_Address)>>12;
      p[i].Present=1;
      p[i].User=0;
      p[i].Read_Write=1;
      APIC_REGS=(dword*)Local_APIC_Base_Address;
    printk(" * Checking APIC ... \n");
    printk(" * APIC is a %s\n",APIC_REGS[APIC_REG_Version]<0x10?"82489DX":"Local APIC");

    printk(" * Activating Local APIC\n");
    printk(" *   SIV   = %x\n", APIC_REGS[APIC_REG_Spurius_IV]);
    APIC_REGS[APIC_REG_Spurius_IV]|= (1<<8);
    printk(" *   SIV 2 = %x\n", APIC_REGS[APIC_REG_Spurius_IV]);
    }
    return 1;   
  }
  return 0;
}

