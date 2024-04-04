/*!	\file apps/src/beep/beep.c
 *	\brief System beep.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-20\n
 */

#include <stdio.h>
#include <stdlib.h>

int main( int argc, char **argv )
{
	putchar( '\a' );
	return( EXIT_SUCCESS );
}
