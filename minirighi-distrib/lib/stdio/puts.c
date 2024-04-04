/*!	\file lib/stdio/puts.c
 *	\brief LIBC stdio puts function.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-25\n
 */

#include <stdio.h>

int puts( const char *s )
{
	while( *s )
	{
		putchar( *s );
		s++;
	}
	return( putchar('\n') );
}
