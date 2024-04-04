/*!	\file include/kernel/kmalloc.h
 *	\brief Headers for the kernel memory allocators.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date 2003-10-24
 *	\note Copyright (C) 2003 Andrea Righi
 */

#ifndef KMALLOC_H
#define KMALLOC_H

// Constants								//

/** \ingroup Memory
 *  \defgroup KMemoryAlloc Kernel Memory Allocator
 *  The memory operators for the kernel.
 *  @{
 */

//! Magic number to identify a memory block.
#define M_MAGIC		((dword)"$MB$")
//! Free block.
#define M_FREE		0
//! Allocated block.
#define M_ALLOC		1

//! Allocate normal kernel ram.
#define GFP_KERNEL	1

//! Use inside interrupt.
#define GFP_ATOMIC	2

//! A memory block structure.
typedef struct MEM_BLOCK
{
	dword	magic,
		flags,
		size,
		owner;
} mem_block_t;

void * FASTCALL( kmalloc(size_t size, int mflag) );
void   FASTCALL( kfree(void *ptr) );
void * FASTCALL( kcalloc(size_t num, size_t size, int flags) );
void * FASTCALL( kmemalign(size_t alignment, size_t size, int flags) );

void kmalloc_init();
int mem_sizeof(void *ptr);

/** @} */ // end of KMemoryAlloc

/** \ingroup Memory
 *  \defgroup DebugMemoryAlloc Debug Routines for Memory Allocator
 *  Functions to debug the kernel memory area allocator.
 *  @{
 */

void dump_memory_map();

/** @} */ // end of DebugMemoryAlloc

#endif
