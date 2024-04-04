/*!	\file include/kernel/umalloc.h
 *	\brief Header of Virtual Memory Allocator for the User Space.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-01-01
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef UMALLOC_H
#define UMALLOC_H

#include <kernel/task.h>

//! Magic number to identify a memory block.
#define UM_MAGIC	((uint32_t)"$UM$")
//! Free block.
#define UM_FREE		0
//! Allocated block.
#define UM_ALLOC	1

//! A memory block structure.
typedef struct UMEM_BLOCK
{
	uint32_t	magic,
			flags,
			size,
			owner;
} umem_block_t;

// --- Prototypes ----------------------------------------------------- //

void * FASTCALL( do_umalloc(task_t *t, size_t size) );
void   FASTCALL( do_ufree(task_t *t, void *ptr) );

void umalloc_init(task_t *t, size_t heap_start, size_t heap_size);

void *umalloc( size_t size );
void ufree( void *ptr );

#endif
