/*!	\file include/kernel/queue.h
 *	\brief Circular queues header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-03-15 Andrea Righi:
 *			Restructured queue operators improving
 *			performances; changed simple queues
 *			into doubley linked queues and added
 *			wait queues.\n
 *	\note Copyright (&copy;) 2003-2004 Andrea Righi
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <const.h>
#include <errno.h>
#include <arch/i386.h>
#include <arch/spinlock.h>
#include <kernel/kmalloc.h>

// -------------------------------------------------------------------- //
// --- STANDARD QUEUES ------------------------------------------------ //
// -------------------------------------------------------------------- //

//! Doubley linked queue structure.
typedef struct queue
{
	void *value;		//!< The value of the element.
	struct queue *next;	//!< A pointer to the next element.
	struct queue *prev;	//!< A pointer to the previous element.
} queue_t;

//! Declare a doubley linked circular queue.
#define DECLARE_QUEUE( name ) \
	queue_t *name = NULL

//! Delete the current entry into the queue.
static __INLINE__ void __queue_del_entry( queue_t **q, queue_t *entry )
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;

	if( entry==(*q) )
	{
		(*q) = (*q)->next;
		if( entry == (*q) )
			*q = NULL;
	}

	kfree( entry );
}

//! Delete the current entry into the queue safe against empty queues.
static __INLINE__ int queue_del_entry( queue_t **q, queue_t *entry )
{
	if( *q == NULL )
		return( -1 );

	__queue_del_entry( q, entry );

	return( 0 );
}

//! Delete the head of the queue.
static __INLINE__ int queue_del_head( queue_t **q )
{
	return( queue_del_entry(q, *q) );
}

//! Move an element from a queue to another queue.
static __INLINE__ int queue_move_entry( queue_t **to, queue_t **from, queue_t *entry )
{
	if( *from == NULL )
		return( -1 );

	// Unlink the entry.
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;

	// Update the head of the source queue.
	if( entry == (*from) )
	{
		(*from) = (*from)->next;
		if( entry == (*from) )
			*from = NULL;
	}
	// Put the entry into the destination queue.
	if ( *to == NULL )
	{
		entry->next = entry->prev = entry;
		*to = entry;
	}
	else
	{
		entry->prev = (*to)->prev;
		entry->next = (*to);
		(*to)->prev->next = entry;
		(*to)->prev = entry;
	}

	return( 0 );
}

//! Get the value of the entry.
static __INLINE__ void *queue_get_entry( queue_t *entry )
{
	return( (entry != NULL) ? entry->value : NULL );
}

//! \brief Get the number of elements in the queue.
static __INLINE__ int count_queue(queue_t **q)
{
	queue_t *head = *q, *tmp = *q;
	register int count = 0;

	if( head != NULL )
	{
		while( (tmp != head) || !(count) )
		{
			tmp = tmp->next;
			prefetch( tmp->next );
			count++;
		}
	}

	return( count );
}

//! Iterate over a queue.
//! \param pos The queue_t * to use as a loop counter.
//! \param c A counter for elements explored.
//! \param head The head of the queue.
#define queue_for_each( pos, c, head ) \
	if( (head) != NULL ) \
		for( pos = (head), c = 0, prefetch(pos->next); \
			(pos != (head)) || !(c); \
			pos = pos->next, prefetch(pos->next), ++c )

//! Iterate over a queue backward.
//! \param pos The queue_t * to use as a loop counter.
//! \param c A counter for elements explored.
//! \param head The head of the queue.
#define queue_for_each_prev( pos, c, head ) \
	if( (head) != NULL ) \
		for( pos = (head), c = 0, prefetch(pos->prev); \
			(pos != (head)) || !(c); \
			pos = pos->prev, prefetch(pos->prev), ++c )

//! Iterate over a queue safe against removal of the current
//! queue entry.
//! \param pos The queue_t * to use as a loop counter.
//! \param n Another queue_t * to use as a temporary storage.
//! \param c A counter for elements explored.
//! \param head The head of the queue.
//! \warning At the end of the iteration c is ever 0!!!
#define queue_for_each_safe( pos, n, c, head ) \
	if( (head) != NULL ) \
		for( pos = (head), n = pos->next, \
			c = count_queue( &(head) ); \
			c--; \
			pos = n, n = pos->next )

//! \brief Get the first element in the queue and update the head.
//! \param q The queue pointer.
//! \return The next element in the queue.
//! \exception NULL The queue is empty or not allocated.
static __INLINE__ void *pick_queue(queue_t **q)
{
	void *__ret;

	if ( *q==NULL )
		return( NULL );

	__ret = (*q)->value;
	*q = (*q)->next;

	return( __ret );
}

//! \brief Add a new element to a circular queue.
//! \param q The queue pointer.
//! \param v The value of the element.
//! \return 0 if success, otherwise return the error code.
static __INLINE__ int add_queue( queue_t **q, void *v )
{
	queue_t *p;

	p = (queue_t *)kmalloc( sizeof(queue_t), GFP_ATOMIC );
	if (p == NULL)
		return( -ENOMEM );

	p->value = v;

	if ( *q==NULL )
	{
		p->next = p->prev = p;
		*q = p;
	}
	else
	{
		p->prev = (*q)->prev;
		p->next = (*q);
		(*q)->prev->next = p;
		(*q)->prev = p;
	}
	return( 0 );
}

//! \brief Add a new element to a circular queue after the head.
//! \param q The queue pointer.
//! \param v The value of the element.
//! \return 0 if success, otherwise return the error code.
//! \note Useful for implementing stacks.
static __INLINE__ int add_queue_head( queue_t **q, void *v )
{
	queue_t *p;

	p = (queue_t *)kmalloc( sizeof(queue_t), GFP_ATOMIC );
	if (p == NULL)
		return( -ENOMEM );

	p->value = v;

	if ( *q==NULL )
	{
		p->next = p->prev = p;
		*q = p;
	}
	else
	{
		p->next = (*q)->next;
		p->prev = (*q);
		(*q)->next->prev = p;
		(*q)->next = p;
	}
	return( 0 );
}

//! \brief Remove an element from a circular queue.
//! \param q The queue pointer.
//! \param v The value of the element to remove.
//! \return 0 if the element is correctly removed, -1 otherwise.
//! \note
//!	We assume that the value of the elements are univocal
//!	in the queue.
//!	Otherwise this routine removes the first occurrence.
static __INLINE__ int rem_queue( queue_t **q, void *v )
{
	queue_t *t, *_tmp;
	int n;

	// Search (safe) the element 'v'.
	queue_for_each_safe( t, _tmp, n, *q )
	{
		if( t->value==v )
		{
			// Element found.
			__queue_del_entry( q, t );
			return( 0 );
		}
	}
	return( -1 );
}

//! \brief Clear evey element from a circular queue.
//! \param q The queue pointer.
//! \return
//!	The number of elements removed from the queue.
static __INLINE__ int clear_queue( queue_t **q )
{
	queue_t *_tmp, *t;
	int count;

	// Safe iteration of the queue.
	queue_for_each_safe( t, _tmp, count, *q )
	{
		__queue_del_entry( q, t );
	}
	return( count );
}

//! \brief Find an element into a circular queue.
//! \param q The queue pointer.
//! \param v The value of the element to find.
//! \return
//!	\li The entry in the queue if the element if found;
//!	\li #NULL if the element is not present.
//! \note
//!	We assume that the value of the elements are univocal
//!	in the queue.
//!	Otherwise this routine finds the first occurrence.
static __INLINE__ void *find_queue(queue_t **q, void *v)
{
	queue_t *t;
	int count;

	// Search the element 'v'.
	queue_for_each( t, count, *q )
	{
		if( t->value==v )
		{
			// Element found.
			// Return the entry of the queue.
			return( t );
		}
	}
	return( NULL );
}

// -------------------------------------------------------------------- //
// --- WAIT QUEUES ---------------------------------------------------- //
// -------------------------------------------------------------------- //

//! Wait queue structure.
typedef struct wait_queue
{
	spinlock_t lock;
	queue_t *list;
} wait_queue_t;

//! Declare a wait queue.
#define DECLARE_WAITQUEUE( name ) \
	wait_queue_t name = { SPINLOCK_UNLOCKED, NULL }

//! Add an element into the wait queue.
static __INLINE__ int add_wait_queue( wait_queue_t *q, void *v )
{
	unsigned long flags;
	int __ret;
	spin_lock_irqsave( &(q->lock), flags );
	__ret = add_queue( &(q->list), v );
	spin_unlock_irqrestore( &(q->lock), flags );
	return( __ret );
}

//! Remove an element from the wait queue.
static __INLINE__ int remove_wait_queue( wait_queue_t *q, void *v )
{
	unsigned long flags;
	int __ret;
	spin_lock_irqsave( &(q->lock), flags );
	__ret = rem_queue( &(q->list), v );
	spin_unlock_irqrestore( &(q->lock), flags );
	return( __ret );
}

#endif
