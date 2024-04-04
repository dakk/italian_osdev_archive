/*!	\file include/arch/mem.h
 *	\brief Low level (i386) memory manager header.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-01-04 Andrea Righi:
 			Improved assembler routies.\n
 *	\note Copyright (C) 2003 Andrea Righi
 */

#ifndef MEM_H
#define MEM_H

#include <const.h>

// Kernel virtual memory map.
#include <kernel/kernel_map.h>

/** \ingroup Memory
 *  \defgroup KLowLevelMemoryManager Low-Level Memory Manager
 *  The low level memory operators (based on i386-architecture).
 *  @{
 */

//! Number of entries in the GDT (Global Descriptor Table).
#define GDT_DIM 	8192
//! GDT entry dimension in bytes.
#define GDT_ENTRY_DIM	sizeof(gdt_entry_t)

// Segment Constants //
#define PRESENT		0x80	//!< The segment is present.
#define CODE		0x18	//!< A code segment type.
#define DATA		0x10	//!< A data segment type.
#define EXP_DOWN	0x04	//!< The segment grows downward.
#define CONFORMING	0x04	//!< A conforming code segment.
#define READABLE	0x02	//!< A readable segment.
#define WRITEABLE	0x02	//!< A writeable segment.

//! A standard code segment.
#define CODE_SEG	(CODE | PRESENT | READABLE)
//! A standard data segment.
#define DATA_SEG	(DATA | PRESENT | WRITEABLE)
//! A standard stack segment.
#define STACK_SEG	DATA_SEG
//! A standard TSS segment.
#define TSS_SEG		(PRESENT | 0x09)
//! A real-mode area segment.
#define REAL_SEG	(CODE | PRESENT | READABLE)

#define INT_GATE	(PRESENT | 0x0E) //!< 32-bit Interrupt Gate.
#define TASK_GATE	(PRESENT | 0x05) //!< Task Gate.
#define TRAP_GATE	(PRESENT | 0x0F) //!< 32-bit Trap Gate.

//! Descriptor Privilege Level 0 (maximum privilege).
#define DPL_0		0x00
//! Descriptor Privilege Level 1.
#define DPL_1		0x20
//! Descriptor Privilege Level 2.
#define DPL_2		0x40
//! Descriptor Privilege Level 3 (minimum privilege).
#define DPL_3		0x60

//! The busy bit of the TSS descriptor (to avoid recursive tasks).
#define BUSY_FLAG	0x02

//! Default size of the istructions (for a code segment) or
//! the stack alignment (for a stack segment).
#define ATTR_32BIT      0x40
//! The granularity of the segment size (limit field):
//! \li	0 => byte granularity;
//! \li 1 => page granularity (4096 bytes).
#define ATTR_GRANUL     0x80

//! This is the data selector for the kernel.
//! REMEBER!!! It is defined also into irq.S!
#define KERNEL_DATA		0x08
//! This is the stack selector for the kernel.
#define KERNEL_STACK		0x08
//! This is the code selector for the kernel.
#define KERNEL_CODE		0x10

//! This is the selector for the user data segment.
#define USER_DATA		0x18
//! This is the selector for the user stack segment.
#define USER_STACK		0x18
//! This is the selector for the user code segment.
#define USER_CODE		0x20

//! This is the selector for the double fault exception task.
#define GDT_DOUBLE_FAULT_ENTRY	0x28

//! (real-mode) Extract the segment part of a linear address.
#define SEGMENT(linear)		( (word)(((dword)(linear) & 0xFFFF0000) >> 4) )
//! (real-mode) Extract the offset part of a linear address.
#define OFFSET(linear)		( (word)((dword)(linear) & 0xFFFF) )
//! (real-mode) Make a linear address from a segment:offset address.
#define LINEAR(seg, off)	( (dword)(((word)(seg) << 4) + (word)(off)) )

//! (i386) far pointer.
typedef uint32_t FARPTR;
//! (i386) far pointer.
typedef uint32_t far;

//! (i386) Make a far pointer from \p seg segment and \p off offset.
//! \note In this case a segment is considered as 65536(0x10000) bytes.
#define MK_FP(seg, off)		((FARPTR) (((uint32_t) (seg) << 16) | (uint16_t) (off)))
//! (i386) Get the segment from the far pointer \p fp.
#define FP_SEG(fp)		(((FARPTR) fp) >> 16)
//! (i386) Get the offset from the far pointer \p fp.
#define FP_OFF(fp)		(((FARPTR) fp) & 0xffff)
//! Translate a real-mode segment:offset address to the linear address.
//! This macro is equivalent to LINEAR(seg, off) macro.
#define FP_TO_LINEAR(seg, off)	((void*) ((((uint16_t) (seg)) << 4) + ((uint16_t) (off))))

//! GDT structure.
/** P=Present, DPL=Descriptor Privilege Level, S=Descriptor type (0=system; 1=code or data),
 * Type=Segment type, G=Granularity, D/B=Default operation size(0=16bit; 1=32bit segment),
 * AVL=Available for use by system software.
 */
typedef struct gdt_entry
{
	word limit;
	word base0_15;
	byte base16_23;
	byte attribs0_7;  //!< P(1bit) | DPL(2bit) | S(1bit) | Type(4bit)
	byte attribs8_15; //!< G(1bit) | D/B(1bit) | 0(1bit) | AVL(1bit) | limit16_19(4bit)
	byte base24_31;
} __attribute__ ((packed)) gdt_entry_t;

//! The GDT register format.
typedef struct gdt_register
{
	uint16_t limit;
	uint32_t base;
} __attribute__ ((packed)) gdt_reg;

//! \brief Update segment registers with kernel selectors.
static __INLINE__ void k_update_segment_regs()
{
	__asm__ __volatile__(
		"ljmp %0, $1f\n"
		"nop\n"
		"1: mov %1, %%ds\n"
		"mov %1, %%es\n"
		"mov %1, %%ss\n"
		"nop\n"
		"mov %1, %%fs\n"
		"mov %1, %%gs\n"
		: : "i"(KERNEL_CODE), "r"(KERNEL_DATA) );
}

word setup_GDT_entry(word limit, dword base, byte attribs0_7, byte attribs8_15);
void remove_GDT_entry(word sel);
void install_GDT();

/** @} */ // end of KLowLevelMemoryManager

#endif
