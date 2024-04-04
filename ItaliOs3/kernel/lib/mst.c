/***  File: mst.c - Minimal Spanning Tree Algorithms ***/
/*
 *    Shane Saunders
 */

#include <stdlib.h>
#include <mst.h>


/*** Special values. ***/
#define TRUE 1
#define FALSE 0


/* mst_prim() - Minimum spanning tree algorithm.  Computes the minimum spanning
 * tree of the directed graph pointed to by g.  Returns the result as a
 * prim_result_t structure (see prim.h).
 */
mst_result_t *mst_prim(const dgraph_t * g, int v0,
		       const heap_info_t * heap_info)
{
	int v, w;
	int *s;
	long dist, *d;
	int i, n;

	dgraph_vertex_t *vertices;
	dgraph_edge_t *edge_ptr;

	void *front;
	int (*heap_delete_min) (void *);
	void (*heap_insert) (void *, int, long);
	void (*heap_decrease_key) (void *, int, long);
	int (*heap_n) (void *);
	void *(*heap_alloc) (int);
	void (*heap_free) (void *);

	mst_result_t *result;
	int *reached;


	/* Set up pointers to heap interface functions. */
	heap_delete_min = heap_info->delete_min;
	heap_insert = heap_info->insert;
	heap_decrease_key = heap_info->decrease_key;
	heap_n = heap_info->n;
	heap_alloc = heap_info->alloc;
	heap_free = heap_info->free;

	/* Provide quick access to graph fields. */
	vertices = g->vertices;
	n = g->n;

	/* Allocate result structure. */
	result = malloc(n * sizeof(mst_result_t));
	reached = result->reached = malloc(n * sizeof(int));
	d = result->d = calloc(n, sizeof(long));

	/* Allocate arrays used by the algorithm. */
	s = calloc(n, sizeof(int));

	/* Create the heap. */
	front = heap_alloc(n);

	/* Initially all vertices are unreached. */
	for (i = 0; i < n; i++)
		reached[i] = -1;

	/* The start vertex is part of the solution set. */
	s[v0] = TRUE;
	d[v0] = 0;

	/* Put out set of the starting vertex into the frontier and update the
	 * distances to vertices in the out set.  k is the index for the out set.
	 */
	edge_ptr = vertices[v0].first_edge;
	while (edge_ptr) {
		w = edge_ptr->vertex_no;
		dist = d[w] = edge_ptr->dist;
		heap_insert(front, w, dist);
		reached[w] = v0;
		edge_ptr = edge_ptr->next;
	}

	/* At this point we are assuming that all vertices are reachable from the
	 * starting vertex and N > 1 so that j > 0.
	 */

	while (heap_n(front) > 0) {

		/* Find the vertex in frontier that has minimum distance. */
		v = heap_delete_min(front);

		/* Add the vertex to the solution set. */
		s[v] = TRUE;

		/* Update distances to vertices, w, in the out set of v.
		 */
		edge_ptr = vertices[v].first_edge;
		while (edge_ptr) {
			w = edge_ptr->vertex_no;

			/* Only update if w is not already in the solution set.
			 */
			if (!s[w]) {

				/* If w is in the frontier (reached[w] >= 0) the new edge
				 * distance to w is the minimum of its current distance and the
				 * edge distance (v,w).
				 */
				dist = edge_ptr->dist;
				if (reached[w] >= 0) {
					if (dist < d[w]) {
						d[w] = dist;
						heap_decrease_key(front, w,
								  dist);
						reached[w] = v;
					}
				} else {
					d[w] = dist;
					heap_insert(front, w, dist);
					reached[w] = v;
				}
			}
			/* if */
			edge_ptr = edge_ptr->next;
		}		/* while */
	}			/* while */


	/* Free space used by arrays local to this function. */
	free(s);
	heap_free(front);

	return result;
}


/* mst_result_free() - frees up space used by a prim_result_t structure. */
void mst_result_free(mst_result_t * r)
{
	free(r->reached);
	free(r->d);
	free(r);
}
