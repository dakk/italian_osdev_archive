#ifndef TRIHEAP_H
#define TRIHEAP_H

/*** Header File for the Trinomial Heap Implementation ***/
/*
 *   Shane Saunders
 */
#include <heap_info.h>  /* Defines the uiversal heap structure type. */

/* This implementation uses a node-pair structure. */



/*** Option to print debugging information.  Use 1 for yes, or 0 for no. ***/
#define SHOW_trih 0



/*** Structure Types ***/

/* The structure type used for node pairs is very similar to that used for
 * the Fibonacci heap.  The main difference is that a partner pointer is used
 * which points to a nodes partner in a (2nd, 3rd) node-pair, from the nodes
 * (head, 2nd, 3rd) on a trunk.  Another naming convention is to call the
 * nodes on a trunk (parent, 1st child, 2nd child), so the 1st and 2nd child
 * nodes are paired.
 *
 * We use the boolean variable `extra' to identify whether a node is the extra
 * node (2nd child) in the node-pair.  An extra node only uses the `partner'
 * and `child' pointer, and relies on the left, right and parent pointers of
 * its partner (1st child).  Also, extra nodes are only pointed to by a
 * `partner' pointer of a 1st child node.
 *
 * Note that while a node is an extra node its left, right, and parent pointers
 * are undefined.  When an extra node becomes the main node of the pair, the
 * correct left, right and parent pointers should be assigned to it.
 *
 * The remaining structure fields are:
 * `dim'       - the nodes dimension.
 * `key'       - the nodes key.
 * `vertex_no' - the number of the graph vertex that the node corresponds to.
 *               Vertex numbering in the graph should be:
 *               1, 2, 3, ... max_vertex.
 *
 * In this implementation, dimensioning of nodes begins at zero, so the
 * dimension of a single node with no children is zero.
 */
typedef struct triheap_node {
    struct triheap_node *parent;
    struct triheap_node *left, *right;
    struct triheap_node *child;
    struct triheap_node *partner;

    int extra;    
    int dim;

    long key;
    int vertex_no;
} triheap_node_t;



/* The structure type for a trinomial heap.
 *
 * trees - An array of pointers to trees at root level in the heap.  Entry i
 *         in the array points to the root node of a tree that has nodes of
 *         dimension i on the main trunk.
 * active - An array of pointers to active nodes in the heap.
 * nodes - An array of pointers to nodes in the heap.  Nodes are indexed
 *         according to their vertex number.  This array can then be used to
 *         look up the node corresponding to a vertex number, and is useful
 *         when freeing space taken up by the heap.
 * max_nodes - The maximum number of nodes allowed in the heap.
 * max_trees - The maximum number of trees allowed in the heap (calculated from
 *             max_nodes).
 * n     - The current number of nodes in the heap.
 * value - The binary value represented by trees in the heap.
 *         By maintaining this it is easy to keep track of the maximum rank
 *         tree in the heap.
 * key_comps - can be used for experimental purposes when counting the number
 *             of key comparisons.
 */
typedef struct triheap {
    triheap_node_t **trees;
    triheap_node_t **active;
    triheap_node_t **nodes;
    int max_nodes, max_trees, n, value;
    long key_comps;
} triheap_t;



/*** Function prototypes. ***/

/* trinomial heap functions. */

/* trih_alloc() - creates and returns a pointer to a trinomial heap.  Argument
 * max_nodes specifies the maximum number of nodes the heap can contain.
 */
triheap_t *trih_alloc(int max_nodes);

/* trih_free() - destroys the heap pointed to by h, freeing up any space that
 * was used by it.
 */
void trih_free(triheap_t *h);

/* trih_insert() - creates and inserts new a node representing vertex_no with
 * key k into the heap pointed to by h.
 */
void trih_insert(triheap_t *h, int vertex_no, long k);

/* trih_delete_min() - deletes the minimum node from the heap pointed to by h
 * and returns its vertex number.
 */
int trih_delete_min(triheap_t *h);

/* trih_decrease_key() - For the heap pointed to by h, this function decreases
 * the key of the node corresponding to vertex_no to new_value.   No check is
 * made to ensure that new_value is in-fact less than or equal to the current
 * value, so it is up to the user of this function to ensure that it is.
 */
void trih_decrease_key(triheap_t *h, int vertex_no, long new_value);


/* Debugging functions. */
#if SHOW_trih
void trih_dump_nodes(triheap_node_t *ptr, int level);
void trih_dump(triheap_t *h);
#endif


/*** Alternative interface via the universal heap structure type. ***/
extern const heap_info_t TRIHEAP_info;

#endif
