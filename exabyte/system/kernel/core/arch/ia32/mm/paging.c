/* paging.c -- IA-32 Hardware Paging */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#include <arch-ia32/mm/paging.h>
#include <arch-ia32/string.h>
#include <mm/paging.h>
#include <kernel.h>	// per le variabili del linker script

/* Paging normale:
 page directory: 1024 entry
 page table: 1024 entry
 
 1024 * 1024 = 1048576 pagine -- 4 GB

   Paging con PAE:
 page directory: 512 entry
 page table: 512 entry
 512 * 512 = 262144 pagine -- 1 GB 
   per coprire 4 GB ci vogliono 4 page directory
   mappate nella page-directory-pointer table (4 * unsigned long long = 32 byte) */

// ok, si presuppone che il primo mega non sia virtuale :)
#ifdef CONFIG_PAE
#define KERNEL_PAGEDIR1_TMP	0x10000
#define KERNEL_PAGEDIR2_TMP	KERNEL_PAGEDIR1_TMP+PAGE_SIZE
#define KERNEL_PAGEDIR3_TMP	KERNEL_PAGEDIR2_TMP+PAGE_SIZE
#define KERNEL_PAGEDIR4_TMP	KERNEL_PAGEDIR3_TMP+PAGE_SIZE
#define KERNEL_PAGETBL1_TMP	KERNEL_PAGEDIR4_TMP+PAGE_SIZE	// mappatura primo mega + kernel identity
#define KERNEL_PAGEDIRPTR_TMP	KERNEL_PAGETBL2_TMP+PAGE_SIZE	// indirizzo page-directory-pointer table del kernel
#else
#define KERNEL_PAGEDIR_TMP	0x10000
#define KERNEL_PAGETBL1_TMP	KERNEL_PAGEDIR_TMP+PAGE_SIZE
#endif

#if 0
#define block()	__asm__ __volatile__("movw $0x9F45,(0xB8000)\n" \
		"jmp .\n")
#else
#define block() __asm__ __volatile__("jmp .\n")
#endif


/* discardabile -- inizializza i dati di paginazione per il kernel al boot
 questa funzione mappa il primo mega e lo spazio del kernel */
void ia32_paging_init(void)
{
	unsigned int i,krnl_size;
	pagedir_t * pd = 0;
	pagetable_t * pt = 0;
#ifdef CONFIG_PAE
	unsigned int pdr_entry,pd_entry;
	pagedirptr_t * pdr = (pagedirptr_t *)(KERNEL_PAGEDIRPTR_TMP);
	/* TODO: controlla disponibilità PAE con CPUID */
/* pulisce la page-directory-pointer (pdr) table */
	memset((void *)pdr,(char )0,sizeof(pagedirptr_t));
/* imposta la pdr con le quattro page-directory e le pulisce */
	pdr->pdre[0] = make_pagedirptr(KERNEL_PAGEDIR1_TMP,PAGE_PRESENT);
	pdr->pdre[1] = make_pagedirptr(KERNEL_PAGEDIR2_TMP,PAGE_PRESENT);
	pdr->pdre[2] = make_pagedirptr(KERNEL_PAGEDIR3_TMP,PAGE_PRESENT);
	pdr->pdre[3] = make_pagedirptr(KERNEL_PAGEDIR4_TMP,PAGE_PRESENT);
	for(i=0;i<PTRTABLE_ENTRIES;i++)
	 memset((void *)((unsigned long)get_address(pdr->pdre[i])),(char )0,sizeof(pagedir_t));
/* setta la ricorsione */
	pd = (pagedir_t *)((unsigned long)get_address(pdr->pdre[PTRTABLE_ENTRIES-1])); // prende l'ultima page directory
	pd->pde[TABLE_ENTRIES-1] = make_pagedir((unsigned long)pd,PAGE_LASTPDE);
#else
/* pulisce la pagedir */
	pd = (pagedir_t *)(KERNEL_PAGEDIR_TMP);
	memset((void *)pd,(char)0,sizeof(pagedir_t));
/* setta la ricorsione */
	pd->pde[TABLE_ENTRIES-1] = make_pagedir(pd,PAGE_LASTPDE);
#endif
/* mappa fisicamente il primo mega come PAGE_SYSTEM */
	pt = (pagetable_t *)(KERNEL_PAGETBL1_TMP);
	memset((void *)pt,(char)0,sizeof(pagetable_t));
	pd->pde[0] = make_pagedir((unsigned long)pt,PAGE_TABLE);
	for(i=1;i<(1048576/PAGE_SIZE);i++) pt->pte[i] = make_pagetable(i*PAGE_SIZE,PAGE_SYSTEM);
/* mappa fisicamente il kernel (il kernel deve finire prima del 4° mega) */
	krnl_size = (g_end - g_start) / PAGE_SIZE;
	if((g_end - g_start) % PAGE_SIZE) krnl_size++;
	if((((unsigned long)load_adr / PAGE_SIZE)+krnl_size) >= 0x400000) block();	// peccato :P
	for(i=(unsigned long)load_adr/PAGE_SIZE;i<=((unsigned long)load_adr/PAGE_SIZE)+krnl_size;i++)
	 pt->pte[i%TABLE_ENTRIES] = make_pagetable(i*PAGE_SIZE,PAGE_SYSTEM);
#ifdef CONFIG_PAE
/* aggiorna CR3 */
	set_CR3((dword)pd & 0xFFFFFFE0);	// 32 byte alignment
#else
/* aggiorna CR3 */
	set_CR3((dword)pd & 0xFFFFF000);	// 4096 byte aligment
#endif
}

#if 0	// inutile -- il kernel è mappato fisicamente (non rimuovere, potrebbe servirci in futuro :)
/* discardabile -- toglie la mappatura fisica del kernel all'avvio dopo init_paging */
void remove_kernelident(void)
{
#ifdef CONFIG_PAE
	/* TODO: rimuovi l'identity mapping del kernel */
	block();
#else
	pagetable_t * pt;
	unsigned int krnl_size,i;
	krnl_size = (g_end - g_start) / PAGE_SIZE;
	if((g_end - g_start) % PAGE_SIZE) krnl_size++;
/* ora posso togliere la mappatura fisica del kernel */
	pt = (pagetable_t *)(PT_VIRT + (sizeof(pagetable_t) * (((unsigned long)load_adr/PAGE_SIZE)/TABLE_ENTRIES)));
	for(i=(unsigned long)load_adr/PAGE_SIZE;i<=((unsigned long)load_adr/PAGE_SIZE)+krnl_size;i++)
	 pt->pte[i] = 0;
#endif
}
#endif

/* mappa l'indirizzo <physical> in <virtual> con flag <flags>
 restituisce l'indirizzo virtuale se tutto è andato bene, altrimenti un di errore formato con i flag di paging */
void * ia32_paging_map(void * virtual,void * physical,unsigned int flags)
{
	return (void *)0;
}

/** ******************************
  funzioni architecture-standard
******************************** **/

/* mappa la pagina all'indirizzo <physical> in <virtual> di tipo <type> */
int arch_mm_paging_map(void * virtual,void * physical,int type)
{
	/* TODO: scegliere i tipi di mappatura */
	return -1;
}

/* mappa <count> pagine agli indirizzi <physicals> in <virtual>
 restituisce il numero di pagine mappate correttamente */
int arch_mm_paging_multimap(void * virtual,void * physicals[],int count,int type)
{
	unsigned int retmap = 0,i;
	for(i=0;i<count;i++) if(arch_mm_paging_map(virtual,physicals[i],type) >= 0) retmap++;
	return retmap;
}

/* mappa fisicamente <count> pagine in <virtual>
 restituisce il numero di pagine mappate correttamente */
int arch_mm_paging_identmap(void * virtual,int count)
{
	unsigned int retmap = 0,i;
	for(i=0;i<count;i++) if(arch_mm_paging_map(virtual,virtual,type) >= 0) retmap++;
	return retmap;
}
