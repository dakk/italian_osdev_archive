/*!	\file include/arch/i386.h
 *	\brief Generic (IA-32) routines and headers
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-03-14 Andrea Righi:
 *			Added prefetch() operation.\n
 *	\note Copyright (&copy;) 2003 Andrea Righi
 *
 *	This file contains the definition of a lot useful function for
 *	the x86 architecture.
 *	The greater part of them are in assembler...
 */

#ifndef I386_H
#define I386_H

#include <const.h>

/** \ingroup Kernel
 *  \defgroup Ki386Routines Generic (IA-32) Routines
 *  Some useful asm routines for the IA-32 architecture.
 *  @{
 */

//! \brief Clear task switch flag.
static __INLINE__ void clts()
{
	__asm__ __volatile__ ("clts");
}

//! \brief Set task switch flag.
static __INLINE__ void slts()
{
	unsigned int __cr0;
	__asm__ __volatile__ (
		"movl %%cr0, %0\n"
		"or $8, %0\n"
		"movl %0, %%cr0" : "=r"(__cr0));
}

//! \brief
//!	Read the timestamp counter from the model-specific register
//!	and put it into two 32-bit registers.
#define rdtsc(low, high) \
	__asm__ __volatile__( "rdtsc" : "=a"(low), "=d"(high) )

//! \brief
//!	Read the timestamp counter from the model-specific register
//!	and put it into eax:edx registers. This is a 64-bit value.
#define rdtscll(val) \
	__asm__ __volatile__( "rdtsc" : "=A"(val) )

//! \brief
//!	Memory barrier: force strict CPU ordering (required for all
//!	out-of-order Intel CPUs to preserve the order of execution).
#define barrier() \
	__asm__ __volatile__ ("lock; addl $0,0(%%esp)": : :"memory")

#if !defined(CONFIG_M386)
//! \brief Write back and invalidate cache.
#define wbinvd() \
	__asm__ __volatile__ ("wbinvd" : : : "memory");
#endif

//! \brief No operation.
#define nop() \
	__asm__ __volatile__ ("nop");

//! \brief
//!	A macro that returns current instruction pointer.
#define get_eip() ({ void *eip; __asm__("movl $1f,%0\n1:" : "=g" (eip)); eip; })

// --- INTERRUPT CONTROL ---------------------------------------------- //

//! \brief Enable all interrupts.
#define enable()	__asm__ __volatile__ ("sti" : : : "memory");

//! \brief Disable all interrupts.
#define disable()	__asm__ __volatile__ ("cli" : : : "memory");

//! Save EFLAGS register.
#define save_flags( flags ) \
	__asm__ __volatile__ ("pushfl ; popl %0" : "=g"(flags) : )

//! Restore EFLAGS register.
#define restore_flags( flags ) \
	__asm__ __volatile__ ("pushl %0 ; popfl" : : "g"(flags) : "memory", "cc")

//! Interrupt control: save flags and disable irqs.
#define local_irq_save(flags) \
	__asm__ __volatile__ ("pushfl ; popl %0 ; cli" : "=g"(flags) : : "memory")

//! Interrupt control: save flags and enable irqs.
#define local_irq_set(flags) \
	__asm__ __volatile__ ("pushfl ; popl %0 ; sti" : "=g"(flags) : : "memory")

//! Interrupt control: restore flags.
#define local_irq_restore(flags)	restore_flags(flags)

//! \brief Halt the CPU until an interrupt occurs.
#define idle() \
	__asm__ __volatile__ ("hlt" : : : "memory");

//! Secure idle.
#define safe_idle() \
	do { enable(); idle();  } while(0)

// --- inport#x --------------------------------------------------------//

//! A macro equivalent to the inportb(word port) function.
#define in	inportb
//! A macro equivalent to the inportb(word port) function.
#define inb	inportb
//! A macro equivalent to the inportw(word port) function.
#define in16	inportw
//! A macro equivalent to the inportw(word port) function.
#define inw	inportw
//! A macro equivalent to the inportl(word port) function.
#define in32	inportl
//! A macro equivalent to the inportl(word port) function.
#define inl	inportl

//! \brief
//!	Get a \c byte from an I/O port.
//! \param port The I/O port.
//! \return The value of the byte.
static __INLINE__ byte inportb(word port)
{
	// Get a byte from I/O port						//
	register uint8_t val;
	__asm__ __volatile__ ("inb %%dx, %%al" : "=a" (val) : "d" (port));
        return( val );
}

//! \brief
//!	Get a \c word from an I/O port.
//! \param port The I/O port.
//! \return The value of the word.
static __INLINE__ word inportw(word port)
{
	register uint16_t val;
	__asm__ __volatile__ ("inw %%dx, %%ax" : "=a" (val) : "d" (port));
        return( val );
}

//! \brief
//!	Get a \c dword from an I/O port.
//! \param port The I/O port.
//! \return The value of the double word.
static __INLINE__ dword inportl(word port)
{
	register uint32_t val;
	__asm__ __volatile__ ("inl %%dx, %%eax" : "=a" (val) : "d" (port));
        return( val );
}

// --- outport#x -------------------------------------------------------//

//! A macro equivalent to the outportb(word port, byte val) function.
#define out	outportb
//! A macro equivalent to the outportb(word port, byte val) function.
#define outb	outportb
//! A macro equivalent to the outportw(word port, word val) function.
#define out16	outportw
//! A macro equivalent to the outportw(word port, word val) function.
#define outw	outportw
//! A macro equivalent to the outportl(word port, dword val) function.
#define out32	outportl
//! A macro equivalent to the outportl(word port, dword val) function.
#define outl	outportl

//! \brief
//!	Put a \c byte to an I/O port.
//! \param port The I/O port.
//! \param val The value you want to put.
static __INLINE__ void outportb(word port, byte val)
{
	__asm__ __volatile__ ("outb %%al, %%dx" : : "d" (port), "a" (val));
}

//! \brief
//!	Put a \c word to an I/O port.
//! \param port The I/O port.
//! \param val The value you want to put.
static __INLINE__ void outportw(word port, word val)
{
	__asm__ __volatile__ ("outw %%ax, %%dx" : : "d" (port), "a" (val));
}

//! \brief
//!	Put a \c dword to an I/O port.
//! \param port The I/O port.
//! \param val The value you want to put.
static __INLINE__ void outportl(word port, dword val)
{
	__asm__ __volatile__ ("outl %%eax, %%dx" : : "d" (port), "a" (val));
}

// --- ins* ------------------------------------------------------------//

//! \brief
//!	The string version of in().
//! \param port The I/O port.
//! \param addr A memory buffer where the values will be copied.
//! \param count How many bytes we want to read from the port.
/*!	Copy a sequence of \a count bytes to the buffer \a addr from
 *	the port \a port.
 */
static inline void insb(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ ("rep ; insb": "=D"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

//! \brief
//!	The string version of in16().
//! \param port The I/O port.
//! \param addr A memory buffer where the values will be copied.
//! \param count How many words we want to read from the port.
/*!	Copy a sequence of \a count words to the buffer \a addr from
 *	the port \a port.
 */
static inline void insw(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ ("rep ; insw": "=D"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

//! \brief
//!	The string version of in32().
//! \param port The I/O port.
//! \param addr A memory buffer where the values will be copied.
//! \param count How many double words we want to read from the port.
/*!	Copy a sequence of \a count double words to the buffer \a addr
 *	from the port \a port.
 */
static inline void insl(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ ("rep ; insl": "=D"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

//! A macro equivalent to the
//! insb(unsigned short port, void * addr, unsigned long count) function.
#define inportbm	insb
//! A macro equivalent to the
//! insw(unsigned short port, void * addr, unsigned long count) function.
#define inportwm	insw
//! A macro equivalent to the
//! insl(unsigned short port, void * addr, unsigned long count) function.
#define inportlm	insl

// --- outs* -----------------------------------------------------------//

//! \brief
//!	The string version of out().
//! \param port The I/O port.
//! \param addr A memory buffer that contains the values to be copied.
//! \param count How many bytes we want to write to the port.
/*!	Copy a sequence of \a count bytes to the port \a port
 *	from the buffer \a addr.
 */
static inline void outsb(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ ("rep ; outsb": "=S"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

//! \brief
//!	The string version of out16().
//! \param port The I/O port.
//! \param addr A memory buffer that contains the values to be copied.
//! \param count How many words we want to write to the port.
/*!	Copy a sequence of \a count words to the port \a port
 *	from the buffer \a addr.
 */
static inline void outsw(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ ("rep ; outsw": "=S"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

//! \brief
//!	The string version of out32().
//! \param port The I/O port.
//! \param addr A memory buffer that contains the values to be copied.
//! \param count How many double words we want to write to the port.
/*!	Copy a sequence of \a count double words to the port \a port
 *	from the buffer \a addr.
 */
static inline void outsl(unsigned short port, void * addr, unsigned long count)
{
	__asm__ __volatile__ ("rep ; outsl": "=S"(addr), "=c"(count) : "d"(port), "0"(addr), "1"(count));
}

//! A macro equivalent to the
//! outsb(unsigned short port, void * addr, unsigned long count) function.
#define outportbm	outsb
//! A macro equivalent to the
//! outsw(unsigned short port, void * addr, unsigned long count) function.
#define outportwm	outsw
//! A macro equivalent to the
//! outsl(unsigned short port, void * addr, unsigned long count) function.
#define outportlm	outsl

//! Slow I/O mode. Some hardware really needs it!
#define slow_down_io()	__asm__ __volatile__ ("jmp 1f; 1: jmp 1f; 1:" : : )

//! Slow down output to a port.
static __INLINE__ void outb_p( unsigned short port, int value )
{
	outb( port, value );
	slow_down_io();
}

//! Slow down output to a port.
static __INLINE__ void outw_p( unsigned short port, int value )
{
	outw( port, value );
	slow_down_io();
}

//! Slow down output to a port.
static __INLINE__ void outl_p( unsigned short port, int value )
{
	outl( port, value );
	slow_down_io();
}

//! Slow down intput from a port.
static __INLINE__ byte inb_p( unsigned short port )
{
	int __ret = inb( port );
	slow_down_io();
	return( __ret );
}

//! Slow down intput from a port.
static __INLINE__ word inw_p( unsigned short port )
{
	int __ret = inw( port );
	slow_down_io();
	return( __ret );
}

//! Slow down intput from a port.
static __INLINE__ dword inl_p( unsigned short port )
{
	int __ret = inl( port );
	slow_down_io();
	return( __ret );
}

// --- TASK SWITCHING ------------------------------------------------- //

//! \brief
//!	Perform a context switch between two tasks.
//! \param tss_sel
//!	The selector of the next task.
/*!
 *	This is an asm routine to jump to a TSS (TSS-based multitasking)
 */
static __INLINE__ void jmp_to_tss(word tss_sel)
{
	static struct {
	unsigned eip : 32; // 32 bit
	unsigned cs  : 16; // 16 bit
	} __attribute__ ((packed)) tss_link = {0, 0};

	// Set the TSS link						//
	tss_link.cs = tss_sel;

	// Jump to the task						//
	__asm__ __volatile__ ("ljmp *(%0)" : : "m" (tss_link));
}

// --- CPUID -----------------------------------------------------------//

//! \brief
//!	Get the CPUID information.
//! \param op The operation code to perform.
//! \param eax EAX register value after the CPUID execution.
//! \param ebx EBX register value after the CPUID execution.
//! \param ecx ECX register value after the CPUID execution.
//! \param edx EDX register value after the CPUID execution.
//! \warning
//!	Not all the Intel CPUs support the CPUID instruction!!!
//!	Only some Intel486 family and subsequent Intel processors
//!	provide this method for determinig the architecture flags.
//!	Execution of CPUID on a processor that does not support this
//!	instruction will result in an invalid opcode exception.
//!	\n \n
//!	To determine if it is possible to use this instruction we can
//!	use bit 21 of the EFLAGS register. If software can change the
//!	value of this flag, the CPUID instruction is executable.
static __INLINE__ void cpuid(int op, int *eax, int *ebx, int *ecx, int *edx)
{
	__asm__ __volatile__(
		"cpuid"
		:
		"=a" (*eax),
		"=b" (*ebx),
		"=c" (*ecx),
		"=d" (*edx)
		:
		"0" (op)
	);
	__asm__ __volatile__ ("" : : : "eax", "ebx", "ecx", "edx");
}

//! \brief Halt the system by disabling the CPU.
//! \warning
//!	This is truly the end. You have to reboot the system
//!	after this...
static __INLINE__ void halt()
{
	__asm__ __volatile__ ("cli\n hlt");
}

//! \brief Pause - this is a good thing to insert into busy-wait loops.
static __INLINE__ void rep_nop(void)
{
	__asm__ __volatile__ ("rep; nop" : : : "memory");
}

#if defined(CONFIG_MPENTIUMIII) || defined(CONFIG_MPENTIUM4)
//! Prefetch data into caches.
//! \warning
//!	Only for Pentium III, Pentium IV and AMD Athlon processors.
static __INLINE__ void prefetch( const void *p )
{
	__asm__ __volatile__ ("prefetchnta (%0)" : : "r"(p));
}
#elif defined(CONFIG_MK7) || defined(CONFIG_MATHLONXP)
//! Prefetch data into caches.
//! \warning
//!	Only for Pentium III, Pentium IV and AMD Athlon processors.
static __INLINE__ void prefetch( const void *p )
{
	__asm__ __volatile__ ("prefetch (%0)" : : "r"(p));
}
#else
//! Prefetch data into caches.
//! \warning
//!	Only for Pentium III, Pentium IV and AMD Athlon processors.
static __INLINE__ void prefetch( const void *p ) { ; }
#endif

// --- Debug operators ------------------------------------------------- //

//! \brief Perform a breakpoint exeception.
static __INLINE__ void breakpoint()
{
	__asm__ __volatile__ ("int3" : : : "memory");
}

/** @} */ // end of Ki386Routines

#endif
