#ifndef SORT_H
#define sort_H
/*** File:  sort.h  - Provides Functions for Sorting Arrays ***/
/*
 *   Shane Saunders
 */
#include <stdlib.h>

/* quicksort() - A quicksort implementation which takes the same arguments as
 * the C library function qsort() provided by stdlib.h.  It sorts the array
 * pointed to by base.  The array has nmemb elements, of which each is `size'
 * bytes long.  compare is a pointer to a comparison function.  The function
 * compares two array elements, and returns a negative, zero, or positive
 * integer depending on whether the first array element is less than, equal
 * to, or greater than the second.
 */
void quicksort(void *base, size_t nmemb, size_t size,
	       int (* compar)(const void *, const void *));

/* Mergesort implementations are also provided, which takes the same arguments
 * as the C library function qsort() provided by stdlib.h.  It sorts the array
 * pointed to by base.  The array has nmemb elements, of which each is `size'
 * bytes long.  compare is a pointer to a comparison function.  The function
 * compares two array elements, and returns a negative, zero, or positive
 * integer depending on whether the first array element is less than, equal
 * to, or greater than the second.
 *
 * mergesort0() - This recursive implementation does not alternate between the
 *                two arrays.
 * mergesort() - This recursive implementation alternates between two arrays as
 *               it writes the merge result at each level.  This prevents
 *               unnecessary array copying.
 */
void mergesort0(void *base, size_t nmemb, size_t size,
	       int (* compar)(const void *, const void *));
void mergesort(void *base, size_t nmemb, size_t size,
	       int (* compar)(const void *, const void *));

/* radixsort() - Uses a radix sort to sort entries in the array pointed to by
 * base.  The array has nmemb elements, of which each is size bytes long.
 * getvalue is a pointer to a function that returns a non-negative integer
 * representing the value of an array item.  The parameter radix specifies the
 * radix to use for the sort.
 */
void radixsort(void *base, size_t nmemb, size_t size,
               int (* getvalue)(const void *), int radix);

/* heapsort() - A heapsort implementation which takes the same arguments as
 * the C library function qsort() provided by stdlib.h.  It sorts the array
 * pointed to by base.  The array has nmemb elements, of which each is `size'
 * bytes long.  compare is a pointer to a comparison function.  The function
 * compares two array elements, and returns a negative, zero, or positive
 * integer depending on whether the first array element is less than, equal
 * to, or greater than the second.
 */
void heapsort(void *base, size_t nmemb, size_t size,
	       int (* compar)(const void *, const void *));

#endif
