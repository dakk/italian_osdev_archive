/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: stdlib.cpp			|
	| Date: 01/12/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

#ifndef _LIBC_STDLIB_H_
#define _LIBC_STDLIB_H_

extern "C"
{ 
	void *malloc(size_t dim);
	void free(void * addr);
}

#endif
