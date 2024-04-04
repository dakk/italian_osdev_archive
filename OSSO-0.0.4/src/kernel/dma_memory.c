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
 * file : kernel/dma_memory.c 
 * Description: function for memory allocation under 1MB for DMA
 ***************************************************************************

 ***************************************************************************
 * $Id: dma_memory.c,v 1.3 2000/12/05 20:59:47 iurlano Exp $
 ***************************************************************************
 *
 * $Log: dma_memory.c,v $
 * Revision 1.3  2000/12/05 20:59:47  iurlano
 * Thread work in progress. Yet to make first complete subsystem
 *
 * Revision 1.2  2000/11/26 23:34:59  iurlano
 * work in progress on DMA memory
 *
 * Revision 1.1  2000/11/24 18:25:54  iurlano
 * Added first try of a DMA memory management. Yet to be tested
 *
 *
 ***************************************************************************/

#define __PRINT_MODULE_NAME__ "DMA_Memory"

#include <types.h>
#include <printk.h>
#include <errors.h>
#include <memory.h>
#include <dma_memory.h>
#include <process.h>
#include <panic.h>

/*
  Range of 1MBMemory to be allocated is 0x10000 - 0x1FFFF (64KB)
  Minimum block size is 256 byte

 */



byte Block_Used[DMA_Memory_Block_Number];

//extern Bool_Semaphore Mem_Mutex; /* Mutual exclusion with the memory module */

int DMA_Memory_Init(void) {
  int i;
  for (i=0;i<DMA_Memory_Block_Number;i++) Block_Used[i]=0;
  return E_SUCCESS;
}

void * DMA_Memory_Get_Block(dword * ret_phys_addr) {
  int i;
  PTE * p;
  PDE * page_dir;
  PTE * page_table;
  dword * indirizzo_di_ritorno;
  dword k;
  dword * phys_addr;
  
  for (i=0;i<DMA_Memory_Block_Number;i++) 
    if (Block_Used[i]==0) {
      Bool_P(&(Processo_In_Esecuzione->Mem_Sem));

      page_dir=(PDE *)Processo_In_Esecuzione->PageDir;
      phys_addr=(dword*)physical_address((dword)ret_phys_addr,page_dir);
      Processo_In_Esecuzione->last_used_page+=4096;
      //     printk("DMA MEM : Last used page = %x\n",Processo_In_Esecuzione->last_used_page);
      indirizzo_di_ritorno=(void *)(Processo_In_Esecuzione->last_used_page);
      k=(Processo_In_Esecuzione->last_used_page)>>22;
      //printk("Page Dir : %d\n",k);
      if (page_dir[k].Present==0) {
	p=getpagetable();
	if (p==NULL) panic("Memory finished !");
	page_dir[k].Page_Table_Base_Addr=(dword)(p)>>12;
	page_dir[k].Present=1;
	page_dir[k].Read_Write=1;
	page_dir[k].User=0;
      
      }
      page_table=(PTE *)((dword)page_dir[k].Page_Table_Base_Addr<<12);
      k=((Processo_In_Esecuzione->last_used_page)>>12)&0x3FF;
      //printk("Page Table : %d\n",k);
      *phys_addr=((dword)(DMA_Memory_Start+DMA_Memory_Block_Size*i));
      page_table[k].Page_Base_Addr=((dword)(DMA_Memory_Start+DMA_Memory_Block_Size*i))>>12;
      page_table[k].Present=1;
      page_table[k].Read_Write=1;
      page_table[k].User=1;

      Block_Used[i]=1;
      //      Processo_In_Esecuzione->last_used_page+=4096;
      Bool_V(&(Processo_In_Esecuzione->Mem_Sem));
      //      printk("Indirizzo di ritorno = %x\n",indirizzo_di_ritorno);
      //      printk("Indirizzo fisico : %x\n",physical_address(indirizzo_di_ritorno,page_dir));
      //      printk("Stringa1 %s\n",indirizzo_di_ritorno);
      //printk("Stringa2 %s\n",*phys_addr);
      return indirizzo_di_ritorno;
    }
  return (void*)-1;
}

int DMA_Memory_Free_Block(void * address) {
  int i;
  dword addr;
  addr=physical_address((dword)address,(PDE *)Processo_In_Esecuzione->PageDir);
  //printk("Deallocating DMA memory area at 0x%x\n",addr);
  i=((addr-DMA_Memory_Start)/DMA_Memory_Block_Size);
  Block_Used[i]=0;
  return E_SUCCESS;
}



