/*!	\file include/kernel/cow.h
 *	\brief Headers for COW paging.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-11-22\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#ifndef COW_H
#define COW_H

//! COW page structure.
typedef struct cow {
	size_t addr;
	int count;
} cow_t;

int set_shared_page(size_t addr);
int unset_shared_page(size_t addr);

#endif
