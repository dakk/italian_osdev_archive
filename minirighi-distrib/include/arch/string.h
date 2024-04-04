/*!	\file include/arch/string.h
 *	\brief Low level implementation of string operators.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update:\n
 *		2004-03-26 Andrea Righi:
 			Improved assembler routies.\n
 *	\note Copyright (C) 2003 Andrea Righi
 */

#ifndef ARCH_STRING_H
#define ARCH_STRING_H

#include <const.h>
#include <types.h>

#ifndef HAVE_ARCH_MEMSETW
#define HAVE_ARCH_MEMSETW
//! \brief Sets count words to val.
static __INLINE__ void *memsetw( uint16_t *s, uint16_t c, size_t count )
{
	int d0, d1;
	__asm__ __volatile__ (
		"cld\n"
		"rep stosw\n"
		: "=&c"(d0), "=&D"(d1)
		: "a"(c), "1"(s), "0"(count)
		: "memory" );
	return( s );
}
#endif

#ifndef HAVE_ARCH_MEMSETL
#define HAVE_ARCH_MEMSETL
//! \brief Sets count double words to val.
static __INLINE__ void *memsetl( uint32_t *s, uint32_t c, size_t count)
{
	int d0, d1;
	__asm__ __volatile__ (
		"cld\n"
		"rep stosl\n"
		: "=&c"(d0), "=&D"(d1)
		: "a"(c), "1"(s), "0"(count)
		: "memory" );
	return( s );
}
#endif

#ifndef HAVE_ARCH_MEMSET
#define HAVE_ARCH_MEMSET
//! This is the memset implementation when count is a constant
//! and it is less or equal than 16.
#define __memset_constant_count_short( s, c, count )					\
( (count==0) ? (s) : 									\
	({										\
	void *__s = (void *)(s);							\
	union {										\
		unsigned int ui;							\
		unsigned short int us;							\
		unsigned char uc;							\
	} *u = __s;									\
	unsigned int pattern = ((unsigned int) ((unsigned char) (c))) * 0x01010101;	\
	switch( count )									\
	{										\
		case 15:								\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 11:								\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 7:									\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 3:									\
			u->us = (unsigned short)pattern;				\
			u = __extension__ (void *)((char *)u + 2);			\
			u->uc = (unsigned char)pattern;					\
			break;								\
		case 14:								\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 10:								\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 6:									\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 2:									\
			u->us = (unsigned short)pattern;				\
			break;								\
		case 13:								\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 9:									\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 5:									\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 1:									\
			u->uc = (unsigned char)pattern;					\
			break;								\
		case 16:								\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 12:								\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 8:									\
			u->ui = pattern;						\
			u = __extension__ (void *)((char *)u + 4);			\
		case 4:									\
			u->ui = pattern;						\
			break;								\
	}										\
	__s;										\
	})										\
)

//! This is the memset implementation when count is a constant
//! and it is greater than 16.
static __INLINE__ void *__memset_constant_count_long( void *s, int c, size_t count )
{
	#define MEMSET_TAIL( x )		\
		__asm__  __volatile__ (		\
			"rep ; stosl\n"		\
			x			\
			: "=&c"(d0), "=&D"(d1)	\
			: "a"(pattern), "0"(count/4), "1"((long)s) \
			: "memory" )
	int d0, d1;
	unsigned int pattern = ((unsigned int) ((unsigned char) (c))) * 0x01010101;
	// This switch statement will be removed at compile-time
	// because count is a constant.
	switch( count % 4 )
	{
		case 0:
			MEMSET_TAIL( "" );
			return( s );
		case 1:
			MEMSET_TAIL( "stosb" );
			return( s );
		case 2:
			MEMSET_TAIL( "stosw" );
			return( s );
		default:
			MEMSET_TAIL( "stosw\nstosb" );
			return( s );
	}
	#undef MEMSET_TAIL
}

//! This is the memset implementation when c is a constant.
static __INLINE__ void *__memset_constant_c( void *s, unsigned long pattern, size_t count )
{
	int d0, d1;
	__asm__ __volatile__ (
		"rep stosl\n"
		"testb $2, %b3\n"
		"je 1f\n"
		"stosw\n"
		"1:\ttestb $1, %b3\n"
		"je 2f\n"
		"stosb\n"
		"2:"
		: "=&c"(d0), "=&D"(d1)
		: "a"(pattern), "q"(count), "0"(count/4), "1"((long)s)
		: "memory");
	return( s );
}

//! This is the generic implementation of memset, when both
//! c and count are not constant.
static __INLINE__ void *__memset_generic( void *s, int c, int count )
{
	int d0, d1;
	__asm__ __volatile__ (
		"rep stosb"
		: "=&c"(d0), "=&D"(d1)
		: "a"(c), "0"(count), "1"((long)s)
		: "memory" );
	return( s );
}

//! The memset implementation when count is not a constant.
#define __memset( s, c, count ) \
	( __builtin_constant_p(c) ? \
		__memset_constant_c( (s), (0x01010101UL*(unsigned char)(c)), (count) ) : \
		__memset_generic((s), (c), (count)) \
	)

//! The memset implementation when count is constant.
#define __memset_constant_count( s, c, count ) \
	( (count <= 16) ? \
		__memset_constant_count_short((s), (c), (count)) : \
		__memset_constant_count_long((s), (c), (count)) \
	)

//! This is the main memset routine.
#define memset( s, c, count ) \
	( __extension__ (__builtin_constant_p(count) ? \
		__memset_constant_count((s), (c), (count)) : \
		__memset((s), (c), (count))) \
	)
#endif // HAVE_ARCH_MEMSET

#ifndef HAVE_ARCH_BZERO
#define HAVE_ARCH_BZERO
//! \brief Zero a memory area.
#define bzero( s, count ) \
	memset( (s), 0, (count) )
#endif

#ifndef HAVE_ARCH_MEMCPY
#define HAVE_ARCH_MEMCPY
#define __memcpy_constant_short( to, from, n )						\
( (n==0) ? (to) :									\
	({										\
	void *__to = (void *)(to), *__from = (void *)(from);				\
	union {										\
		unsigned int ui;							\
		unsigned short int us;							\
		unsigned char uc;							\
	} *ut = __to, *uf = __from;							\
	switch( n )									\
	{										\
		case 15:								\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 11:								\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 7:									\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 3:									\
			ut->us = uf->us;						\
			ut = __extension__ (void *)((char *)ut + 2);			\
			uf = __extension__ (void *)((char *)uf + 2);			\
			ut->uc = uf->uc;						\
			break;								\
		case 14:								\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 10:								\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 6:									\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 2:									\
			ut->us = uf->us;						\
			break;								\
		case 13:								\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 9:									\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 5:									\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 1:									\
			ut->uc = uf->uc;						\
			break;								\
		case 16:								\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 12:								\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 8:									\
			ut->ui = uf->ui;						\
			ut = __extension__ (void *)((char *)ut + 4);			\
			uf = __extension__ (void *)((char *)uf + 4);			\
		case 4:									\
			ut->ui = uf->ui;						\
			break;								\
	}										\
	__to;										\
	})										\
)

static __INLINE__ void *__memcpy_constant_long( void *to, const void *from, size_t n)
{
	#define MEMCPY_TAIL( x )	\
		__asm__ __volatile__ (	\
		"rep ; movsl\n"		\
		x			\
		: "=&c"(d0), "=&D"(d1), "=&S"(d2) \
		: "0"(n/4), "1"((long)to), "2"((long)from) \
		: "memory" );
	int d0, d1, d2;
	// This switch statement will be removed at compile-time
	// because n is a constant.
	switch( n % 4 )
	{
		case 0:
			MEMCPY_TAIL( "" );
			return( to );
		case 1:
			MEMCPY_TAIL( "movsb" );
			return( to );
		case 2:
			MEMCPY_TAIL( "movsw" );
			return( to );
		default:
			MEMCPY_TAIL( "movsw\nmovsb" );
			return( to );
	}
	#undef MEMCPY_TAIL
}

//! Generic implementation of memcpy (when n is not constant).
static __INLINE__ void *__memcpy_generic( void *to, const void *from, size_t n )
{
	int d0, d1, d2;
	__asm__ __volatile__ (
		"cld\n"
		"rep movsl\n"
		"testb $2, %b4\n"
		"je 1f\n"
		"movsw\n"
		"1:\ttestb $1, %b4\n"
		"je 2f\n"
		"movsb\n"
		"2:"
		: "=&c"(d0), "=&D"(d1), "=&S"(d2)
		: "0"(n/4), "q"(n), "1"((long)to), "2"((long)from)
		: "memory" );
	return( to );
}

//! The memcpy implementation when n is a constant.
#define __memcpy_constant( to, from, n ) \
	( __extension__ (n <= 16) ? \
		__memcpy_constant_short((to), (from), (n)) : \
		__memcpy_constant_long((to), (from), (n)) \
	)

//! \brief Optimized version of memcpy.
#define memcpy( to, from, n ) \
	( __extension__ (__builtin_constant_p (n) ? \
		__memcpy_constant((to), (from), (n)) : \
		__memcpy_generic((to), (from), (n))) \
	)
#endif

#ifndef HAVE_ARCH_MEMCMP
#define HAVE_ARCH_MEMCMP
//! \brief
//!	Compares the first \p n bytes of memory areas \p s1 and \p s2.
//!	It returns an integer less than, equal to, or greater than zero
//!	if \p s1 is found, respectively, to be less than, to match, or
//!	be greater than \p s2.
static __INLINE__ int memcmp( const void *s1, const void *s2, size_t n )
{
	int	d0, d1, d2;
	register int __res;
	__asm__ __volatile__ (
		"cld\n"
		"repe cmpsb\n"
		"je 1f\n"
		"sbbl %0, %0\n"
		"orb $1, %b0\n"
		"1:"
		: "=a"(__res), "=&S"(d0), "=&D"(d1), "=&c"(d2)
		: "0"(0), "1"(s1), "2"(s2), "3"(n) );
	return( __res );
}
#endif

#ifndef HAVE_ARCH_MEMCHR
#define HAVE_ARCH_MEMCHR
//! \brief Find a byte into a memory area.
//! \param cs The pointer of the memory area.
//! \param c The character to find.
//! \param count The size of the memory area.
//! \return
//!	The address of the first occurrence or 0 if the char has not
//!	been found.
static __INLINE__ void *memchr( const void *cs, int c, size_t count )
{
	int d0;
	register void * __res;

	if( !count )
		return( 0 );

	__asm__ __volatile__ (
		"cld\n"
		"repne scasb\n"
		"je 1f\n"
		"movl $1,%0\n"
		"1:\tdecl %0"
		: "=D"(__res), "=&c"(d0)
		: "a"(c), "0"(cs), "1"(count) );
	return( __res );
}
#endif

#ifndef HAVE_ARCH_STRCPY
#define HAVE_ARCH_STRCPY
static __INLINE__ char *strcpy( char *dest, const char *src )
{
	int d0, d1, d2;
	__asm__ __volatile__ (
		"1:\tlodsb\n"
		"stosb\n"
		"testb %%al, %%al\n"
		"jne 1b"
		: "=&S"(d0), "=&D"(d1), "=&a"(d2)
		: "0"(src), "1"(dest)
		: "memory" );
	return( dest );
}
#endif

#ifndef HAVE_ARCH_STRNCPY
#define HAVE_ARCH_STRNCPY
static __INLINE__ char *strncpy( char *dest, const char *src, size_t count )
{
	int d0, d1, d2, d3;
	__asm__ __volatile__ (
		"1:\tdecl %2\n"
		"js 2f\n"
		"lodsb\n"
		"stosb\n"
		"testb %%al, %%al\n"
		"jne 1b\n"
		"rep\n"
		"stosb\n"
		"2:"
		: "=&S"(d0), "=&D"(d1), "=&c"(d2), "=&a"(d3)
		: "0"(src), "1"(dest), "2"(count)
		: "memory" );
	return( dest );
}
#endif

#ifndef HAVE_ARCH_STRCAT
#define HAVE_ARCH_STRCAT
static __INLINE__ char *strcat( char *dest, const char *src )
{
	int d0, d1, d2, d3;
	__asm__ __volatile__(
		"repne\n"
		"scasb\n"
		"decl %1\n"
		"1:\tlodsb\n"
		"stosb\n"
		"testb %%al, %%al\n"
		"jne 1b"
		: "=&S"(d0), "=&D"(d1), "=&a"(d2), "=&c"(d3)
		: "0"(src), "1"(dest), "2"(0), "3"(0xffffffff)
		: "memory" );
	return( dest );
}
#endif

#ifndef HAVE_ARCH_STRNCAT
#define HAVE_ARCH_STRNCAT
static __INLINE__ char *strncat( char *dest, const char *src, size_t count )
{
	int d0, d1, d2, d3;
	__asm__ __volatile__(
		"repne\n"
		"scasb\n"
		"decl %1\n"
		"movl %8, %3\n"
		"1:\tdecl %3\n"
		"js 2f\n"
		"lodsb\n"
		"stosb\n"
		"testb %%al, %%al\n"
		"jne 1b\n"
		"2:\txorl %2, %2\n"
		"stosb"
		: "=&S"(d0), "=&D"(d1), "=&a"(d2), "=&c"(d3)
		: "0"(src), "1"(dest), "2"(0), "3"(0xffffffff), "g"(count)
		: "memory" );
	return( dest );
}
#endif

#ifndef HAVE_ARCH_STRCMP
#define HAVE_ARCH_STRCMP
static __INLINE__ int strcmp( const char *cs, const char *ct )
{
	int d0, d1;
	register int __res;
	__asm__ __volatile__(
		"1:\tlodsb\n"
		"scasb\n"
		"jne 2f\n"
		"testb %%al, %%al\n"
		"jne 1b\n"
		"xorl %%eax, %%eax\n"
		"jmp 3f\n"
		"2:\tsbbl %%eax, %%eax\n"
		"orb $1, %%al\n"
		"3:"
		: "=a"(__res), "=&S"(d0), "=&D"(d1)
		: "1"(cs), "2"(ct) );
	return( __res );
}
#endif

#ifndef HAVE_ARCH_STRNCMP
#define HAVE_ARCH_STRNCMP
static __INLINE__ int strncmp( const char *cs, const char *ct, size_t count )
{
	register int __res;
	int d0, d1, d2;
	__asm__ __volatile__(
		"1:\tdecl %3\n"
		"js 2f\n"
		"lodsb\n"
		"scasb\n"
		"jne 3f\n"
		"testb %%al, %%al\n"
		"jne 1b\n"
		"2:\txorl %%eax, %%eax\n"
		"jmp 4f\n"
		"3:\tsbbl %%eax, %%eax\n"
		"orb $1, %%al\n"
		"4:"
		: "=a"(__res), "=&S"(d0), "=&D"(d1), "=&c"(d2)
		: "1"(cs), "2"(ct), "3"(count) );
	return( __res );
}
#endif

#ifndef HAVE_ARCH_STRLEN
#define HAVE_ARCH_STRLEN
static __INLINE__ size_t strlen( const char *s )
{
	int d0;
	register int __res;
	__asm__ __volatile__(
		"repne\n"
		"scasb\n"
		"notl %0\n"
		"decl %0"
		: "=c"(__res), "=&D"(d0)
		: "1"(s), "a"(0), "0"(0xffffffff) );
	return( __res );
}
#endif

#ifndef HAVE_ARCH_STRNLEN
#define HAVE_ARCH_STRNLEN
static __INLINE__ size_t strnlen( const char *s, size_t count )
{
	int d0;
	register int __res;
	__asm__ __volatile__(
		"movl %2, %0\n"
		"jmp 2f\n"
		"1:\tcmpb $0, (%0)\n"
		"je 3f\n"
		"incl %0\n"
		"2:\tdecl %1\n"
		"cmpl $-1, %1\n"
		"jne 1b\n"
		"3:\tsubl %2, %0"
		: "=a"(__res), "=&d"(d0)
		: "c"(s), "1"(count) );
	return( __res );
}
#endif

#endif
