#ifndef DFS_BFS_H
#define DFS_BFS_H
/*** File:  sc.h - Tarjan's Strongly Connected Components Algorithm ***/
/*
 *   Shane Saunders
 */
#include <dgraph.h>



/* The resulting SC components are represented using arrays of vertex numbers.

 * Field sets_n stores the number of SC components found in the search.
 * Field size specifies the array sizes.
 *
 * The SC components are stored using three arrays:
 *   - sets_s[i] gives the starting position in the vertices[] array of SC
 *     component i.
 *   - sets_f[i] gives the finishing position in the vertices[] array of SC
 *     component i.
 *   - vertices[] is used for storing the vertex numbers of vertices in the
 *     SC components.
 * For example if there are three SC components the vertices in each are stored
 * as follows:
 *   - SC0:  vertices[sets_s[0]] ... vertices[sets_f[0]].
 *   - SC1:  vertices[sets_s[1]] ... vertices[sets_f[1]].
 *   - SC2:  vertices[sets_s[2]] ... vertices[sets_f[2]].
 * Note that array entries sets[3] onwards are set to UNDEFINED.
 */
typedef struct sc_result {
    int size, n_sets;
    int *vertices;
    int *sets_s, *sets_f;
} sc_result_t;



/* sc() - Generates SC components using Tarjan's algorithm.  Pointer g
 * specifies the graph, and v specifies the starting vertex for the depth first
 * search.  The result is returned as a pointer to a sc_result_t structure.
 */
sc_result_t *sc(dgraph_t *g, int v);
    
/* sc_recursive() - Tarjan's SC component algorithm, proceeds as a depth
 * first search from vertex v in the graph.
 */
void sc_recursive(int v);

/* sc_result_free() - Frees space used by a SC component result structure.
 */
void sc_result_free(sc_result_t *r);

/* sc_result_print() - Displays the SC components stored in an SC component
 * result structure.
 */
void sc_result_print(sc_result_t *r);

#endif
