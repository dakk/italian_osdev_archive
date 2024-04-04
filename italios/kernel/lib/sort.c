/*** File:  sort.c  - Functions for Sorting Arrays ***/
/*
 *   Shane Saunders
 */
#include <sort.h>
#include <string.h>


/* Option for quicksort:
 * Use #define MEDIAN_PIVOT 0 to select the leftmost array element.
 * Use #define MEDIAN_PIVOT 1 to determine the median array element.
 */
#define MEDIAN_PIVOT 1


/* Each `bucket' in radix sort uses a linked list to store numbers.
 *     value - used in sorting
 *     ptr - a pointer the array element it represents.
 *     next - a pointer to the next item in the bucket.
 */
typedef struct radix_item {
	int value;
	void *ptr;
	struct radix_item *next;
} radix_item_t;


/* Functions only visible within this file. */
void quicksort_recursive(void *p, void *q);
void mergesort0_recursive(void *p, void *q);
void mergesort_recursive(void *p, void *q, void *dest);



/* Global variables accessed by recursive sorting functions.
 *   element_size - size of array elements
 *   compare_fn   - the function used for comparing array elements.
 *   x - space allocated to store the pivot during quicksort.
 *   temp_merge_array - used by mergesort0() which does not alternate the
 *                      arrays.
 */
int element_size;
int (*compare_fn) (const void *, const void *);
void *x;
void *temp_merge_array;



/* quicksort() - A quicksort implementation which takes the same arguments as
 * the C library function qsort() provided by stdlib.h.  It sorts the array
 * pointed to by base.  The array has nmemb elements, of which each is `size'
 * bytes long.  compare is a pointer to a comparison function.  The function
 * compares two array elements, and returns a negative, zero, or positive
 * integer depending on whether the first array element is less than, equal
 * to, or greater than the second.
 */
void quicksort(void *base, size_t nmemb, size_t size,
	       int (*compar) (const void *, const void *))
{

	/* The quicksort_recursive() function knows the array element size, and the
	 * comparison function through global variables.
	 */
	element_size = size;
	compare_fn = compar;

	/* Allocate space for the global variable x which holds the pivot during
	 * quicksort.
	 */
	x = malloc(size);

	/* Call the recursive quicksort implementation. */
	quicksort_recursive(base, base + size * (nmemb - 1));

	/* Free space used for storing the pivot. */
	free(x);
}



/* quicksort_recursive() - Performs quicksort recursively on the section of
 * the array specified by pointers p and q.  Accesses the global variables
 * element_size and compare_fn.
 */
void quicksort_recursive(void *p, void *q)
{
	void *i, *j;
#if MEDIAN_PIVOT
	void *m;
#endif
	/* For the quicksort implementation to be flexible, the array is accessed
	 * by void pointers.  For void pointers, addition and subtraction will
	 * change the address pointed to by a given number of bytes.  The global
	 * variable element_size is used for updating pointers to point to the
	 * next array element.  Also, when assigning to array elements, the
	 * memcpy() function is used for copying the appropriate number of bytes.
	 */

	/* Sorting is only required if p < q; that is, two ore more elements are
	 * being sorted.
	 */
	if (p < q) {

#if MEDIAN_PIVOT
		/* Select the pivot, x, as the median of a[p], a[(p+q) div 2], and
		 * a[q].
		 */
		m = p + (q - p) / 2;
		if (compare_fn(p, q) <= 0) {
			if (compare_fn(m, p) <= 0) {
				memcpy(x, p, element_size);
			} else {
				if (compare_fn(m, q) <= 0) {
					memcpy(x, m, element_size);
					memcpy(m, p, element_size);
				} else {
					memcpy(x, q, element_size);
					memcpy(q, p, element_size);
				}
			}
		} else {
			if (compare_fn(p, m) <= 0) {
				memcpy(x, p, element_size);
			} else {
				if (compare_fn(q, m) <= 0) {
					memcpy(x, m, element_size);
					memcpy(m, p, element_size);
				} else {
					memcpy(x, q, element_size);
					memcpy(q, p, element_size);
				}
			}
		}
#else
		/* Select the pivot, x, as first entry in the array region. */
		memcpy(x, p, element_size);
#endif
		/* Proceed with partitioning, which places all elements smaller than x
		 * to x's left, and all elements greater than x to x's right.
		 */

		i = p;
		j = q + element_size;

		while (i < j) {

			do {
				j -= element_size;
				if (i == j)
					goto partition_end;
			} while (compare_fn(j, x) > 0);	/* Better balance using > rather
							 * than >=.  (can be much faster)
							 */

			memcpy(i, j, element_size);

			do {
				i += element_size;
				if (i == j)
					goto partition_end;
			} while (compare_fn(i, x) < 0);	/* Better balance using < rather
							 * than <=.  (can be much faster)
							 */

			memcpy(j, i, element_size);
		}

	      partition_end:
		memcpy(i, x, element_size);

		/* Now perform quicksort on the region to the left of x, and the
		 * region to the right of x.
		 */
		quicksort_recursive(p, i - element_size);
		quicksort_recursive(i + element_size, q);

	}
}

/* mergesort0() - A mergesort implementation which takes the same arguments as
 * the C library function qsort() provided by stdlib.h.  It sorts the array
 * pointed to by base.  The array has nmemb elements, of which each is `size'
 * bytes long.  compare is a pointer to a comparison function.  The function
 * compares two array elements, and returns a negative, zero, or positive
 * integer depending on whether the first array element is less than, equal
 * to, or greater than the second.
 *
 * This implementation does not alternate between the two arrays.
 */
void mergesort0(void *base, size_t nmemb, size_t size,
		int (*compar) (const void *, const void *))
{

	/* The mergesort0_recursive() function knows the array element size, and
	 * the comparison function through global variables.  It also accesses
	 * the temporary array through the global variable temp_merge_array.
	 */
	element_size = size;
	compare_fn = compar;
	temp_merge_array = malloc(nmemb * size);

	/* Call the recursive mergesort implementation. */
	mergesort0_recursive(base, base + size * (nmemb - 1));
	free(temp_merge_array);
}



/* mergesort0_recursive() - Performs mergesort recursively on the section of
 * the array specified by pointers p and q.  Accesses the global variables
 * temp_merge_array, element_size and compare_fn.
 */
void mergesort0_recursive(void *p, void *q)
{
	void *i, *j, *k, *m;

	/* For the mergesort implementation to be flexible, the array is accessed
	 * by void pointers.  For void pointers, addition and subtraction will
	 * change the address pointed to by a given number of bytes.  The global
	 * variable element_size is used for updating pointers to point to the
	 * next array element.  Also, when assigning to array elements, the
	 * memcpy() function is used for copying the appropriate number of bytes.
	 */

	/* Sorting is only required if p < q; that is, two ore more elements are
	 * being sorted.
	 */
	if (p < q) {
		m = p + element_size * ((q - p) / (2 * element_size));

		mergesort0_recursive(p, m);
		m += element_size;
		mergesort0_recursive(m, q);

		/* The array is sorted in the regions p to m-element_size and
		 * m to q.  Now we merge these two regions into one sorted region
		 * between p and q.
		 */

		/* i points to array elements in the first region, j points to array
		 * elements in the second region, and k points to array elements in the
		 * temporary array containing the merge result.
		 */
		i = p;
		j = m;
		k = temp_merge_array;

		/* This loop performs merging while there is still at least one element
		 * left in each region.
		 */
		while (i < m && j <= q) {
			if (compare_fn(i, j) <= 0) {
				memcpy(k, i, element_size);
				i += element_size;
			} else {
				memcpy(k, j, element_size);
				j += element_size;
			}
			k += element_size;
		}

		/* Add any remaining elements in the first region to the end of the
		 * merge result.
		 */
		memcpy(k, i, m - i);


		/* Add any remaining elements in the second region to the end of the
		 * merge result.
		 */
		memcpy(k, j, (q - j) + element_size);

		/* Finally, overwrite the original array region with the merge result.
		 */
		memcpy(p, temp_merge_array, (q - p) + element_size);
	}
}



/* mergesort() - A mergesort implementation which takes the same arguments as
 * the C library function qsort() provided by stdlib.h.  It sorts the array
 * pointed to by base.  The array has nmemb elements, of which each is `size'
 * bytes long.  compare is a pointer to a comparison function.  The function
 * compares two array elements, and returns a negative, zero, or positive
 * integer depending on whether the first array element is less than, equal
 * to, or greater than the second.
 *
 * This mergesort implementation alternates between two arrays as it writes the
 * merge result at each level.  This prevents unnecessary array copying.
 */
void mergesort(void *base, size_t nmemb, size_t size,
	       int (*compar) (const void *, const void *))
{
	void *swap_array;

	/* The mergesort_recursive() function knows the array element size, and the
	 * comparison function through global variables.
	 */
	element_size = size;
	compare_fn = compar;

	/* A swap array is used to prevent an extra array copy at each merge level.
	 * The destination for the merge result alternates between the swap array
	 * and the merge array.  Initially, we set the contents of swap_array
	 * to be the same as the array pointed to by base, as swap_array could be
	 * the first array accessed at the lowest recursion level.
	 */
	swap_array = malloc(nmemb * size);
	memcpy(swap_array, base, nmemb * size);

	/* Call the recursive mergesort implementation. */
	mergesort_recursive(swap_array, swap_array + size * (nmemb - 1),
			    base);
	free(swap_array);
}



/* mergesort_recursive() - Performs mergesort recursively on the section of
 * the array specified by pointers p and q.  Accesses the global variables
 * element_size and compare_fn.
 *
 * The dest pointer points into the destination array.  The destination array
 * alternates between the original array and the temporary array as the
 * recursion level changes.
 */
void mergesort_recursive(void *p, void *q, void *dest)
{
	void *i, *j, *k, *m, *dest_m;

	/* For the mergesort implementation to be flexible, the array is accessed
	 * by void pointers.  For void pointers, addition and subtraction will
	 * change the address pointed to by a given number of bytes.  The global
	 * variable element_size is used for updating pointers to point to the
	 * next array element.  Also, when assigning to array elements, the
	 * memcpy() function is used for copying the appropriate number of bytes.
	 */

	/* Sorting is only required if p < q; that is, two ore more elements are
	 * being sorted.
	 */
	if (p < q) {
		m = p + element_size * ((q - p) / (2 * element_size));
		dest_m = dest + (m - p);

		mergesort_recursive(dest, dest_m, p);
		m += element_size;
		dest_m += element_size;
		mergesort_recursive(dest_m, dest + (q - p), m);
		/* i.e. mergesort_recursive(dest_m, dest_q, m); */

		/* The array is sorted in the regions p to m-element_size and
		 * m to q.  Now we merge these two regions into one sorted region
		 * between p and q.
		 */

		/* i points to array elements in the first region, j points to array
		 * elements in the second region, and k points to array elements in the
		 * temporary array containing the merge result.
		 */
		i = p;
		j = m;
		k = dest;

		/* This loop performs merging while there is still at least one element
		 * left in each region.
		 */
		while (i < m && j <= q) {
			if (compare_fn(i, j) <= 0) {
				memcpy(k, i, element_size);
				i += element_size;
			} else {
				memcpy(k, j, element_size);
				j += element_size;
			}
			k += element_size;
		}

		/* Add any remaining elements in the first region to the end of the
		 * merge result.
		 */
		memcpy(k, i, m - i);


		/* Add any remaining elements in the second region to the end of the
		 * merge result.
		 */
		memcpy(k, j, (q - j) + element_size);
	}
}



/* radixsort() - Uses a radix sort to sort entries in the array pointed to by
 * base.  The array has nmemb elements, of which each is size bytes long.
 * getvalue is a pointer to a function that returns a non-negative integer
 * representing the value of an array item.  The parameter radix specifies the
 * radix to use for the sort.
 */
void radixsort(void *base, size_t nmemb, size_t size,
	       int (*getvalue) (const void *), int radix)
{
	radix_item_t **bucket_heads, **bucket_tails;
	radix_item_t *bucket_items, *first_item, *item_ptr, *next_item;
	int prev_i, x, max_value, ii;
	void *element_ptr;
	void *result;
	size_t i;

	/* Allocate space for arrays used. */
	bucket_heads = malloc(radix * sizeof(radix_item_t *));
	bucket_tails = malloc(radix * sizeof(radix_item_t *));
	item_ptr = bucket_items = malloc(nmemb * sizeof(radix_item_t));

	/* Arrange bucket items into a linked list. */
	item_ptr->ptr = element_ptr = base;
	item_ptr->value = max_value = getvalue(element_ptr);
	for (i = 1; i < nmemb; i++) {
		next_item = item_ptr + 1;
		item_ptr = item_ptr->next = next_item;
		element_ptr += element_size;

		item_ptr->ptr = element_ptr;

		x = getvalue(element_ptr);
		item_ptr->value = x;
		if (x > max_value)
			max_value = x;
	}
	item_ptr->next = NULL;


	/* Perform radix sort. */
	first_item = bucket_items;
	do {
		/* Set all bucket head and tail pointers to NULL. */
		for (ii = 0; ii < radix; i++)
			bucket_tails[ii] = bucket_heads[ii] = NULL;

		/* Scan the list of bucket items, placing each one in the appropriate
		 * bucket.
		 */
		item_ptr = first_item;
		do {
			i = item_ptr->value % radix;
			item_ptr->value /= radix;

			if (bucket_heads[i]) {
				bucket_tails[i] = bucket_tails[i]->next =
					item_ptr;
			} else {
				bucket_tails[i] = bucket_heads[i] = item_ptr;
			}

			item_ptr = item_ptr->next;
		} while (item_ptr);


		/* Now arrange bucket items back into a single linked list. */

		/* Locate the first non-empty list. */
		i = 0;
		item_ptr = bucket_heads[0];
		while (!item_ptr)
			item_ptr = bucket_heads[++i];
		first_item = item_ptr;

		/* Concatenate lists as they are located. */
		do {
			prev_i = i;

			/* Get the next non-empty list (if any). */
			do {
				i++;
				if ((int) i == radix)
					goto list_complete;
				item_ptr = bucket_heads[i];
			} while (!item_ptr);

			bucket_tails[prev_i]->next = bucket_heads[i];
		} while (1);
	      list_complete:
		bucket_tails[prev_i]->next = NULL;

		/* We use max_value to determine when all items in the resulting list
		 * are in sorted order.
		 */
		max_value /= radix;
	} while (max_value);


	/* We no longer require the bucket pointers. */
	free(bucket_heads);
	free(bucket_tails);

	/* At this point, the linked list of bucket items is sorted. */
	item_ptr = first_item;
	element_ptr = result = malloc(nmemb * size);
	for (i = 0; i < nmemb; i++) {
		memcpy(element_ptr, item_ptr->ptr, element_size);
		element_ptr += element_size;
		item_ptr = item_ptr->next;
	}
	memcpy(base, result, nmemb * size);

	free(bucket_items);
	free(result);
}


/* heapsort() - A heapsort implementation which takes the same arguments as
 * the C library function qsort() provided by stdlib.h.  It sorts the array
 * pointed to by base.  The array has nmemb elements, of which each is `size'
 * bytes long.  compare is a pointer to a comparison function.  The function
 * compares two array elements, and returns a negative, zero, or positive
 * integer depending on whether the first array element is less than, equal
 * to, or greater than the second.
 */
void heapsort(void *base, size_t nmemb, size_t size,
	      int (*compar) (const void *, const void *))
{
	int i, j, k, other_k, last_entry;
	void *y, *i_ptr;

	/* First, build the array into a binary heap.  Since array indexing starts
	 * at 0 (root node stored at index 0), an entry in the heap at position i
	 * in the array will have its left child is stored at 2i+1 and its right
	 * child at 2i+2.  Similarly, the parent of entry i is located at
	 * (i-1) div 2.
	 *
	 * Usually the heap is ordered so that a smaller items is the parent of
	 * larger items.  However, if the sorted array is to be in ascending order,
	 * the opposite order must be used in the heap.
	 */

	/* A heap is built in a bottom up manner by repeatedly using sift-up. */
	y = malloc(size);
	last_entry = (nmemb - 1) * size;
	for (i = ((nmemb - 2) / 2) * size; i >= 0; i -= size) {
		/* Perform sift-up on the subtree rooted at index i. */

		/* Copy the value of the root and initialise the insertion point and
		 * child.
		 */
		i_ptr = base + i;
		memcpy(y, i_ptr, size);
		j = i;
		k = (i << 1) + size;	/* k = 2 * i + size; */

		/* sift-up only if there is a child of the insertion point. */
		while (k <= last_entry) {

			/* Choose the maximum child unless there is only one. */
			if (k < last_entry) {
				other_k = k + size;
				if (compar(base + k, base + other_k) < 0)
					k = other_k;
			}

			/* We stop if the insertion point for the root is in the correct
			 * place.  Otherwise the child goes up and the root goes down.
			 * (i.e. swap)
			 */
			if (compar(y, base + k) >= 0)
				break;

			/* Copy child to parents position, and update pointer variables. */
			memcpy(base + j, base + k, size);
			j = k;
			k = (j << 1) + size;	/* k = 2 * j + size; */
		}

		/* At this point, the correct position for y has been found. */
		memcpy(base + j, y, size);
	}

	/* The array has now been ordered into a heap.  To complete sorting, we
	 * swap the last elements in the unsorted part of the array with the root,
	 * This increases the sorted part of the array, located after the entry
	 * pointed to by i.  Initially i is set to the last entry.  Sift-up is
	 * performed to relocate the new root.
	 *
	 * Because the new root node came from the bottom of the heap, it is likely
	 * that sift-up will return it to the bottom of the heap.  The sift-up
	 * process is made slightly more efficient by sifting up until the bottom
	 * of the heap is reached and then performing a few sift-down steps.  This
	 * way, only one key comparison is spent during each step in sift-up.
	 */

	/* Perform sift-up + sift-down until all the array is sorted. */
	i = last_entry;
	while (i > 0) {

		/* Add the largest array element in the unsorted part (the root) to the
		 * sorted part.  Keep track of the overwritten array element in y.
		 */
		memcpy(y, base + i, size);
		memcpy(base + i, base, size);
		i -= size;

		j = 0;
		k = size;

		/* Sift-up only if there is a child of the insertion point. */
		while (k <= i) {

			/* Choose the maximum child unless there is only one. */
			if (k < i) {
				other_k = k + size;
				if (compar(base + k, base + other_k) < 0)
					k = other_k;
			}

			/* Copy child to parents position, and update pointer
			 * variables.
			 */
			memcpy(base + j, base + k, size);

			j = k;
			k = (j << 1) + size;	/* k = 2 * j + size; */
		}

		/* Now sift-down if necessary to get the correct insertion point
		 * for y.  For efficiency, scale down the indexes of j and k.
		 */
		while (j > 0) {
			k = (((j / size) - 1) >> 1) * size;
			/* k = (((j / size) - 1) / 2) * size; */

			if (compar(y, base + k) <= 0)
				break;

			memcpy(base + j, base + k, size);
			j = k;
		}

		memcpy(base + j, y, size);
	}


	free(y);
}
