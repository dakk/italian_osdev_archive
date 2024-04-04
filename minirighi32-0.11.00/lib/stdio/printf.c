/*!	\file lib/stdio/printf.c
 *	\brief LIBC stdio printf function.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-10-27
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <stdio.h>

int printf(const char *fmt, ...)
{
	char buf[1024];
	va_list args;
	int i;

	va_start( args, fmt );

	vsnprintf( buf, sizeof(buf), fmt, args );
	va_end( args );

	for( i=0; i<1024; i++ )
	{
		if( !buf[i] ) break;
		putchar( buf[i] );
	}

	return( i );
}
