/*!	\file apps/src/date/date.c
 *	\brief Print the current date.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-26\n
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Print the current date.
int main( int argc, char **argv )
{
	char buf[ 26 ];
	time_t t;

	t = time( NULL );
	printf( "%s", ctime_r(&t, buf) );

	return( EXIT_SUCCESS );
}
