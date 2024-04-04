#ifndef DGRAPH_H
#define DGRAPH_H
/*** File: dgraph.h - Supplies Directed Random Graphs ***/
/*
 *   Shane Saunders
 */


/*** Structure types used for directed random graphs. ***/

/* Struture type for out set items.  Each item stores the vertex number
 * corresponding to a vertex in the OUT set, and the distance.
 */
typedef struct dgraph_edge {
    int vertex_no;
    int dist;
    struct dgraph_edge *next;
} dgraph_edge_t;

/* Vertex structure type.
 */
typedef struct dgraph_vertex {
    dgraph_edge_t *first_edge;
    dgraph_edge_t *last_edge;
} dgraph_vertex_t;

/* Directed graph structure type.
 *     n - the number of nodes in the graph.
 *     vertices - an array containing vertices in the graph.
 */
typedef struct dgraph {
    int n;
    dgraph_vertex_t *vertices;
} dgraph_t;


/*** Prototypes of functions supplied by this header file. ***/

/* dgraph_blank() - creates a graph with n vertices but no endges.  Can be used
 * when an outside algorithm generates the graphs edges.
 */
dgraph_t *dgraph_blank(int n);
    
/* dgraph_rnd_sparse() - creates a directed random graph with all vertices
 * reachable from the starting vertex.  Parameter n is the size of the graph,
 * and parameter edge_factor is the average OUT set size for vertices.
 * Returns a pointer to the random graph created.
 * For freeing space used by the random graph see dgraph_free().
 */
dgraph_t *dgraph_rnd_sparse(int n, double edge_factor);

/* dgraph_rnd_dense() - creates a directed random graph with all vertices
 * reachable from the starting vertex.  Parameter n is the size of the graph,
 * and parameter prob is the probability of edge existence.
 * Returns a pointer to the random graph created.
 */
dgraph_t *dgraph_rnd_dense(int n, double prob);

/* dgraph_rnd_tree_parts() - create a random graph made up of tree parts.
 * n_trees - the number of trees.
 * tree_size - number of vertices per tree.
 * branch_f - branching factor of trees.
 * ext_f - Number of additional external edges into each tree root.
 *         Note that each leaf vertex of a tree automatically causes the
 *         creation of an edge to the root vertex of some tree.
 * The user should ensure that:
 *     1.  tree_size > branch_f.  (do not allow edge from a root vertex
 *                                 to itself.)
 *     2.  n_trees >= branch_f.
 * Otherwise the algorithm may run indefinitely trying to generate the graph.
 */
dgraph_t *dgraph_rnd_tree_parts(int n_trees, int tree_size,
				int branch_f, int ext_f);

/* Same as dgraph_rnd_tree_parts(), except an acyclic structure is used which
 * is like a tree with additional edges to grandchildren.
 */
dgraph_t *dgraph_rnd_acyclic_parts(int n_trees, int tree_size,
			           int branch_f, int ext_f);

/* add_new_edge() - adds a new edge to the edge list of the vertex pointed to
 * by `vertex'.  The edge has destination vertex number drst_vertex_no, and
 * distance dist.
 */
void add_new_edge(dgraph_vertex_t *vertex, int dest_vertex_no, long dist);

/* dgraph_free() - frees space used by a directed random graph created using
 * dgraph_create().
 */
void dgraph_free(dgraph_t *g);



/*** Global Constants ***/

/* Global constant for the number of the source vertex of the graph.
 * This is always vertex number 0.
 */
extern const int StartVertex;


#endif
