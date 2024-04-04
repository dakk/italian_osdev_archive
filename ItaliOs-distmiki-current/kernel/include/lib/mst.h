/***  File: mst.h - Minimal Spanning Tree Algorithms ***/
/*
 *  Shane Saunders
 */
#ifndef PRIM_H
#define PRIM_H
#include <heap_info.h>
#include <dgraph.h>

/* Prim's algorithm results structure. */
typedef struct mst_result {
    int n;  /* n is the size of the graph the result is for. */
    int *reached;  /* Array; reached[w] is the vertex from which w is reached
		    */
    long *d;  /* Array; d[w] is the edge cost from reached[w] to w. */
} mst_result_t;

/*** Function prototypes. ***/
mst_result_t *mst_prim(const dgraph_t *g, int v0, const heap_info_t *heap_info);
void mst_result_free(mst_result_t *r);

#endif /* PRIM_H */
