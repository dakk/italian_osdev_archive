/*!	\file kernel/kmalloc.c
 *	\brief Memory Allocator.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-24\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#include <const.h>
#include <stdio.h>
#include <string.h>

#include <arch/i386.h>
#include <arch/mem.h>
#include <arch/paging.h>

#include <kernel/console.h>
#include <kernel/kernel_map.h>
#include <kernel/keyboard.h>
#include <kernel/task.h>

#include <kernel/kmalloc.h>

//! \brief Initialize the kernel virtual memory area.
void __INIT__ kmalloc_init( void )
{
	mem_block_t *p;

	// Create only one hudge block					//
	p = (mem_block_t *)K_HEAP_START;
	p->magic = M_MAGIC;
	p->flags = M_FREE;
	p->size = K_HEAP_END - K_HEAP_START - sizeof(mem_block_t);
	p->owner = NULL;
}

//! \brief
//!	Allocate a memory block from the kernel memory area.
//!	This is a first-fit like allocator.
//! \param size The size of the block you want to allocate.
//! \exception NULL Out-of-memory.
//! \return A pointer to a memory area.
//! \note The memory area is not initialized when it's just allocated.
void *kmalloc( size_t size, int mflag )
{
	mem_block_t *p, *p_best = NULL;
	uint32_t flags;

	if( !size )
		return( NULL );

	// Round the size up to sizeof(mem_block_t); the
	// sizeof(mem_block_t) is the unit of allocation.
	size = ( size & -(sizeof(mem_block_t)) ) + sizeof(mem_block_t);
	p = (mem_block_t *)K_HEAP_START;

	local_irq_save( flags );

	// Explore the heap for the best-fit hole.
	while( p < (mem_block_t *)K_HEAP_END )
	{
		// Find a free block.
		if( p->flags == M_FREE )
		{
			// This block is free... let's proceed!
			if( p->size >= size )
			{
				// Best fit block found! You're lucky!
				p_best = p;
				break;
			}
		}
		// Examine the next memory block.
		p = (void *)p + p->size + sizeof(mem_block_t);
	}

	// Have you found a free block?!
	if( p_best == NULL )
	{
		// No free memory :(
		local_irq_restore( flags );
		return NULL;
	}

	// If the block does not cover perfectly the size,
	// split the hole in two blocks
	if( p_best->size != size )
	{
		p = (void *)p_best + sizeof(mem_block_t) + size;
		p->magic = M_MAGIC;
		p->flags = M_FREE;
		p->size = p_best->size - size - sizeof(mem_block_t);
		p->owner = NULL;

		p_best->size = size;
	}

	// Allocate the block.
	p_best->magic = M_MAGIC;
	p_best->flags = M_ALLOC;
	if( mflag == GFP_KERNEL )
	{
		p_best->owner = curr_task->pid;
	}
	else
	{
		// kmalloc() during interrupt! the owner
		// is ambiguous.
		p_best->owner = NULL;
	}

	// Return the first-fit pointer.
	p_best = (void *)p_best + sizeof(mem_block_t);

	local_irq_restore( flags );

	return( (void *)p_best );
}

//! \brief Try to release physical frames occupied by the freed block.
//! \param ptr The pointer to deallocate.
static void deallocate( mem_block_t *ptr )
{
	size_t start =
		PAGE_ALIGN_UP( (size_t)((void *)ptr +
			sizeof(mem_block_t)) );
	size_t end =
		PAGE_ALIGN( (size_t)((void *)ptr +
			sizeof(mem_block_t) + ptr->size) );

	while( start < end )
	{
		if( *ADDR_TO_PDE(start) != NULL )
		{
			delete_page( start );
			start += PAGE_SIZE;
		}
		else
		{
			start = PAGE_DIR_ALIGN_UP( start );
		}
	}
}

//! \brief
//!	Frees the memory space pointed by ptr, which must have
//!	been returned by a previous call to kmalloc(size_t size).
//! \param ptr The pointer you want to free.
void kfree( void *ptr )
{
	mem_block_t *p, *p2;
	uint32_t flags;

	if( ptr == NULL ) return;

	// Point to the header.
	p = (void *)ptr - sizeof(mem_block_t);

	if( p->magic != M_MAGIC ) return;
	if( p->flags != M_ALLOC ) return;

	local_irq_save( flags );

	// Free the block.
	p->flags = M_FREE;
	p->owner = NULL;

	// Try to combine the block wih the next one.
	p2 = (void *)p + p->size + sizeof(mem_block_t);

	if( p2 < (mem_block_t *)K_HEAP_END )
	{
		if( p2->flags == M_FREE )
		{
			// Let's merge the two blocks!
			p->size += p2->size + sizeof(mem_block_t);
			p2->magic = NULL;
		}
	}

	// Try to combine the block with the previous one.
	if( p != (mem_block_t *)K_HEAP_START )
	{
		// Find the previous block.
		p2 = (void *)K_HEAP_START;
		for(;;)
		{
			if( ((void *)p2 + p2->size + sizeof(mem_block_t)) == p )
			{
				// Block found!
				// Check if it's a free block.
				if( p2->flags == M_FREE )
				{
					// Let's merge the two blocks!
					p2->size += (p->size + sizeof(mem_block_t));
					p->magic = NULL;
					p = p2;
				}
				break;
			}
			p2 = (void *)p2 + p2->size + sizeof(mem_block_t);
		}
	}

	// Release the physical space occupied by the block.
	deallocate( p );

	local_irq_restore( flags );
}

//! \brief
//!	Return the size of the pointer \a ptr passed as argument.
//! \param ptr The pointer you want to know the size.
//! \exception NULL Invalid pointer \a ptr.
//! \return The size of the pointer \a ptr passed as argument.
int mem_sizeof( void *ptr )
{
	mem_block_t *p = ((mem_block_t *)ptr-1);
	uint32_t flags;
	int __ret;

	local_irq_save( flags );

	if ( (p->magic == M_MAGIC) && (p->flags == M_ALLOC) )
		__ret =  p->size;
	else
		__ret = NULL;

	local_irq_restore( flags );

	return( __ret );
}

//! \brief Allocate some memory aligned to a boundary.
//! \param alignment The boundary.
//! \param size The size you want to allocate.
//! \exception NULL Out-of-memory.
//! \return
//! 	A pointer to a memory area aligned to the boundary.
//!	The pointer is a \c aligned_mem_block_t pointer, so if you
//!	want to access to the data area of this pointer you must
//!	specify the \c p->start filed.
//! \note Use kfree(void *ptr) to free the allocated block.
void *kmemalign( size_t alignment, size_t size, int flags )
{
	mem_block_t *p, *p2;

	// Cannot allocate memory with a size of zero.
	if( !size )
	{
		return( NULL );
	}

	// Allocate the pointer.
	p = kmalloc(size + alignment + 2 * sizeof(mem_block_t), flags);
	if( p == NULL )
	{
		return( NULL );
	}

	// An alignment of zero is intended as a simple kmalloc(size).
	if ( alignment == 0 )
	{
		return( p );
	}

	// Check if the pointer is correcly aligned.
	if( (size_t)p % alignment )
	{
		// Align the pointer p to the boundary.
		uint32_t flags;

		local_irq_save( flags );

		// Allocate the new block.
		p2 = p + 2;
		p2 = (mem_block_t *)ALIGN_UP((size_t)p2, alignment)  - 1;
		p2->magic = M_MAGIC;
		p2->flags = (p-1)->flags;
		p2->owner = (p-1)->owner;
		p2->size = mem_sizeof(p) - ((size_t)(p2+1) - (size_t)p);

		// Free the unused space.
		(p-1)->size = (size_t)p2 - (size_t)p;

		local_irq_restore( flags );

		kfree( p );

		return( p2 + 1 );
	}

	// The pointer p is already aligned to the boudary.
	return( p );
}

//! Allocate num elements of size bytes each, all initialized to 0.
void *kcalloc( size_t num, size_t size, int flags )
{
	void *__ret;

	__ret = kmalloc( num * size, flags );
	if( __ret != NULL )
	{
		memset( __ret, 0, num * size );
	}

	return( __ret );
}

#if 0
//! Free the unused kernel memory.
//! \warning Do not use this! It's very buggy and unstable!!!
/**
 *	Unused memory blocks are generated when tasks or threads
 *	do some kmalloc()s and are killed before doing the kfree().
 *	With this routine we free these blocks.
 */
void free_unused_kernel_memory( void )
{
	mem_block_t *p = (mem_block_t *)K_HEAP_START;
	task_t *t;
	uint32_t flags;

	local_irq_save( flags );

	while( p < (mem_block_t *)K_HEAP_END )
	{
		if( (p->flags == M_ALLOC) && (p->owner > 1) )
		{
			t = get_task_info( p->owner );
			if( t == NULL )
			{
				mem_block_t *p2;

				// Free the block.
				p->flags = M_FREE;
				p->owner = NULL;

				// Try to combine the block wih the next one.
				p2 = (void *)p + p->size + sizeof(mem_block_t);
				if( p2 < (mem_block_t *)K_HEAP_END )
				{
					if( p2->flags == M_FREE )
					{
						// Let's merge the two blocks!
						p->size += p2->size + sizeof(mem_block_t);
						p2->magic = NULL;
					}
				}
				// Try to combine the block with the previous one.
				if( p != (mem_block_t *)K_HEAP_START )
				{
					p2 = (mem_block_t *)K_HEAP_START;
					for(;;)
					{
						if( ((void *)p2 + p2->size + sizeof(mem_block_t)) == p )
						{
							// Block found!
							// Check if it's a free block
							if( p2->flags == M_FREE )
							{
								// Let's merge the two blocks!
								p2->size += (p->size + sizeof(mem_block_t));
								p->magic = NULL;
								p = p2;
							}
							break;
						}
						p2 = (void *)p2 + p2->size + sizeof(mem_block_t);
					}
				}
				// Release the physical space occupied by the block.
				deallocate( p );
			}
		}
		// Go to the next memory block.
		p = (void *)p + p->size + sizeof(mem_block_t);
	}
	local_irq_restore( flags );
}
*/
#endif

//! \brief This is a routine for debug only.
//! It prints all the memory block headers.
//! \note Prints the result to the current console.
void dump_memory_map( void )
{
	mem_block_t *p = (mem_block_t *)K_HEAP_START;

	for( ; ; )
	{
		kprintf(
			"\np        = %#010x"
			"\np->magic = %s"
			"\np->flags = %#010x"
			"\np->size  = %u"
			"\np->owner = %i\n",
			(dword)p + sizeof(mem_block_t),
			(byte *)p->magic,
			p->flags,
			p->size,
			p->owner);

		p = (void *)p + p->size + sizeof(mem_block_t);

		if (p >= (mem_block_t *)K_HEAP_END) break;

		// Wait for a key... //
		if( kgetchar() == CTRL_C ) break;
	}
}
