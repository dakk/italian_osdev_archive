/*!	\file lib/stdlib/rand.c
 *	\brief LIBC random routines.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-04-11\n
 *	\note Copyright (&copy;) 2004 Andrea Righi
 */

#include <stdlib.h>

//! The global random seed.
unsigned int _seed = 1;

//! Sets its argument as the seed for a new sequence of pseudo-random
//! numbers to be returned by rand().
void srand( unsigned int seed )
{
	_seed = seed;
}

//! Returns a pseudo-random number between 0 and RAND_MAX.
int rand()
{
	_seed = (28629 * _seed + 157) % 0x7fffffff;
	return( _seed );
}
