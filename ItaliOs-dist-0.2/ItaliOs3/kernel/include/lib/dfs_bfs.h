#ifndef DFS_BFS_H
#define DFS_BFS_H
/*** File:  dfs_bfs.h - Depth First Search and Breadth First Search ***/
/*
 *   Shane Saunders
 */
#include <dgraph.h>



/* The result of dfs is represented using arrays of vertex numbers.
 * Array entries (parents[i], nodes[i]) represent a directed edge from the
 * depth first search, where vertices[i] has the associated visit_no i.
 * The array entry visit_nos[i] specifies the visit number of vertex i.
 * That is the array visti_nos[] is the inverse of the nodes array.
 * Field n stores the number of nodes reached in the search.
 * Filed size specifies the array sizes.
 */
typedef struct dfs_bfs_result {
    int n, size;
    int *vertices;
    int *parents;
    int *visit_nos;
} dfs_bfs_result_t;



/* dfs() - Performs a depth first search on the graph pointed to by g, and
 * returns an array of edges with a length equal to the number of vertices in
 * g.  The search begins at vertex number v.
 */
dfs_bfs_result_t *dfs(dgraph_t *g, int v);

/* bfs() - performs a breadth first search on the graph pointed to by g, and
 * returns an array of edges with a length equal to the number of vertices in
 * g.  The search begins at vertex v.
 */
dfs_bfs_result_t *bfs(dgraph_t *g, int v);

/* dfs_bfs_result_free() - Frees space used by a dfs/bfs search result
 * structure.
 */
void dfs_bfs_result_free(dfs_bfs_result_t *r);

/* dfs_bfs_result_print() - Displays the result of a depth first or breadth
 * first search.
 */
void dfs_bfs_result_print(dfs_bfs_result_t *r);



#endif
