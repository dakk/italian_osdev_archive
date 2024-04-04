#ifndef MF_H
#define MF_H
/*** Maximum Flow Algorithms ***/

/* The file provides maximum flow algorithms, and a network structure type for
 * use with these algorithms.  The maximum flow algorithms update the flow
 * field of edges in the network supplied to them.  This header file defines a
 * suitable structure type for the networks used by the maximum flow
 * algorithms, and supplies a function to generate a suitable random network
 * structure for testing the algorithms.
 */

/*** --- Output Format Settings  ----------------------------------------- ***/

/* Set this to 1 to have print_network output the graph file language used by
 * the directed graph preprocessor "dot".
 */
#define OUTPUT_GRAPH_LANG 1


/*** --- Network Structure Type ------------------------------------------ ***/

/* Edge structure type.
 *   source_no, dest_no  - The source an destination vertex numbers.
 *   next_out, next_in   - Next edge in the same OUT/IN set.
 *   next                - Next edge in the list of all edges.
 */
typedef struct network_edge {
    int source_no, dest_no;
    int capacity;
    int flow;
    struct network_edge *next_out;
    struct network_edge *next_in;
    struct network_edge *next;
} network_edge_t;

/* Vertex structure type.
 * The OUT set and IN set of edges is maintained for each vertex.
 */
typedef struct network_vertex {
    network_edge_t *out_head;  /* Pointer to the first edge in OUT Set. */
    network_edge_t *out_tail;  /* Pointer to the last edge in OUT Set. */
    network_edge_t *in_head;  /* Pointer to the first edge in IN Set. */
    network_edge_t *in_tail;  /* Pointer to the last edge in IN Set. */
} network_vertex_t;

/* Network graph structure type.
 *     n - the number of nodes in the graph.
 *     s, t - the source and sink vertex numbers.
 *     vertices - an array containing vertices in the graph.
 *     first_edge, last_edge - First and last edge in the list of all edges.
 */
typedef struct network {
    int n;
    int s, t;
    network_vertex_t *vertices;
    network_edge_t *first_edge, *last_edge;
} network_t;


/*** --- Maximum Flow Functions --- ***/

/* The following maximum flow algorithms compute a maximum flow on the graph
 * pointed to by g, and update the flow of edges in the graph to reflect this.
 * Before using these algorithms, the edges of the graph should have been
 * assigned  some legal initial flow, such as all zero.
 */
void mf_ford(network_t *g);      /* Ford and Fulkerson's Algorithm */
void mf_dinic(network_t *g);     /* Dinic's Algorithm */
void mf_mpm(network_t *g);       /* MPM Algorithm */
void mf_karzanov(network_t *g);  /* Karzanov Algorithm */

/*** --- Network Functions --- ***/

/* This algorithm generates an almost random network with n vertices and m
 * edges, with edge capacities uniformly distributed between min_cap and
 * max_cap.  The time complexity of the algorithm is  at worst O(mn), and the
 * algorithm uses O(m) space.  All edge flows in the network are initialised to
 * zero.
 *
 * It is up to the user of this function to use values of m between n-1 and
 * n*(n-1).
 */
network_t *network_rand(int n, int m, int min_cap, int max_cap);

/* Reset all edge flows in the network pointed to by g to zero. */
void network_reset(network_t *g);

/* Free space which is used by the network pointed to by g, including space
 * used by the edges of g.
 */
void network_free(network_t *g);

/* This prints a text representation of the network pointed to by g to the
 * standard output.
 */
void network_print(network_t *g);

/* This function returns the total flow going through the network. */
int network_flow(network_t *g);


#endif  /* MF_H */
