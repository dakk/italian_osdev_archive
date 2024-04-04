/*!	\file lib/stdlib/calloc.c
 *	\brief LIBC calloc routine
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2004-02-28
 *	\note Copyright (&copy;) 2003 Andrea Righi
 */

#include <stdlib.h>
#include <string.h>
#include <sys/sys.h>

//! Allocate num elements of size bytes each, all initialized to 0.
void *calloc( size_t num, size_t size )
{
	void *__ret;

	__ret = malloc( num*size );
	memset( __ret, 0, num*size );

	return( __ret );
}
