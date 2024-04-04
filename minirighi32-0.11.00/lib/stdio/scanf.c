/*!	\file lib/stdio/scanf.c
 *	\brief LIBC stdio scanf function.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-10-28
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <stdio.h>
#include <stdarg.h>

#define BUF_SIZE	STR_MAX_LENGTH

int scanf(const char *fmt, ...)
{
	char buf[ BUF_SIZE ];
	va_list args;
	int i=0;

	gets( buf );

	va_start( args, fmt );
	i = vsscanf( buf, fmt, args );
	va_end( args );

	return( i );
}
