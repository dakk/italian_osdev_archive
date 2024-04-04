/*!	\file include/arch/paging.h
 *	\brief Paging header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-26
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef PAGING_H
#define PAGING_H

#include <const.h>

// Kernel virtual memory map.
#include <kernel/kernel_map.h>

#if	defined(CONFIG_M686) || defined(CONFIG_MPENTIUMII) || \
	defined(CONFIG_MPENTIUMIII) || defined(CONFIG_MPENTIUM4) || \
	defined(CONFIG_MK7) || defined(CONFIG_MATHLONXP)

#define ARCH_HAS_GLOBAL_PAGE
#endif

//! Determine the page size.
#define PAGE_SHIFT	12

//! Determine the memory area covered by a page directory entry.
#define PAGE_DIR_SHIFT	22

//! The page size.
#define PAGE_SIZE	( 1UL << PAGE_SHIFT )

//! The memory covered by a page directory entry.
#define PAGE_DIR_SIZE	( 1UL << PAGE_DIR_SHIFT )

//! The page mask.
#define PAGE_MASK	( ~(PAGE_SIZE-1) )

//! Page present flag.
#define P_PRESENT	( 1 << 0 )

//! Page writeable flag.
#define P_WRITE		( 1 << 1 )

//! Page at user privilege level.
#define P_USER		( 1 << 2 )

//! Write-through policy.
#define P_WTHROUGH	( 1 << 3 )

//! Page caching disabled.
#define P_NOCACHE	( 1 << 4 )

//! Page accessed flag.
#define P_ACCESSED	( 1 << 5 )

//! Page dirty flag (the page has been written).
#define P_DIRTY		( 1 << 6 )

#ifdef ARCH_HAS_GLOBAL_PAGE
//! Global page (for TLB invalidation).
#define P_GLOBAL	( 1 << 8 )
#endif

//! Rounds an address up to a page boundary.
#define PAGE_ALIGN_UP(addr)	(((addr) + PAGE_SIZE - 1) & -PAGE_SIZE)

//! Rounds an address down to a page boundary.
#define PAGE_ALIGN(addr)	((addr) & -PAGE_SIZE)

//! Rounds an address up to a page directory boundary.
#define PAGE_DIR_ALIGN_UP(addr)	(((addr) & -(PAGE_SIZE*1024)) + PAGE_SIZE*1024)

//! Rounds an address down to a page directory boundary.
#define PAGE_DIR_ALIGN(addr)	((addr) & -(PAGE_SIZE*1024))

//! Converts from address to page.
#define ADDR_TO_PAGE(addr)	(addr / PAGE_SIZE)

//! Converts from page to address
#define PAGE_TO_ADDR(p_addr)	(p_addr * PAGE_SIZE)

//! Get current page directory base address.
#define GET_PDBR()		(*ADDR_TO_PDE(PAGE_TABLE_MAP) & -PAGE_SIZE)

//! Access to the page directory entry referred to the
//! virtual address \p addr.
#define ADDR_TO_PDE(addr)	(dword *)(PAGE_DIR_MAP + (((dword) (addr) / (1024 * 1024))&(~0x3)))

//! Access to the page table entry referred to the
//! virtual address \p addr.
#define ADDR_TO_PTE(addr)	(dword *)(PAGE_TABLE_MAP + ((((dword) (addr) / 1024))&(~0x3)))

//! Get the page directory index of the current address.
#define PDE_INDEX( addr ) \
	( ((addr) >> PAGE_DIR_SHIFT) & 1023 )

//! Get the page table index of the current address.
#define PTE_INDEX( addr ) \
	( ((addr) >> PAGE_SHIFT) & 1023 )

//! Get the offset of a page directory entry.
#define PDE_OFFSET( pd, addr ) \
	( (uint32_t *)(pd) + PDE_INDEX( addr ) )

//! Get the offset of a page table entry.
#define PTE_OFFSET( pt, addr ) \
	( (uint32_t *)(pt) + PTE_INDEX( addr ) )

//! Access to an address into the mapped physical memory area.
#define PHYSICAL(addr)		((addr) + PHYS_MEM_START)

//! Convert a virtual address of the low memory area (DMA-memory) into
//! the equivalent physical address.
#define VIRTUAL(addr)		((addr) - PHYS_MEM_START)

//! Page table structure.
typedef struct PAGE_TABLE
{
	dword entry[1024];
} page_table_t;

//! Flush every entry into the TLB cache (address translation).
#define __flush_tlb_all()					\
	do							\
	{							\
		register unsigned int tmpreg;			\
								\
		__asm__ __volatile__(				\
			"movl %%cr3, %0;  # flush TLB \n"	\
			"movl %0, %%cr3;              \n"	\
			: "=r" (tmpreg)				\
			: : "memory");				\
	} while (0)

//! Flush every entry into the TLB cache (address translation),
//! also global pages
#define __flush_tlb_global()					\
	do							\
	{							\
		register unsigned int tmp, cr4;			\
								\
		__asm__ __volatile__(				\
			"movl %%cr4, %1\n"			\
			"test $0x80, %1 ; # pge is on?\n"	\
			"jz 1f\n"				\
			"and $(~0x80), %1 ; # turn off pge\n"	\
			"movl %1, %%cr4\n"			\
			"movl %%cr3, %0 ; # invalidate\n"	\
			"movl %0, %%cr3\n"			\
			"or $0x80, %1 ; # turn on pge\n"	\
			"movl %1, %%cr4\n"			\
			"jmp 2f\n"				\
			"1: "					\
			"movl %%cr3, %0 ; # invalidate\n"	\
			"movl %0, %%cr3\n"			\
			"2:"					\
			: "=&r"(tmp), "=&r"(cr4)		\
			: : "memory"				\
		);						\
	} while ( 0 )

#ifdef ARCH_HAS_GLOBAL_PAGE
	#define flush_tlb_all()		__flush_tlb_all()
#else
	#define flush_tlb_all()		__flush_tlb_global()
#endif

#if defined(CONFIG_M386)
#define flush_tlb_single(addr)	__flush_tlb_all()
#else
//! Flush a single entry into the TLB cache (address translation).
//! \param addr
//!	The virtual address of the page that must be invalidated.
#define flush_tlb_single(addr) \
	__asm__ __volatile__("invlpg %0": :"m" (*(char *) addr))
#endif

//! \brief
//!	Translate a virtual address into the physical address.
//! \param vir_addr The virtual address to be translated.
static __INLINE__ size_t vir_to_phys(size_t vir_addr)
{
	if (*ADDR_TO_PDE(vir_addr) == NULL)
		return(NULL);
	return( (*ADDR_TO_PTE(vir_addr) & -PAGE_SIZE) + (vir_addr % PAGE_SIZE) );
}

//! \brief
//! Switch into a new virtual address space.
//! \warning pdbr is the physical address of the page directory!
static __INLINE__ void switch_mmu( uint32_t pdbr )
{
	// Switch to the new virtual space.
	__asm__ __volatile__ ( "movl %0, %%cr3" : : "r"(pdbr) );
}

/** \ingroup Memory
 *  \defgroup Paging Paging
 *  The page manager module.
 *  @{
 */

__INLINE__ size_t pop_frame();
__INLINE__ void push_frame(size_t p_addr);

__INLINE__ size_t get_free_frames();

bool FASTCALL( map_page(dword vir_addr, dword phys_addr, word attribs) );
void FASTCALL( unmap_page(dword vir_addr) );
void FASTCALL( delete_page(dword vir_addr) );

void *get_temp_page();
void FASTCALL( free_temp_page(void *p, bool do_delete) );

int FASTCALL( page_fault_handler(addr_t cr2) );
int FASTCALL( page_fault_cow_handler(addr_t cr2) );

void dump_dirty_pages();
void dump_free_frames();
void check_free_frames_integrity() __NORETURN__;

void init_paging();

/** @} */ // end of Paging

#endif
