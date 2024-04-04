/*!	\file include/arch/asm.h
 *	\brief Common assembler header included from *.S files.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-03-25
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#ifndef ASM_H
#define ASM_H

; //! The selector for the kernel data area.
#define KERNEL_DATA_SEL		0x08

; //! Procedure alignment.
#define ALIGN			.align 16

#endif
