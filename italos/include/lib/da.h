/*** da.h - Provides implementations of dijkstra's algorithm. ***/
/*
 *  Shane Saunders
 */
#ifndef DA_H
#define DA_H
#include <timing.h>
#include <heap_info.h>
#include <dgraph.h>


/* To enable printing of heap infomation use #define DA_HEAP_DUMP 1, otherwise
 * use #define DA_HEAP_DUMP 0.
 */
#define DA_HEAP_DUMP 0



/* Dijkstra's algorithm results structure. */
typedef struct da_result {
    int n;
    long *d;
    clockval_t ticks;
    long key_comps;
} da_result_t;



/*** Function prototypes. ***/

da_result_t *heap_dijkstra(const dgraph_t *g, int v0,
			   const heap_info_t *heap_info);

void da_result_free(da_result_t *r);

#endif /* DA_H */
