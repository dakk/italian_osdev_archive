/*!	\file lib/string/string.c
 *	\brief LIBC string routines.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-10-27
 *	\note Copyright (&copy;) 2003 Andrea Righi
 *
 *	Thanks to Linux, The Mobius and libc...
 */

#include <const.h>
#include <ctype.h>

#include <string.h>

//! \brief Compare two strings ignoring cases.
int strcasecmp(const char *s1, const char *s2)
{
	register const unsigned char
			*us1 = (const unsigned char *)s1,
			*us2 = (const unsigned char *)s2;

	while( tolower(*us1) == tolower(*us2++) )
		if ( *us1++ == '\0' )
			return( 0 );

	return( tolower(*us1) - tolower(*--us2) );
}

//! \brief Compare two strings ignoring cases for a maximum
//! length of n.
int strncasecmp(const char *s1, const char *s2, size_t n)
{
	if( n != 0 )
	{
		register const unsigned char
				*us1 = (const unsigned char *)s1,
				*us2 = (const unsigned char *)s2;
		do
		{
			if( tolower(*us1) != tolower(*us2++) )
				return( tolower(*us1) - tolower(*--us2) );
			if ( *us1++ == '\0' )
				break;
		} while( --n != 0 );
	}
	return( 0 );
}

//! \brief Find the first substring in a '\0'-terminated string.
//! \param s The string to be searched.
//! \param find The string to search for.
//! \return
//! 	\li A pointer of the \p find string into the \p s string.
//!	\li 0 if the \p find string has not been found into \p s.
char *strstr( const char *s, const char *find )
{
	char c, sc;
	size_t len;

	if( (c = *find++) != 0 )
	{
		len = strlen( find );
		do
		{
			do
			{
				if( (sc = *s++) == 0 )
					return( 0 );
			} while( sc != c );
		} while( strncmp(s, find, len) != 0 );
		s--;
	}
	return( (char*)s );
}

/*!
 * \brief Find the first occurrence of a set of characters.
 * \param cs The string to be searched.
 * \param ct The characters to search for.
 * \return
 *	The beginning of the first occurrence string if it has
 *	found it, #NULL otherwise.
 */
char *strpbrk( const char *cs, const char *ct )
{
	const char *sc1, *sc2;

	for( sc1 = cs; *sc1 != '\0'; ++sc1 )
	{
		for( sc2 = ct; *sc2 != '\0'; ++sc2 )
		{
			if( *sc1 == *sc2 )
				return( (char *)sc1 );
		}
	}
	return( NULL );
}

/*!
 * \brief Split a string into tokens.
 * \param s The string to be searched.
 * \param ct The characters to search for.
 * \return The beginning of the first string.
 *
 * strsep() updates \p s to point after the token,
 * ready for the next call.
 */
char *strsep( char **s, const char *ct )
{
	char *sbegin = *s, *end;

	if( sbegin == NULL )
		return( NULL );

	end = strpbrk( sbegin, ct );
	if( end )
		*end++ = '\0';
	*s = end;

	return( sbegin );
}
