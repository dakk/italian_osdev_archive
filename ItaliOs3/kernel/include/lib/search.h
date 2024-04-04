#ifndef SEARCH_H
#define SEARCH_H
#include <stdlib.h>

/* binarysearch() - A binary search implementation which takes the same
 * arguments as the C library function bsearch() provided by stdlib.h.  It
 * searches the sorted array at address base, which has nmemb elements, each of
 * which is size bytes long.  compar is a pointer to a comparison function.
 * The function compares two array elements, and returns a negative, zero, or
 * positive integer depending on whether the first array element is less than,
 * equal to, or greater than the second.
 */
void *binarysearch(const void *key, const void *base, size_t nmemb,
		   size_t size, int (*compar)(const void *, const void *));

#endif
