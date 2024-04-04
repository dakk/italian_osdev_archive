/*!	\file kernel/cow.c
 *	\brief Copy On Write paging
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-11-22\n
 *		2004-12-08 Andrea Righi:
 *			Fixed a deadlock bug.\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <errno.h>
#include <types.h>
#include <kernel/queue.h>
#include <kernel/semaphore.h>
#include <kernel/cow.h>

//! Shared pages queue.
DECLARE_QUEUE(cow_pages);

//! Set a page shared between 2 or more tasks.
int set_shared_page(size_t addr) {
	cow_t *p;
	queue_t *entry;
	int n;
	unsigned long flags;

	local_irq_save(flags);

	// Search the page on the COW pages queue.
	queue_for_each (entry, n, cow_pages) {
		p = queue_get_entry(entry);
		if (p->addr == addr) {
			// This page is already a COW page!
			// Let's update the shared counter.
			int __ret = ++(p->count);
			local_irq_restore(flags);
			return __ret;
		}
	}

	// Add the new page to the COW shared pages.
	p = kmalloc(sizeof(cow_t), GFP_ATOMIC);
	if (p == NULL) {
		local_irq_restore(flags);
		return -ENOMEM;
	}
	p->addr = addr;
	// The page is shared at least with two tasks.
	p->count = 1;
	if (add_queue(&cow_pages, p) < 0) {
		kfree(p);
		local_irq_restore(flags);
		return -ENOMEM;
	}
	local_irq_restore(flags);
	return 1;
}

//! Unset a page shared between 2 or more tasks.
int unset_shared_page(size_t addr) {
	cow_t *p;
	queue_t *entry;
	int n;
	unsigned long flags;

	local_irq_save(flags);

	// Search the page on the COW pages queue.
	queue_for_each (entry, n, cow_pages) {
		p = queue_get_entry(entry);
		if (p->addr == addr) {
			int __ret = (p->count)--;
			if (!__ret) {
				kfree(p);
				queue_del_entry(&cow_pages, entry);
			}
			local_irq_restore(flags);
			return __ret;
		}
	}

	local_irq_restore(flags);
	// The page is not present in the COW pages queue.
	return 0;
}
