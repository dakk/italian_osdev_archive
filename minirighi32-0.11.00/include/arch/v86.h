/*!	\file include/arch/v86.h
 *	\brief Virtual 8086 mode header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-03
 *	\note Copyright (C) 2003 Andrea Righi
 */

#ifndef V86_H
#define V86_H

#include <arch/exception.h>
#include <kernel/task.h>

//! Virtual-8086 mode flag bit of the EFLAGS register.
#define EFLAGS_VM	0x20000

//! Context of a Virtual 8086 task.
typedef struct v86_context {
	dword 	err,
		ip,
		cs,
		eflags,
		sp,
		ss,
		es,
		ds,
		fs,
		gs;
} v86_context;

// --- Prototypes ----------------------------------------------------- //

/** \ingroup Multitasking
 *  \defgroup MTVirtual8086 Virtual-8086 Mode
 *  The Virtual-8086 mode task manager.
 *  @{
 */

task_t *create_v86_process(void *address, void *buffer, size_t size, char *v86name);
void v86_monitor(exc_context_t *v86context);

/** @} */ // end of MTVirtual8086

#endif
