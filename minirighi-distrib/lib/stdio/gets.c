/*!	\file lib/stdio/gets.c
 *	\brief LIBC stdio gets function.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-10-28
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <stdio.h>
#include <ctype.h>

#define GETS_MAX	STR_MAX_LENGTH

char *gets( char *s )
{
	int c;
	int i = 0;

	for( ; ; )
	{
		c = getchar();
		if( c == EOF )
		{
			break;
		}

		c &= 0xff;
		if( (c == '\r') || (c == '\n') )
		{
			break;
		}
		switch( c )
		{
			case '\b':
				if( i > 0 )
				{
					putchar( c );
					i--;
				}
			break;

			default:
			{
				if( i < (GETS_MAX - 1) )
				{
					if ( isprint( c ) )
					{
						s[ i ] = c;
						putchar( c );
						i++;
					}
				}
			}
		}
	}
	putchar( '\n' );
	s[ i ] = 0;
	return( s );
}
