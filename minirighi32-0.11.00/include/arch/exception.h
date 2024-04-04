/*!	\file include/arch/exception.h
 *	\brief Exceptions header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-18
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <const.h>

//! Context after an exception.
typedef struct exc_context
{
	//! General purpose register.
	uint32_t	edi, esi, ebp, ebx, edx, ecx, eax;
	//! Segment register.
	uint32_t	es, ds;
	//! The exception number and error code.
	uint32_t	EXC, err_code;
	//! Current instruction pointer (a.k.a. program counter).
	uint32_t	eip;
	//! Code segment register.
	uint32_t	cs;
	//! EFLAGS register.
	uint32_t	eflags;
} __attribute__ ((packed)) exc_context_t;

//! Context after an exception.
typedef struct exc_user_context
{
	//! General purpose register.
	uint32_t	edi, esi, ebp, ebx, edx, ecx, eax;
	//! Segment register.
	uint32_t	es, ds;
	//! The exception number and error code.
	uint32_t	EXC, err_code;
	//! Current instruction pointer (a.k.a. program counter).
	uint32_t	eip;
	//! Code segment register.
	uint32_t	cs;
	//! EFLAGS register.
	uint32_t	eflags;
	//! User stack pointer register.
	uint32_t	uesp;
	//! User stack segment register.
	uint32_t	uss;
} __attribute__ ((packed)) exc_user_context_t;

#endif
