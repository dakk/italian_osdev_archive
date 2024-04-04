/*!	\file include/stdlib.h
 *	\brief LIBC stdlib.
 *	\author Andrea Righi <righiandr@users.sf.net>
 *	\date Last update: 2003-11-05
 *	\note Copyright (C) 2003 Andrea Righi
 */

#ifndef STDLIB_H
#define STDLIB_H

#include <types.h>
#include <sys/sys.h>

/** \ingroup LibC
 *  \defgroup LibCStdlib stdlib
 *  Standard library operators.
 *  @{
 */

//! Count the elements from an array.
#define _countof(a)	( sizeof(a) / sizeof((a)[0]) )

//! The largest number to be returned by rand().
#define RAND_MAX	2147483647

//! Failure exist status.
#define EXIT_FAILURE	1

//! Successful exit status.
#define EXIT_SUCCESS	0

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

extern int getopt(int __argc, char * const *__argv, const char *__shortopts);

extern int atoi(const char *__str);

extern void srand(unsigned int seed);
extern int rand(void);

extern void *malloc(size_t __size);
extern void *calloc( size_t num, size_t size );
extern void free(void *__ptr);

/** @} */ // end of LibCStdlib

#endif
