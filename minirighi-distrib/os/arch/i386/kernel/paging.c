/*!	\file arch/i386/kernel/paging.c
 *	\brief Paging manager.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-12-04 Andrea Righi:
 *			Added the fork() syscall.\n
 *		2004-08-25 Andrea Righi:
 *			Added page fault handler for COW pages.\n
 *		2004-03-23 Andrea Righi:
 *			Added global page feature for kernel pages.\n
 *			Added kpdbr counter to update the page directory
 *			of each task with the kernel page directory
 *			only when it is necessary.\n
 *		2004-02-07 Andrea Righi:
 *			Added the fast_clear_page using MMX extensions
 *			to clear a page after is mapped.\n
 *		2004-01-24 Andrea Righi:
 *			Added the NULL-pointer mapping check in the
 *			page fault handler.\n
 *		2003-12-16 Andrea Righi:
 *			Fixed get_temp_page() and free_temp_page()
 *			bugs.\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 *
 *	\note
 *		This is the GNU linker script (LD) where are declared
 *		some external variables used by the paging manager.
 *		\include kernel.ld
 */

#include <const.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <arch/i386.h>
#include <arch/mem.h>
#include <arch/mmx.h>
#include <arch/spinlock.h>

#include <kernel/console.h>
#include <kernel/cow.h>
#include <kernel/debug.h>
#include <kernel/dma.h>
#include <kernel/multiboot.h>
#include <kernel/kernel_map.h>
#include <kernel/keyboard.h>
#include <kernel/speaker.h>
#include <kernel/task.h>

#include <arch/paging.h>

extern task_t *curr_task;

//! Total amount of physical memory.
volatile size_t PHYS_MEM_DIM;

//! The kernel bounds. These variables must be declared externally
//! into the linker script.
extern size_t KERNEL_TOP;

//! Free-frames stack - placed just above kernel memory.
uint32_t *free_frames = (size_t *)&KERNEL_TOP;

//! End of kernel virtual space.
uint32_t *K_VIR_END = (size_t *)&KERNEL_TOP;

// --- Free frames stack operators ------------------------------------ //

//! \brief Pop a free frame from the free frames stack.
//! \exception NULL Out of physical memory.
__INLINE__ size_t pop_frame()
{
	uint32_t __ret;
	uint32_t flags;

	local_irq_save( flags );

	if( *free_frames!=NULL )
	{
		__ret = *free_frames++;

		local_irq_restore( flags );
		return( __ret );
	}

	local_irq_restore( flags );

	// Out of memory!!!
	return( NULL );
}

//! \brief Push a free frame into the free frames stack.
//! \param p_addr
//!	The page number you want to push.
__INLINE__ void push_frame( size_t p_addr )
{
	uint32_t flags;

	local_irq_save( flags );

	// Push the frame into free frames stack.
	if( (size_t)free_frames>((size_t)&KERNEL_TOP) )
	{
		*(--free_frames) = ( p_addr & PAGE_MASK );
	}

	local_irq_restore( flags );
}

//! \brief Get the number of free physical frames.
//! \return The number of free frames.
__INLINE__ size_t get_free_frames()
{
	return( ((size_t)K_VIR_END-(size_t)free_frames)/sizeof(size_t) );
}

//! \brief
//!	Initialize the free frames stack.
//! \note
//!	This routine must be called once, when the system is
//!	initializing.
void __INIT__ init_free_frames()
{
	// First physical 16MB are reserved for kernel, BIOS & DMA
	// so let's start with free memory area at 16MB.
	register size_t p_addr = MEM_FREE_FRAMES_START;
	uint32_t flags;

	local_irq_save( flags );

	K_VIR_END = free_frames;
	while( p_addr < PHYS_MEM_DIM )
	{
		*K_VIR_END++ = (p_addr += PAGE_SIZE);
	}
	// Last frame is NULL => out of physical memory.
	// Kernel virtual address space ends here...
	*K_VIR_END = NULL;

	local_irq_restore( flags );
}

// --- Mapping operators ---------------------------------------------- //

//! \brief
//!	Map a physical address into a virtual address for the
//!	current address space.
//! \param vir_addr The virtual address.
//! \param phys_addr The physical address.
//! \param attribs
//!	The attributes mask for this page.
//! \return
//!	\li #TRUE on success;
//!	\li #FALSE if out-of-memory.
bool map_page( size_t vir_addr, size_t phys_addr, uint16_t attribs )
{
	uint32_t *PTE;
	uint32_t i;
	uint32_t flags;

	// Round virtual & physical address to the page boundary.
	vir_addr = PAGE_ALIGN(vir_addr);
	phys_addr = PAGE_ALIGN(phys_addr);

	// Get only valid attribs.
	attribs &= (PAGE_SIZE - 1);

	local_irq_save(flags);

	// If the page directory entry is NULL must be created.
	if (*ADDR_TO_PDE(vir_addr) == NULL) {
		// Create a new page table.
		PTE = (uint32_t *)pop_frame();
		if (PTE == NULL) {
			// Out of memory!!!
			local_irq_restore(flags);
			return FALSE;
		}

		// Set the PDE as present, user level, read-write.
		*ADDR_TO_PDE(vir_addr) = (uint32_t)PTE | P_PRESENT | P_USER | P_WRITE;

		// NULL every PTE entry.
		for (i = PAGE_DIR_ALIGN(vir_addr); i < PAGE_DIR_ALIGN_UP(vir_addr); i += PAGE_SIZE) {
			*ADDR_TO_PTE(i) = NULL;
		}

		if (vir_addr >= K_VIR_START) {
			// Other tasks must update their page directory.
			if (curr_task != NULL)
				curr_task->pdbr_update_counter++;
		}
	}

	// Store the physical address into the page table entry.
	*ADDR_TO_PTE(vir_addr) = (size_t)phys_addr | attribs;

	// Invalidate the page in the TLB cache.
	flush_tlb_single(vir_addr);

	local_irq_restore(flags);

	return TRUE;
}

//! \brief
//!	Unmap a page, but doesn't destroy the physical frame.
//! \param vir_addr The virtual address you want to unmap.
void unmap_page( size_t vir_addr )
{
	size_t temp;
	uint32_t flags;
	size_t __pde;
	int __ret;
	int i;

	// Align address to the page boundary.
	vir_addr = PAGE_ALIGN(vir_addr);

	local_irq_save(flags);

	__pde = *ADDR_TO_PDE(vir_addr);
	if ((__pde & P_PRESENT) == P_PRESENT) {
		// Unset the PT as shared.
		__ret = unset_shared_page(PAGE_ALIGN(__pde));
		if (!__ret) {
			*ADDR_TO_PDE(vir_addr) |= P_WRITE;
		} else {
			// The page table is shared.
			size_t tmp_page[PAGE_SIZE / sizeof(size_t)];
			size_t phys_addr;

			// Get a physical free frame.
			phys_addr = pop_frame();
			if (phys_addr == NULL) {
				// Out of physical memory.
				printk(KERN_EMERG
					"%s: out of physical memory!!!\n",
					__FUNCTION__);
				local_irq_restore(flags);
				return;
			}

			// First COW event for this page table; create also a new identical page table.
			memcpy(tmp_page, ADDR_TO_PTE(PAGE_DIR_ALIGN(vir_addr)), PAGE_SIZE);

			// Update the page directory entry.
			*((size_t *)ADDR_TO_PDE(vir_addr)) = phys_addr | P_PRESENT | P_WRITE | P_USER;

			// Map each entry in read-only mode.
			for (i = 0; i < PAGE_SIZE / sizeof(size_t); i++) {
				if ((tmp_page[i] & P_PRESENT) == P_PRESENT) {
					tmp_page[i] &= ~P_WRITE;
					__ret = set_shared_page(PAGE_ALIGN(tmp_page[i]));
					if (__ret < 0) {
						printk(KERN_EMERG
							"%s: out of physical memory!!!\n",
							__FUNCTION__);
						return;
					}
				}
			}
			// Copy the page table.
			memcpy(ADDR_TO_PTE(PAGE_DIR_ALIGN(vir_addr)), tmp_page, PAGE_SIZE);

			// Flush TLB.
			flush_tlb_all();
		}
	} else {
		local_irq_restore(flags);
		return;
	}

	// Unmap the page.
	*ADDR_TO_PTE(vir_addr) = NULL;

	// Invalidate the page in the TLB cache.
	flush_tlb_single(vir_addr);

	// Check if it is possible to deallocate the frame
	// of the page table used to map the address
	// So let's examine all entries in the page table
	// where the address is mapped.
	for (temp = PAGE_DIR_ALIGN(vir_addr);
		temp < PAGE_DIR_ALIGN_UP(vir_addr);
		temp += PAGE_SIZE) {
		if ((*ADDR_TO_PTE(temp) & P_PRESENT) == P_PRESENT) {
			local_irq_restore(flags);
			return;
		}
	}

	// No PTEs found... deallocate the page table!
	push_frame(*ADDR_TO_PDE(vir_addr));
	*ADDR_TO_PDE(vir_addr) = NULL;

	if (vir_addr >= K_VIR_START) {
		// Other tasks must update their page directory.
		if (curr_task!=NULL)
			curr_task->pdbr_update_counter++;
	}

	local_irq_restore(flags);
}

//! \brief
//!	Unmap a page and destroy the physical frame where the address
//!	is mapped.
//! \param vir_addr The virtual address you want to delete.
void delete_page(size_t vir_addr)
{
	size_t temp;
	uint32_t flags;
	size_t __pde, __pte;
	int __ret;
	int i;

	// Align address to the page boundary.
	vir_addr = PAGE_ALIGN(vir_addr);

	local_irq_save(flags);

	__pde = *ADDR_TO_PDE(vir_addr);
	if ((__pde & P_PRESENT) == P_PRESENT) {
		// Unset the PT as shared.
		__ret = unset_shared_page(PAGE_ALIGN(__pde));
		if (!__ret) {
			*ADDR_TO_PDE(vir_addr) |= P_WRITE;
		} else {
			// The page table is shared.
			size_t tmp_page[PAGE_SIZE / sizeof(size_t)];
			size_t phys_addr;

			// Get a physical free frame.
			phys_addr = pop_frame();
			if (phys_addr == NULL) {
				// Out of physical memory.
				printk(KERN_EMERG
					"%s: out of physical memory!!!\n",
					__FUNCTION__);
				local_irq_restore(flags);
				return;
			}

			// First COW event for this page table; create also a new identical page table.
			memcpy(tmp_page, ADDR_TO_PTE(PAGE_DIR_ALIGN(vir_addr)), PAGE_SIZE);

			// Update the page directory entry.
			*((size_t *)ADDR_TO_PDE(vir_addr)) = phys_addr | P_PRESENT | P_WRITE | P_USER;

			// Map each entry in read-only mode.
			for (i = 0; i < PAGE_SIZE / sizeof(size_t); i++) {
				if ((tmp_page[i] & P_PRESENT) == P_PRESENT) {
					tmp_page[i] &= ~P_WRITE;
					__ret = set_shared_page(PAGE_ALIGN(tmp_page[i]));
					if (__ret < 0) {
						printk(KERN_EMERG
							"%s: out of physical memory!!!\n",
							__FUNCTION__);
						return;
					}
				}
			}

			// Copy the page table.
			memcpy(ADDR_TO_PTE(PAGE_DIR_ALIGN(vir_addr)), tmp_page, PAGE_SIZE);

			// Flush TLB.
			flush_tlb_all();
		}
	} else {
		local_irq_restore(flags);
		return;
	}

	__pte = *ADDR_TO_PTE(vir_addr);
	if ((__pte & P_PRESENT) == P_PRESENT) {
		__pte = PAGE_ALIGN(__pte);
		// Unset the page as shared.
		__ret = unset_shared_page(__pte);
		if (!__ret) {
			// Push the physical frame into the free frames stack.
			if ((__pte >= DMA_MEMORY_END) && (__pte < PHYS_MEM_DIM)) {
				push_frame(__pte);
			}
		}
	} else {
		local_irq_restore(flags);
		return;
	}

	// Unmap the page.
	*ADDR_TO_PTE(vir_addr) = NULL;

	// Invalidate the page in the TLB cache.
	flush_tlb_single(vir_addr);

	// Check if it is possible to deallocate the frame
	// of the page table used to map the address
	// So let's examine all entries in the page table
	// where the address is mapped.
	for (temp = PAGE_DIR_ALIGN(vir_addr);
		temp < PAGE_DIR_ALIGN_UP(vir_addr);
		temp += PAGE_SIZE) {
		if ((*ADDR_TO_PTE(temp) & P_PRESENT) == P_PRESENT) {
			local_irq_restore(flags);
			return;
		}
	}

	// No PTEs found... deallocate the page table!
	push_frame(*ADDR_TO_PDE(vir_addr));
	*ADDR_TO_PDE(vir_addr) = NULL;

	if (vir_addr >= K_VIR_START) {
		// Other tasks must update their page directory.
		if (curr_task!=NULL)
			curr_task->pdbr_update_counter++;
	}

	local_irq_restore(flags);
}

// --- Temporary pages ------------------------------------------------ //

//! Spinlock for mutual exclusion of temporary pages.
DECLARE_SPINLOCK( mem_temp_lock );

//! \brief
//!	Get a free virtual page from the temporary memory area.
//! \return
//!	The address of the temporary page or NULL if out of memory.
void *get_temp_page()
{
	byte *p = (byte *)K_MEM_TEMP_START;
	size_t frame;
	uint32_t flags;

	spin_lock_irqsave( &mem_temp_lock, flags );

	while( TRUE )
	{
		if( *ADDR_TO_PDE((size_t)p) )
		{
			if( *ADDR_TO_PTE((size_t)p) )
			{
				p += PAGE_SIZE;
				if( p >= (byte *)K_MEM_TEMP_END )
				{
					// Out of temporary memory!
					spin_unlock_irqrestore( &mem_temp_lock, flags );
					return( NULL );
				}
				continue;
			}
		}
		// OK! A free temporary page has been found!
		// Now map this page to a free frame and return the
		// virtual address of this page, or NULL if there is
		// no free frame.
		frame = pop_frame();
		if ( frame==NULL )
		{
			p = NULL;
		}
		#ifdef ARCH_HAS_GLOBAL_PAGE
		else if( !map_page((uint32_t)p, frame, P_PRESENT | P_WRITE | P_GLOBAL) )
		#else
		else if( !map_page((uint32_t)p, frame, P_PRESENT | P_WRITE) )
		#endif
		{
			p = NULL;
		}
		spin_unlock_irqrestore( &mem_temp_lock, flags );
		return( (void *)p );
	}
}

//! \brief
//!	Free a virtual temporary page allocated by get_temp_page().
//! \param p The temporary page virtual address.
//! \param do_delete
//!	\li TRUE free the page and also the physical frame;
//!	\li FALSE unmap the page only, do not free the physical frame.
void free_temp_page(void *p, bool do_delete)
{
	uint32_t flags;

	spin_lock_irqsave( &mem_temp_lock, flags );

	// Is the page into the temporary memory range?!		//
	if ( (((size_t)p >= (K_MEM_TEMP_START))) && ((size_t)p < K_MEM_TEMP_END) )
	{
		if ( do_delete == TRUE )
			delete_page( (uint32_t)p );
		else
			unmap_page( (uint32_t)p );
	}

	spin_unlock_irqrestore( &mem_temp_lock, flags );
}

// --- Page fault handler --------------------------------------------- //

/*! \ingroup Handlers */
//! \brief
//!	This is the page-fault handler. Every time a page-fault
//!	occurs, this routine must be invoked.
//! \param cr2 The address that caused the fault.
//! \return
//!	\li 0 success;
//!	\li < 0 otherwise.
int page_fault_handler(addr_t cr2)
{
	addr_t phys_addr;
	int __ret;

#ifdef PAGING_DEBUG
	printk(KERN_DEBUG "page fault at %#010x\n", cr2);
#endif
	if (cr2 == NULL) {
		// Cannot map the NULL pointer.
		printk(KERN_ERR
			"%s: NULL address is forbidden!\n",
			__FUNCTION__);
		return -EFAULT;
	}

	// Get a physical free frame.
	phys_addr = pop_frame();
	if (phys_addr == NULL) {
		// Out of physical memory.
		printk(KERN_EMERG
			"%s: out of physical memory!!!\n",
			__FUNCTION__);
		return -ENOMEM;
	}

	// Map page with correct attributes.
	if (cr2 < K_VIR_START) {
		__ret = map_page(cr2, phys_addr, P_PRESENT | P_WRITE | P_USER);
	} else {
#ifdef ARCH_HAS_GLOBAL_PAGE
		__ret = map_page(cr2, phys_addr,
			(cr2 < PAGE_TABLE_MAP) ?
			(P_PRESENT | P_WRITE | P_GLOBAL) : (P_PRESENT | P_WRITE));
#else
		__ret = map_page( cr2, phys_addr, P_PRESENT | P_WRITE );
#endif
	}
	if (!__ret) {
		printk(KERN_EMERG
			"%s: cannot perform the page mapping!\n",
			__FUNCTION__);
		return( -ENOMEM );
	}

	fast_clear_page((void *)PAGE_ALIGN(cr2));

	// Well done.
	return 0;
}

/*! \ingroup Handlers */
//! \brief
//!	The page fault handler for COW paging.
//! \param cr2 The address that caused the fault.
//! \return
//!	\li 0 success;
//!	\li < 0 otherwise.
/**
 * \todo frame counter.
 */
int page_fault_cow_handler(addr_t cr2)
{
	size_t tmp_page[PAGE_SIZE / sizeof(size_t)];
	size_t phys_addr;
	size_t pd_entry, pt_entry;
	int __ret, i;

	// NULL address if forbidded!
	if (cr2 == NULL) {
		return -EFAULT;
	}

	// Can't use COW for kernel pages!
	if (cr2 >= K_VIR_START) {
		return -EFAULT;
	}

	// Get the page directory entry.
	pd_entry = *((size_t *)ADDR_TO_PDE(cr2));
	if ((pd_entry & P_WRITE) != P_WRITE) {
		// Get a physical free frame.
		phys_addr = pop_frame();
		if (phys_addr == NULL) {
			printk(KERN_EMERG
				"%s: out of physical memory!!!\n",
				__FUNCTION__);
			return -ENOMEM;
		}
		// Unset the PT as shared.
		__ret = unset_shared_page(PAGE_ALIGN(pd_entry));
		if (!__ret) {
			*((size_t *)ADDR_TO_PDE(cr2)) |= (P_PRESENT | P_WRITE | P_USER);
			push_frame(phys_addr);
		} else {
			// First COW event for this page table; create also a new identical page table.
			memcpy(tmp_page, ADDR_TO_PTE(PAGE_DIR_ALIGN(cr2)), PAGE_SIZE);

			// Update the page directory entry.
			*((size_t *)ADDR_TO_PDE(cr2)) = phys_addr | P_PRESENT | P_WRITE | P_USER;

			// Map each entry in read-only mode.
			for (i = 0; i < PAGE_SIZE / sizeof(size_t); i++) {
				if ((tmp_page[i] & P_PRESENT) == P_PRESENT) {
					tmp_page[i] &= ~P_WRITE;
					__ret = set_shared_page(PAGE_ALIGN(tmp_page[i]));
					if (__ret < 0) {
						printk(KERN_EMERG
							"%s: out of physical memory!!!\n",
							__FUNCTION__);
						return -ENOMEM;
					}
				}
			}
			// Copy the page table.
			memcpy(ADDR_TO_PTE(PAGE_DIR_ALIGN(cr2)), tmp_page, PAGE_SIZE);
		}

		// Flush TLB.
		flush_tlb_all();
	}

	// Get the page table entry.
	pt_entry = *((size_t *)ADDR_TO_PTE(cr2));

	// Get a physical free frame.
	phys_addr = pop_frame();
	if (phys_addr == NULL) {
		printk(KERN_EMERG
			"%s: out of physical memory!!!\n",
			__FUNCTION__);
		return -ENOMEM;
	}
	// Unset the page as shared.
	__ret = unset_shared_page(PAGE_ALIGN(pt_entry));
	if (!__ret) {
		*((size_t *)ADDR_TO_PTE(cr2)) |= (P_PRESENT | P_WRITE | P_USER);
		push_frame(phys_addr);
	} else {
		memcpy(tmp_page, (void *)PAGE_ALIGN(cr2), PAGE_SIZE);

		__ret = map_page(cr2, phys_addr, P_PRESENT | P_WRITE | P_USER);
		if (!__ret) {
			printk(KERN_EMERG
				"%s: out of physical memory!!!\n",
				__FUNCTION__);
			return -ENOMEM;
		}

		// Copy the page data into the new page.
		memcpy((void *)PAGE_ALIGN(cr2), tmp_page, PAGE_SIZE);
	}

	// Flush TLB.
	flush_tlb_single(cr2);

	return 0;
}

// --- Initialization routines ---------------------------------------- //

#ifdef ARCH_HAS_GLOBAL_PAGE
//! Enable global pages feature.
static __INLINE__ void enable_global_pages()
{
	size_t addr;
	register unsigned long __tmp=0;
	unsigned long flags;

	local_irq_save( flags );

	__asm__ __volatile__ (
		"movl %%cr4, %0\n"
		"or $0x80, %0\n"
		"movl %0, %%cr4" : : "r"(__tmp)
	);

	// Enable global page feature for all kernel pages.
	addr = K_VIR_START;
	while( addr < PAGE_TABLE_MAP )
	{
		if( *ADDR_TO_PDE(addr) != NULL )
		{
			if( *ADDR_TO_PTE(addr) != NULL )
			{
				*ADDR_TO_PTE(addr) |= P_GLOBAL;
			}
			addr += PAGE_SIZE;
		}
		else
		{
			addr = PAGE_DIR_ALIGN_UP( addr );
		}
	}

	local_irq_restore( flags );
}
#endif

//! \brief
//!	Initialize the paging mechanism.
//! \note
//!	This routine must be called once, when the system is
//!	initializing.
//! \warning
//!	This routine must be the first routine of the kernel
//!	initialization process!!!
void __INIT__ init_paging()
{
	extern size_t KERNEL_TEXT, KERNEL_END_TEXT;
	extern multiboot_info_t *boot_info;
	extern void __task_exit_point;

	size_t addr;

	if( boot_info->flags & 0x02 )
	{
		PHYS_MEM_DIM = (boot_info->mem_upper) * 1024;
	}
	else
	{
		// Error reading multiboot informations... halting!.
		printk( KERN_EMERG
			"%s(): invalid multiboot informations!!!\n",
			__FUNCTION__ );
		printk( KERN_EMERG
			"System halted.\n",
			__FUNCTION__ );
		halt();
	}

	// Evaluate a reasonable value for the DMA memory upper
	// bound considering the amount of physical memory.
	dma_setup_memory_end( PHYS_MEM_DIM );

	// Initialize free frames stack.
	init_free_frames();

	// Unmap first 4MB identical-map pages.
	*ADDR_TO_PDE( 0 ) = NULL;

	// Initialize kernel code section in read-only mode.
	for( addr = (size_t)&KERNEL_TEXT; addr < (size_t)&KERNEL_END_TEXT; addr += PAGE_SIZE )
		if( *ADDR_TO_PDE(addr) )
			*ADDR_TO_PTE( addr ) &= ~P_WRITE;

	// Map only the exit point address available for users.
	*ADDR_TO_PDE( (size_t)&__task_exit_point ) |= P_USER;
	*ADDR_TO_PTE( (size_t)&__task_exit_point ) |= P_USER;

	// Map part of physical memory into the kernel address space.
	for( addr = PHYS_MEM_START; addr < PHYS_MEM_END; addr += PAGE_SIZE )
		map_page( addr, addr-PHYS_MEM_START, P_PRESENT | P_WRITE | P_NOCACHE );

	// Initialize temporary memory area.
	for( addr = K_MEM_TEMP_START; addr < K_MEM_TEMP_END; addr += PAGE_SIZE )
                 if( *ADDR_TO_PDE(addr) )
			*ADDR_TO_PTE(addr) = NULL;

#ifdef ARCH_HAS_GLOBAL_PAGE
	// Enable global pages feature.
	enable_global_pages();
#endif
	// Flush the whole TLB cache.
	flush_tlb_all();
}

// --- Debug functions ------------------------------------------------ //

//! \brief
//!	Dump the dirty pages to stdout.
void dump_dirty_pages()
{
	dword vir_addr;
	dword display=1;

	// Print all the dirty pages.
	kprintf("\nDirty pages:\n");
	for (vir_addr = 0; vir_addr<PAGE_TABLE_MAP; vir_addr += PAGE_SIZE)
		if (*ADDR_TO_PDE(vir_addr) != NULL)
			if (
				(*ADDR_TO_PTE(vir_addr) & (P_DIRTY | P_PRESENT)) ==
				(P_DIRTY | P_PRESENT)
			)
			{
				if (!(display = display++ % 24))
					if( kgetchar() == CTRL_C )
					{
						kprintf("\n");
						return;
					}
				if( !vir_addr )
				{
					printk( KERN_ERR
						"%s(): ouch! null page cannot be mapped!\n",
						__FUNCTION__ );
				}
				kprintf("\nvirtual_address = ");
				set_color( LIGHT_BLUE );
				kprintf("%#010x", vir_addr);
				set_color( DEFAULT_COLOR );
				kprintf("\tpage_entry = ");
				set_color( YELLOW );
				kprintf("%#010x", *(ADDR_TO_PTE(vir_addr)));
				set_color( DEFAULT_COLOR );
			}
	kprintf("\n");
}

//! \brief
//!	Dump the free frames to stdout.
void dump_free_frames()
{
	dword *f = free_frames;
	dword display=1;

	kprintf("\nFree frames stack: (KERNEL_TOP=%#010x)\n", (dword)&KERNEL_TOP);
	for(;;)
	{
		if (*f == NULL) break;
		if (!(display = display++ % 24))
			if( kgetchar() == CTRL_C )
			{
				kprintf("\n");
				return;
			}
		kprintf("\n( &");
		set_color( LIGHT_BLUE );
		kprintf("%#010x", f);
		set_color( DEFAULT_COLOR );
		kprintf(" ) = ");
		set_color( YELLOW );
		kprintf("%#010x", (*f));
		set_color( DEFAULT_COLOR );
		f++;
	}
	kprintf("\n");
}

//! \brief
//!	This process checks the free frames stack integrity.
void check_free_frames_integrity( int argc, char **argv )
{
	dword flag, i;
	dword *j;

	kprintf("\nChecking free frames integrity (%s)...\n", get_pname());
	for( i=0x1000000; i<PHYS_MEM_DIM; i+=PAGE_SIZE )
	{
		flag = 0;
		for( j=free_frames; j<K_VIR_END; j++ )
		{
			if ( *j == i )
				flag++;
			if( flag>1 )
			{
				printk( KERN_EMERG
					"\nerror! duplicated entries of frame : %#010x", i);
				break;
			}
		}
	}
	beep();
	kprintf("\nCheck done.\n");
	_exit( 0 );
}
