/*!	\file mm/umalloc.c
 *	\brief Virtual Memory Allocator for the User Space.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-03-30 Andrea Righi:
 *			Added user interface for malloc() and free().\n
 *			Added mutual exclusion for the heap area.\n
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <const.h>
#include <arch/paging.h>
#include <kernel/debug.h>
#include <kernel/sched.h>
#include <kernel/semaphore.h>
#include <kernel/task.h>
#include <kernel/video.h>

#include <kernel/umalloc.h>

static __INLINE__ task_t *get_thread_father( task_t *t )
{
	task_t *p = t;

	while( p->flags.type == THREAD_TYPE )
	{
		p = p->father;
		if( p == NULL )
		{
			// The father is dead.
			return( NULL );
		}
	}

	return( p );
}

//! Lock a task heap for mutual exclusion.
static __INLINE__ void lock_heap_region( task_t *t )
{
	DOWN( &(t->heap_sem) );
}

//! Unlock a task heap for mutual exclusion.
static __INLINE__ void unlock_heap_region( task_t *t )
{
	UP( &(t->heap_sem) );
}

//! \brief Initialize the user virtual memory area.
//! \param heap_start The start of the user heap.
//! \param heap_size The size of the user heap.
//! \warning Do not use this routine in a thread initialization!!!
void umalloc_init(task_t *t, size_t heap_start, size_t heap_size)
{
	umem_block_t *p;

	if( t == NULL )
		return;

	if( t->flags.type == THREAD_TYPE )
	{
		printk( KERN_WARNING
			"%s(): threads share the father's heap.\n",
			__FUNCTION__ );
		return;
	}

	// Setup the heap in the task structure.
	t->heap_start = heap_start;
	t->heap_size = heap_size;
	init_MUTEX( &(t->heap_sem) );

	// Create only one hudge block.
	p = (umem_block_t *)heap_start;
	p->magic = UM_MAGIC;
	p->flags = UM_FREE;
	p->size = heap_size - sizeof(umem_block_t);
	p->owner = NULL;
}

//! \brief
//!	Allocate a memory block from the user memory area.
//!	This is a best-fit like allocator.
//! \param size The size of the block you want to allocate.
//! \exception NULL Out-of-memory.
//! \return A pointer to a memory area.
//! \note The memory area is not initialized when it's just allocated.
void *do_umalloc( task_t *ut, size_t size )
{
	umem_block_t *p, *p_best = NULL;
	task_t *t;
	bool sched_is_on = FALSE;

	if( ut == NULL ) return( NULL );
	if ( !size ) return( NULL ); // Is the process crazy?! :-)

	// Round the size up to sizeof(umem_block_t); this is the unit
	// of allocation.
	size = ( size & -(sizeof(umem_block_t)) ) + sizeof(umem_block_t);

	t = get_thread_father( ut );
	if( t == NULL )
	{
		return( NULL );
	}

	// Check if the scheduling is enabled.
	// Otherwise the lock of the heap region can cause a deadlock!
	if( !sched_is_disabled() )
	{
		lock_heap_region( t );
		sched_is_on = TRUE;
	}

	if( t->pdbr != curr_task->pdbr )
	{
		if( sched_is_on )
		{
			sched_enter_critical_region();
		}
		task_switch_mmu( curr_task, t );
	}

	// Explore the heap for the best-fit hole.
	p = (umem_block_t *)(t->heap_start);
	if( !p )
	{
		goto out;
	}
	while(p < (umem_block_t *)(t->heap_start + t->heap_size))
	{
		// Find a free block.
		if (p->flags == UM_FREE)
		{
			// This block is free... let's proceed!
			if (p->size == size)
			{
				// Best fit block found! You're lucky!
				p_best = p;
				break;
			}
			if (p->size > size)
			{
				// The hole is bigger than size...
				// ...check if it's the best-fit hole!
				if (p_best==NULL)
					p_best = p;
				else
					if (p_best->size > p->size)
						p_best = p;
			}
		}
		// Examine the next memory block.
		p = (void *)p+p->size+sizeof(umem_block_t);
	}

	// Have you found a free block?!
	if (p_best == NULL)
	{
		// No free memory :(
		goto out;
	}

	// Found, now allocate it!
	p_best->magic = UM_MAGIC;
	p_best->flags = UM_ALLOC;
	p_best->owner = get_pid();

	if (p_best->size == size)
	{
		// The block cover the size perfectly.
		p_best = (void *)p_best + sizeof(umem_block_t);
		goto out;
	}

	// Otherwise split the hole in two blocks.
	p = (void *)p_best + sizeof(umem_block_t) + size;
	p->magic = UM_MAGIC;
	p->flags = UM_FREE;
	p->size = p_best->size - size - sizeof(umem_block_t);
	p->owner = NULL;

	p_best->size = size;
	p_best = (void *)p_best + sizeof(umem_block_t);

out:
	if( t->pdbr != curr_task->pdbr )
	{
		task_switch_mmu( t, curr_task );
		if( sched_is_on )
		{
			sched_leave_critical_region();
		}
	}

	// Unlock the heap region only if it was previously locked.
	if( sched_is_on )
	{
		unlock_heap_region( t );
	}

	// Return the best-fit pointer.
	return ( (void *)p_best );
}

//! \brief Try to release physical frames occupied by the freed block.
//! \param ptr The pointer to deallocate.
static __INLINE__ void deallocate( umem_block_t *ptr )
{
	size_t start =
		PAGE_ALIGN_UP((uint32_t)((void *)ptr + sizeof(umem_block_t)));
	size_t end =
		PAGE_ALIGN((uint32_t)((void *)ptr + sizeof(umem_block_t) + ptr->size));

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
//!	been returned by a previous call to umalloc(size_t size).
//! \param ptr The pointer you want to free.
void do_ufree( task_t *ut, void *ptr )
{
	umem_block_t *p, *p2;
	task_t *t;
	bool sched_is_on = FALSE;

	if( ut == NULL ) return;
	if( ptr == NULL ) return;

	// Point to the header.
	p = (void *)ptr - sizeof(umem_block_t);

	t = get_thread_father( ut );
	if( t == NULL )
	{
		return;
	}

	// Check if the scheduling is enabled.
	// Otherwise the lock of the heap region can cause a deadlock!
	if( !sched_is_disabled() )
	{
		lock_heap_region( t );
		sched_is_on = TRUE;
	}

	if( t->pdbr != curr_task->pdbr )
	{
		if( sched_is_on )
		{
			sched_enter_critical_region();
		}
		task_switch_mmu( curr_task, t );
	}

	if( p->magic != UM_MAGIC )
		goto out;
	if( p->flags != UM_ALLOC )
		goto out;

	// Free the block.
	p->flags = UM_FREE;
	p->owner = NULL;

	// Try to combine the block wih the next one...
	p2 = (void *)p + p->size + sizeof(umem_block_t);

	if (p2 < (umem_block_t *)(t->heap_start + t->heap_size))
		if (p2->flags == UM_FREE)
		{
			// Let's merge the two blocks!
			p->size += p2->size + sizeof(umem_block_t);
			p2->magic = NULL;
		}

	// Try to combine the block with the previous one...
	if (p != (umem_block_t *)(t->heap_start))
	{
		// Find the previous block.
		p2 = (void *)(t->heap_start);
		for(;;)
		{
			if (((void *)p2 + p2->size + sizeof(umem_block_t)) == p)
			{
				// Block found!
				// Check if it's a free block.
				if (p2->flags == UM_FREE)
				{
					// Let's merge the two blocks!
					p2->size += (p->size + sizeof(umem_block_t));
					p->magic = NULL;
					p = p2;
				}
				break;
			}
			p2 = (void *)p2 + p2->size + sizeof(umem_block_t);
		}
	}

	// Release the physical space occupied by the block.
	deallocate( p );
out:
	if( t->pdbr != curr_task->pdbr )
	{
		task_switch_mmu( t, curr_task );
		if( sched_is_on )
		{
			sched_leave_critical_region();
		}
	}

	// Unlock the heap region only if it was previously locked.
	if( sched_is_on )
	{
		unlock_heap_region( t );
	}
}

//! User interface for malloc().
void *umalloc( size_t size )
{
	register void *__ret;

	__ret = do_umalloc( curr_task, size );

	return( __ret );
}

//! User interface for free().
void ufree( void *ptr )
{
	do_ufree( curr_task, ptr );
}
