/* paging.h -- IA-32 Hardware Paging definitions */
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

#ifndef IA32_PAGING_H
#define IA32_PAGING_H

#include <arch-ia32/config.h>

/* costanti dei bit delle pde/pte */
#define PAGE_PRESENT	0x001	// 000000000001
#define PAGE_RW		0x002	// 000000000010
#define PAGE_USER	0x004	// 000000000100

/* Flag pre-confezionti :) */
#define PAGE_NULL	0					// pagina inesistente
#define PAGE_LASTPDE	PAGE_PRESENT | PAGE_RW			// flag per la pde di ricorsione
#define PAGE_TABLE	PAGE_PRESENT | PAGE_RW | PAGE_USER	// flag per una page directory entry
#define PAGE_SHARED	PAGE_PRESENT | PAGE_RW | PAGE_USER	// flag per una pagina utente (kernel/user space)
#define PAGE_SYSTEM	PAGE_PRESENT | PAGE_RW			// flag per una pagina di sistema (user space)

/* costanti varie */
#define PAGE_SIZE       4096
#ifdef CONFIG_PAE
 #define TABLE_ENTRIES		512
 #define PTRTABLE_ENTRIES	4
#else
 #define TABLE_ENTRIES	1024
#endif

#define MAX_VIRPAGES		1048576		// 4GB / 4KB -- massimo numero di pagine virtuali
#define PAGE_VECTOR_SIZE	MAX_PAGES / (sizeof(dword)*8)

/* indirizzi virtuali dei dati di paginazione */
#ifdef CONFIG_PAE
/*
	12 bit -- offset
	 9 bit -- page table entry
	 9 bit -- page directory entry
	 2 bit -- page directory pointer entry
*/
 #define PDR_VIRT		???
 #define PD_VIRT		???
 #define PT_VIRT		???
#else
 #define PD_VIRT		0xFFFFF000	// ((TABLE_ENTRIES*TABLE_ENTRIES-1)*PAGE_SIZE)
 #define PT_VIRT		0xFFC00000	// ((TABLE_ENTRIES*(TABLE_ENTRIES-1))*PAGE_SIZE)
#endif

/* Address alignment */
#define ADDR32_ALIGN(x,a)	((unsigned long)(x) & (0xFFFFFFFF-((a)-1)))
#define PAGE32_ALIGN(x)		ADDR32_ALIGN(x,PAGE_SIZE)

#if 0
typedef struct
{
	unsigned int present:1;		/* Page Present Bit */
	unsigned int readwrite:1;	/* Read/Write Bit */
	unsigned int privilege:1;	/* User/Supervisor Bit */
	unsigned int pagewrite:1;	/* Page Write Through Bit */
	unsigned int pagecache:1;	/* Page Cache Disable Bit */
	unsigned int accessed:1;	/* Accessed Bit */
	unsigned int dirty:1;		/* Dirty Bit */
	unsigned int reserved:1;	/* (Page Size Bit) */
	unsigned int global:1;		/* Global Page Bit */
	unsigned int unused:3;		/* Unused (Free Bits) */
	unsigned int page_address:20;
} __attribute__ ((packed)) pagetable_entry;

typedef struct
{
	unsigned int present:1;		/* Page Present Bit */
	unsigned int readwrite:1;	/* Read/Write Bit */
	unsigned int privilege:1;	/* User/Supervisor Bit */
	unsigned int pagewrite:1;	/* Page Write Through Bit */
	unsigned int pagecache:1;	/* Page Cache Disable Bit */
	unsigned int accessed:1;	/* Accessed Bit */
	unsigned int reserved:1;	/* (Dirty Bit) */
	unsigned int pagesize:1;	/* Page Size Bit */
	unsigned int global:1;		/* Global Page Bit (ignored) */
	unsigned int unused:3;		/* Unused (Free Bits) */
	unsigned int table_address:20;
} __attribute__ ((packed)) pagedir_entry;
#endif

#ifdef CONFIG_PAE
typedef unsigned long long pagetable_entry;
typedef unsigned long long pagedir_entry;
typedef unsigned long long pagedirptr_entry;	// page-directory-pointer table entry
typedef struct
{
	pagedirptr_entry pdre[PTRTABLE_ENTRIES];
} pagedirptr_t;	// page-directory-pointer table
#else
typedef unsigned long pagetable_entry;
typedef unsigned long pagedir_entry;
#endif

typedef struct
{
	pagedir_entry pde[TABLE_ENTRIES];
} pagedir_t;

typedef struct
{
	pagetable_entry pte[TABLE_ENTRIES];
} pagetable_t;

/* Costruzione entry */
#ifdef CONFIG_PAE
 #define make_pagetable(addr,flags) ( ( ((unsigned long long)(addr) / PAGE_SIZE) << 12) | ( (flags) & 0xFFF) )
 #define make_pagedirptr(addr,flags) (((unsigned long long)(addr) / PAGE_SIZE) | ((flags) & 0xE19))
#else
 #define make_pagetable(addr,flags) (((unsigned long)(addr) & 0xFFFFF000) | ((flags) & 0x00000FFF))
#endif
#define make_pagedir(addr,flags) make_pagetable(addr,flags)

/* Modifica entry */
/* presenza */
#define set_present(entry) (entry | PAGE_PRESENT)
#define set_notpresent(entry) (entry & ~PAGE_PRESENT)

/* permessi */
#define set_writable(entry) (entry | PAGE_WRITE)
#define set_readonly(entry) (entry & ~PAGE_WRITE)

/* privilegi */
#define set_userpage(entry) (entry | PAGE_USER)
#define set_supervisor(entry) (entry & ~PAGE_USER)

/* Estrazione indirizzo/flags */
#define get_address(entry) ((entry >> 12) << 12)
#define get_flags(entry) (entry & 0xFFF)

/* Estrazione indici entry */
#define get_pdrentry(addr) (((unsigned long)addr & 0xC0000000) >> 30)
#define get_pdentry(addr) (((unsigned long)addr & 0x3FE00000) >> 21)
#define get_ptentry(addr) (((unsigned long)addr & 0x1FF000) >> 12)

/* Verifiche varie */
#define is_present(entry) (entry & PAGE_PRESENT)
#define is_writable(entry) (entry & PAGE_WRITE)
#define is_userpage(entry) (entry & PAGE_USER)

#define flush_tlb_one(addr) __asm__ __volatile__("invlpg %0": :"m" (*(char *) addr))
#define set_CR3(value) __asm__ __volatile__("mov %0, %%cr3"::"r"(value));
#define get_CR3() ({ \
	unsigned int __dummy; \
	__asm__( \
		"movl %%cr3,%0\n\t" \
		:"=r" (__dummy)); \
	__dummy; \
})
#define get_CR2() ({ \
	unsigned int __dummy; \
	__asm__( \
		"movl %%cr2,%0\n\t" \
		:"=r" (__dummy)); \
	__dummy; \
})

#define invalidate() __asm__ __volatile__("movl %%cr3,%%eax\n\tmovl %%eax,%%cr3": : :"%eax")

void ia32_paging_init(void);
void * ia32_paging_map(void * virtual,void * physical,unsigned int flags);

/** ******************************
  funzioni architecture-standard
******************************** **/

/** PER MIUS: DEVI SEGUIRE UNO STANDARD UGUALE PER TUTTE LE ARCH QUANDO SCRIVI DELLE FUNZIONI CHE USERA` ANCHE IL
 KERNEL CORE... VEDEVO CHE TU USAVI SOLO LE FUNZIONI ia32_*, E LE ALTRE ARCH DOVE LE METTI??? **/

int arch_mm_paging_map(void * physical,void * virtual,int type);
int arch_mm_paging_multimap(void * virtual,void * physicals[],int count,int type);
int arch_mm_paging_identmap(void * virtual,int count);

#endif	/* IA32_PAGING_H */
