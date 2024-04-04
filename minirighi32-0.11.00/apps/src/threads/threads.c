/*!	\file apps/src/threads/threads.c
 *	\brief Threads test.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-04
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/sys.h>

#ifdef NESTED_THREADS
void very_stupid_thread()
{
	printf( "I am nested thread %u, my father is %u\n", getpid(), getppid() );
}
#endif

void stupid_thread()
{
#ifdef NESTED_THREADS
	thread_spawn( very_stupid_thread, 0, 0, "very_stupid_thread" );
#endif
	printf( "I am thread %u, my father is %u\n", getpid(), getppid() );
}

int main( int argc, char **argv )
{
	int n;
	if( argc < 2 )
	{
		printf( "Usage: %s <num>\n"
			"Description: this program spawns <num> threads.\n",
			argv[0] );
		exit( 0 );
	}

	printf( "\n--- Thread stress test ---\n\n" );
	n = atoi( argv[1] );
	printf(
		"I will spawn %u self-killing thread%c.\n"
		"Press a key to begin and another key again to quit... (CTRL+C to break)\n",
		n, (n != 1) ? 's' : ' ' );

	if( getchar() == 0x2e03 )
		return( 0 );

	for( ; n--; )
	{
		thread_spawn( stupid_thread, 0, 0, "stupid_thread" );
		putchar( '.' );
	}

	(void)getchar();

	return( EXIT_SUCCESS );
}
