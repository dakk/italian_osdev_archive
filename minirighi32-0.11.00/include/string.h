/*!	\file include/string.h
 *	\brief Header for string routines.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-10-27
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#ifndef STRING_H
#define STRING_H

#include <const.h>
#include <types.h>
#include <arch/string.h>

/** \ingroup LibC
 *  \defgroup LibCString string
 *  Strings operators.
 *  @{
 */

#ifndef HAVE_ARCH_MEMSETL
static __INLINE__ void *memsetl( void *s, unsigned long c, size_t count )
{
	unsigned long *tmp = (unsigned long *)s;

	while( count-- )
		*tmp++ = c;

	return( s );
}
#endif

#ifndef HAVE_ARCH_MEMSETW
static __INLINE__ void *memsetw( void *s, unsigned short c, size_t count )
{
	unsigned short *tmp = (unsigned short *)s;

	while( count-- )
		*tmp++ = c;

	return( s );
}
#endif

#ifndef HAVE_ARCH_MEMSET
static __INLINE__ void *memset( void *s, int c, size_t count )
{
	unsigned char *tmp = (unsigned char *)s;

	while( count-- )
		*tmp++ = c;

	return( s );
}
#endif

#ifndef HAVE_ARCH_BZERO
static __INLINE__ void bzero( void *s, size_t count )
{
	unsigned char *tmp = (unsigned char *)s;

	while( count-- )
		*tmp++ = 0;
}
#endif

#ifndef HAVE_ARCH_MEMCPY
static __INLINE__ void *memcpy( void *to, const void *from, size_t n )
{
	unsigned char *tmp = (unsigned char *)to, *s = (unsigned char *)from;

	while( n-- )
		*tmp++ = *s++;

	return( to );
}
#endif

#ifndef HAVE_ARCH_MEMCMP
static __INLINE__ int memcmp( const void *cs, const void *ct, size_t count )
{
	const unsigned char *su1, *su2;
	int res = 0;

	for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count-- )
		if( (res = *su1 - *su2) )
			break;
	return( res );
}
#endif

#ifndef HAVE_ARCH_MEMCHR
static __INLINE__ void *memchr( const void *s, int c, size_t n )
{
	const unsigned char *p = s;
	while( n-- )
	{
        	if( (unsigned char)c == *p++ )
		{
			return( (void *)(p-1) );
		}
	}
	return( NULL );
}
#endif

#ifndef HAVE_ARCH_STRCHR
//! \brief Find the first occurrence of a character in a string.
static __INLINE__ char *strchr( const char *s, int c )
{
	for( ; *s != (char)c; ++s )
		if( *s == '\0' )
			return( NULL );
	return( (char *)s );
}
#endif

#ifndef HAVE_ARCH_STRRCHR
//! \brief Find the last occurrence of a character in a string.
static __INLINE__ char *strrchr( const char *s, int c )
{
	const char *p = s + strlen(s);
	do
	{
		if( *p == (char)c )
			return( (char *)p );
	} while( --p >= s );
	return( NULL );
}
#endif

#ifndef HAVE_ARCH_STRCPY
//! \brief Copy a string to a destination.
//! \note
//!	After the copy dest pointer is terminated with NULL.
static __INLINE__ char *strcpy(char *dest, const char *org)
{
	char *temp = dest;

	// Continue until '\0' is copied //
	for (; (*dest = *org); ++dest, ++org);

	return temp;
}
#endif

#ifndef HAVE_ARCH_STRNCPY
//! \brief
//! 	Copy no more than N characters from a source string to a
//!	destination.
static __INLINE__ char *strncpy(char *dest, const char *org, size_t n)
{
	char *d = dest;
	const char *s = org;

	if (n != 0)
	{
		do
		{
			if ((*d++ = *s++) == 0)
			{
				while (--n)
					*d++ = 0;
				break;
			}
		} while (--n != 0);
	}
	return( dest );
}
#endif

#ifndef HAVE_ARCH_STRLEN
//! \brief Get the length in characters of a string.
//! \param str The string pointer.
//! \return The length of the \p str string.
static __INLINE__ size_t strlen(const char *str)
{
	const char *s;

	if (str == 0) return(0);

	for (s = str; *s; ++s);

	return(s-str);
}
#endif

#ifndef HAVE_ARCH_STRNLEN
//! \brief
//!	Get the length of a string for a maximum of count characters.
static __INLINE__ size_t strnlen(const char *s, unsigned int count)
{
	const char *sc;
	if( !s ) return( 0 );
	for ( sc = s; ( count-- ) && ( *sc != '\0' ); ++sc );
	return( sc - s );
}
#endif

#ifndef HAVE_ARCH_STRCAT
//! \brief Append a string to another.
//! \note After the append s is closed by a NULL.
static __INLINE__ char *strcat(char *s, const char *append)
{
	char *save = s;

	for (; *s; ++s);
	while ((*s++ = *append++));
	return save;
}
#endif

#ifndef HAVE_ARCH_STRNCAT
/*!
 * \brief Append a length-limited, NULL-terminated string to another.
 * \note
 * 	In contrast to strncpy, strncat ensures the result is
 * 	terminated.
 */
static __INLINE__ char *strncat(char *dest, const char *src, size_t count)
{
	char *tmp = dest;

	if (count) {
		while (*dest)
			dest++;
		while ((*dest++ = *src++)) {
			if (--count == 0) {
				*dest = '\0';
				break;
			}
		}
	}

	return tmp;
}
#endif

#ifndef HAVE_ARCH_STRCMP
//! \brief Compare two strings.
static __INLINE__ int strcmp(const char *s1, const char *s2)
{
	while ((*s2 != '\0') && (*s2 == *s1))
	{
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}
#endif

#ifndef HAVE_ARCH_STRNCMP
//! \brief Compare N characters of \p s1 and \p s2.
//! \param s1 The first string to compare.
//! \param s2 The second string to compare.
//! \param n The characters to compare.
//! \return A value that depends from the compare:
//!	\li < 0 if \p s1 is less than \p s2;
//!	\li = 0 if \p s1 is equal to \p s2;
//!	\li > 0 if \p s1 is greater than \p s2.
static __INLINE__ int strncmp(const char *s1, const char *s2, size_t n)
{
	if (n == 0)
		return 0;
	do
	{
		if (*s1 != *s2++)
			return *(unsigned const char *)s1 - *(unsigned const char *)--s2;
		if (*s1++ == 0)
			break;
	} while (--n != 0);

	return 0;
}
#endif

// --- Prototypes ----------------------------------------------------- //

extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, size_t n);
extern char *strstr(const char *s, const char *find);
extern char *strpbrk(const char *cs, const char *ct);
extern char *strsep(char **s, const char *ct);

/** @} */ // end of LibCString

#endif
