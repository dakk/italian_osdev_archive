/*!	\file include/kernel/kernel_map.h
 *	\brief The memory map of the kernel.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-05
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef KERNEL_MAP_H
#define KERNEL_MAP_H

// --- Physical address space ----------------------------------------- //

//! Physiscal address of the kernel (don't touch!!!).
#define K_PHYS_ADDR		0x100000

//! Initial kernel page directory (don't touch!!!).
#define K_PDE			0x1000

//! Initial kernel page table for virtual space (don't touch!!!).
#define K_PTE			0x2000

//! DMA memory area start address.
#define DMA_MEMORY_START	0x3000

//! DMA memory area end address.
#define DMA_MEMORY_END		dma_memory_end

// --- V86-required address space ------------------------------------- //

//! BIOS Interrupt Vector Table (IVT) start address.
#define BIOS_IVT_START		0x00000000
//! BIOS Interrupt Vector Table (IVT) end address.
#define BIOS_IVT_END		0x00001000

//! Video Buffer area start address.
#define VIDEO_BUF_START		0x000A0000
//! Video Buffer area end address.
#define VIDEO_BUF_END		0x000C0000

//! ROM BIOS memory start address.
#define BIOS_ROM_START		0x000C0000
//! ROM BIOS memory end address.
#define BIOS_ROM_END		0x00100000

//! The start of the physical memory usable for allocations (>16MB).
#define MEM_FREE_FRAMES_START	DMA_MEMORY_END

// --- Kernel virtual address space ----------------------------------- //

//! Kernel virtual start address.
//! \note Must be (PAGE_SIZE*1024) aligned!!!
#define K_VIR_START		0xC0000000

//! Kernel heap start address.
#define K_HEAP_START		0xD0000000

//! Kernel heap end address.
#define K_HEAP_END		0xE0000000

//! Some physical memory (first 16MB of physical memory are mapped here)
//! start address.
#define PHYS_MEM_START		0xE0000000

//! Some physical memory (first 16MB of physical memory are mapped here)
//! end address.
#define PHYS_MEM_END		0xE1000000

//! Temporary memory area start address.
#define K_MEM_TEMP_START	0xE1000000

//! Temporary memory area end address.
#define K_MEM_TEMP_END		0xFFC00000

//! Page tables area start address.
#define PAGE_TABLE_MAP		(0xFFC00000)

//! Page tables area end address.
#define PAGE_DIR_MAP		(0xFFC00000 + (PAGE_TABLE_MAP / (1024)))

#endif
