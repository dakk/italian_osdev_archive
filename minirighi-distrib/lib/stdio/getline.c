/*!	\file lib/stdio/getline.c
 *	\brief Get a line of input from the console.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-29\n
 */

#include <stdio.h>
#include <ctype.h>
#include <kernel/keyboard.h>

char *getline( char *str, int size )
{
	int i = 0;
	int c;

	while( i < size )
	{
		c = getchar();
		if( c == EOF )
		{
			break;
		}
		else if( c == CTRL_C )
		{
			// Return an empty string.
			i = 0;
			break;
		}

		c &= 0xff;
		if( (c == '\r') || (c == '\n') )
		{
			break;
		}
		switch( c )
		{
			// Backspace.
			case '\b':
				if ( i > 0 )
				{
					putchar( c );
					i--;
				}
			break;

			// Bell.
			case '\a':
				putchar( '\a' );
			break;

			// ^U -- kill line.
			case '\025':
				while( i )
				{
					putchar( '\b' );
					i--;
				}
				str[ 0 ] = '\0';
			break;

			default:
			{
				if( i < (size - 1) )
				{
					if ( isprint( c ) )
					{
						str[ i ] = c;
						putchar( c );
						i++;
					}
				}
			}
		}
	}
	str[ i ] = '\0';
	putchar( '\n' );
	return( str );
}
