/*!	\file include/arch/mmx.h
 *	\brief Generic MMX and SSE/SSE2 extensions.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-08-25
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#ifndef MMX_H
#define MMX_H

#include <const.h>
#include <arch/i386.h>
#include <arch/interrupt.h>
#include <arch/paging.h>

#if defined(CONFIG_M586MMX) || defined(CONFIG_MPENTIUMII) ||  \
	defined(CONFIG_MK7) || defined(CONFIG_MATHLONXP)
#define ARCH_HAS_MMX
#elif defined(CONFIG_MPENTIUMIII)
#define ARCH_HAS_SSE
#elif defined(CONFIG_MPENTIUM4)
#define ARCH_HAS_SSE2
#endif

#if defined(ARCH_HAS_SSE2)

//! \brief Clear a page using SSE2 extensions.
static __INLINE__ void fast_clear_page(void *page)
{
	int i;

	// Clear the task switch flag.
	clts();

	// Clear the 128-bit xmm0 register.
	__asm__ __volatile__ ("pxor %xmm0, %xmm0");

	// Fast clear the page movntdq.
	for(i=0; i<PAGE_SIZE/128; i++) {
		__asm__ __volatile__ (
			"movntdq %%xmm0, (%0)\n"
			"movntdq %%xmm0, 16(%0)\n"
			"movntdq %%xmm0, 32(%0)\n"
			"movntdq %%xmm0, 48(%0)\n"
			"movntdq %%xmm0, 64(%0)\n"
			"movntdq %%xmm0, 80(%0)\n"
			"movntdq %%xmm0, 96(%0)\n"
			"movntdq %%xmm0, 112(%0)\n"
		: : "r"(page) : "memory");
		page += 128;
	}

	// Since movntdq is weakly-ordered, a "mfence" is needed.
	__asm__ __volatile__ ("mfence" : : : "memory");

	// Restore task switch flag.
	slts();
}

//! \brief Copy a page using SSE2 extensions.
static __INLINE__ void fast_copy_page(void *to, void *from)
{
	int i;

	// Clear the task switch flag.
	clts();

	__asm__ __volatile__ (
	        "prefetchnta (%0)\n"
	        "prefetchnta 64(%0)\n"
	        "prefetchnta 128(%0)\n"
        	"prefetchnta 192(%0)\n"
		: : "r" (from) );

	for(i=0; i<PAGE_SIZE/16; i++) {
		__asm__ __volatile__ (
			"prefetchnta 168(%0)\n"
			"movaps (%0), %%xmm0\n"
			"movntdq %%xmm0, (%1)\n"
			: : "r" (from), "r" (to) : "memory");
		from += 64;
		to += 64;
	}

	// Since movntdq is weakly-ordered, a "mfence" is needed.
	__asm__ __volatile__ ("mfence" : : : "memory");

	// Restore task switch flag.
	slts();
}

#elif defined(ARCH_HAS_SSE)

//! \brief Clear a page using SSE extensions.
static __INLINE__ void fast_clear_page(void *page)
{
	int i;

	// Clear the task switch flag.
	clts();

	// Clear the 128-bit xmm0 register.
	__asm__ __volatile__ ("pxor %xmm0, %xmm0");

	// Fast clear the page movntdq.
	for(i=0; i<PAGE_SIZE/128; i++) {
		__asm__ __volatile__ (
			"movntdq %%xmm0, (%0)\n"
			"movntdq %%xmm0, 16(%0)\n"
			"movntdq %%xmm0, 32(%0)\n"
			"movntdq %%xmm0, 48(%0)\n"
			"movntdq %%xmm0, 64(%0)\n"
			"movntdq %%xmm0, 80(%0)\n"
			"movntdq %%xmm0, 96(%0)\n"
			"movntdq %%xmm0, 112(%0)\n"
		: : "r"(page) : "memory");
		page += 128;
	}

	// Since movntdq is weakly-ordered, a "sfence" is needed.
	__asm__ __volatile__ ("sfence" : : : "memory");

	// Restore task switch flag.
	slts();
}

//! \brief Copy a page using SSE extensions.
static __INLINE__ void fast_copy_page(void *to, void *from)
{
	int i;

	// Clear the task switch flag.
	clts();

	__asm__ __volatile__ (
	        "prefetchnta (%0)\n"
	        "prefetchnta 64(%0)\n"
	        "prefetchnta 128(%0)\n"
        	"prefetchnta 192(%0)\n"
		: : "r" (from) );

	for(i=0; i<PAGE_SIZE/16; i++) {
		__asm__ __volatile__ (
			"prefetchnta 168(%0)\n"
			"movaps (%0), %%xmm0\n"
			"movntdq %%xmm0, (%1)\n"
			: : "r" (from), "r" (to) : "memory");
		from += 64;
		to += 64;
	}

	// Since movntdq is weakly-ordered, a "sfence" is needed.
	__asm__ __volatile__ ("sfence" : : : "memory");

	// Restore task switch flag.
	slts();
}

#elif defined(ARCH_HAS_MMX)

//! \brief Clear a page using MMX extensions.
static __INLINE__ void fast_clear_page(void *page)
{
	int i;

	// Clear the task switch flag.
	clts();

	// Clear the 64-bit mm0 register.
	__asm__ __volatile__ ("pxor %mm0, %mm0");
#if (defined(CONFIG_MK7) || defined(CONFIG_MATHLONXP))
	// The K7 has streaming cache bypass load/store. The Cyrix III, K6 and
	// other MMX using processors do not.
	for(i=0; i<PAGE_SIZE/64; i++) {
		__asm__ __volatile__ (
			"movntq %%mm0, (%0)\n"
			"movntq %%mm0, 8(%0)\n"
			"movntq %%mm0, 16(%0)\n"
			"movntq %%mm0, 24(%0)\n"
			"movntq %%mm0, 32(%0)\n"
			"movntq %%mm0, 40(%0)\n"
			"movntq %%mm0, 48(%0)\n"
			"movntq %%mm0, 56(%0)\n"
		: : "r" (page) : "memory");
		page+=64;
	}

	// Since movntq is weakly-ordered, a "sfence" is needed.
	__asm__ __volatile__ ("sfence" : : : "memory");
#else
	// Fast clear the page.
	for(i=0; i<PAGE_SIZE/128; i++) {
		__asm__ __volatile__ (
			"movq %%mm0, (%0)\n"
			"movq %%mm0, 8(%0)\n"
			"movq %%mm0, 16(%0)\n"
			"movq %%mm0, 24(%0)\n"
			"movq %%mm0, 32(%0)\n"
			"movq %%mm0, 40(%0)\n"
			"movq %%mm0, 48(%0)\n"
			"movq %%mm0, 56(%0)\n"
			"movq %%mm0, 64(%0)\n"
			"movq %%mm0, 72(%0)\n"
			"movq %%mm0, 80(%0)\n"
			"movq %%mm0, 88(%0)\n"
			"movq %%mm0, 96(%0)\n"
			"movq %%mm0, 104(%0)\n"
			"movq %%mm0, 112(%0)\n"
			"movq %%mm0, 120(%0)\n"
		: : "r"(page) : "memory");
		page += 128;
	}
#endif
	// Restore task switch flag.
	slts();
}

//! \brief Copy a page using MMX extensions.
static __INLINE__ void fast_copy_page(void *to, void *from)
{
	int i;

	// Clear the task switch flag.
	clts();

#if (defined(CONFIG_MK7) || defined(CONFIG_MATHLONXP))
	__asm__ __volatile__ (
		"prefetchnta (%0)\n"
		"prefetchnta 64(%0)\n"
		"prefetchnta 128(%0)\n"
		"prefetchnta 192(%0)\n"
		: : "r" (from) );

	for(i=0; i<PAGE_SIZE/64; i++) {
		__asm__ __volatile__ (
			"prefetchnta 200(%0)\n"
			"movq (%0), %%mm0\n"
			"movq 8(%0), %%mm1\n"
			"movq 16(%0), %%mm2\n"
			"movq 24(%0), %%mm3\n"
			"movq 32(%0), %%mm4\n"
			"movq 40(%0), %%mm5\n"
			"movq 48(%0), %%mm6\n"
			"movq 56(%0), %%mm7\n"
			"movntq %%mm0, (%1)\n"
			"movntq %%mm1, 8(%1)\n"
			"movntq %%mm2, 16(%1)\n"
			"movntq %%mm3, 24(%1)\n"
			"movntq %%mm1, 32(%1)\n"
			"movntq %%mm2, 40(%1)\n"
			"movntq %%mm3, 48(%1)\n"
			"movntq %%mm3, 56(%1)\n"
			: : "r" (from), "r" (to) : "memory");
		from += 64;
		to += 64;
	}
	// Since movntq is weakly-ordered, a "sfence" is needed.
	__asm__ __volatile__ ("sfence" : : : "memory");
#else
	__asm__ __volatile__ (
		"prefetch (%0)\n"
		"prefetch 64(%0)\n"
		"prefetch 128(%0)\n"
		"prefetch 192(%0)\n"
		"prefetch 256(%0)\n"
		: : "r" (from) );

	for(i=0; i<PAGE_SIZE/64; i++) {
		__asm__ __volatile__ (
			"prefetch 320(%0)\n"
			"movq (%0), %%mm0\n"
			"movq 8(%0), %%mm1\n"
			"movq 16(%0), %%mm2\n"
			"movq 24(%0), %%mm3\n"
			"movq %%mm0, (%1)\n"
			"movq %%mm1, 8(%1)\n"
			"movq %%mm2, 16(%1)\n"
			"movq %%mm3, 24(%1)\n"
			"movq 32(%0), %%mm0\n"
			"movq 40(%0), %%mm1\n"
			"movq 48(%0), %%mm2\n"
			"movq 56(%0), %%mm3\n"
			"movq %%mm0, 32(%1)\n"
			"movq %%mm1, 40(%1)\n"
			"movq %%mm2, 48(%1)\n"
			"movq %%mm3, 56(%1)\n"
			: : "r" (from), "r" (to) : "memory");
		from += 64;
		to += 64;
	}
#endif
	// Restore task switch flag.
	slts();
}

#else

//! \brief Clear a page in the slow mode.
static __INLINE__ void fast_clear_page(void *page)
{
	memset(page, 0, PAGE_SIZE);
}

//! \brief Copy a page in the slow mode.
static __INLINE__ void fast_copy_page(void *to, void *from)
{
	memcpy(to, from, PAGE_SIZE);
}

#endif

#endif // MMX_H
