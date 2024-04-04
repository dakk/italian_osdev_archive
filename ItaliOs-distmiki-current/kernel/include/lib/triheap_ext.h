#ifndef TRIHEAP_EXT_H
#define TRIHEAP_EXT_H

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
 * are undefined.  When an extra node becomes the 1st child node of a pair,
 * the correct left, right and parent pointers should be assigned to it.
 *
 * The pointer field active_entry identifies whether the node is active or not.
 * If the node is active this field will point to the active_node_t linked list
 * item.  If the node is not active this field will be NULL.
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
typedef struct triheap_ext_node {
    struct triheap_ext_node *parent;
    struct triheap_ext_node *left, *right;
    struct triheap_ext_node *child;
    struct triheap_ext_node *partner;

    int extra;
    struct active_ptr *active_entry;
    int dim;

    long key;
    int vertex_no;
} triheap_ext_node_t;


typedef struct candidate_ptr {
    int dim;

    struct candidate_ptr *next, *prev;
} candidate_ptr_t;


typedef struct active_ptr {
    triheap_ext_node_t *node;
    int position;
    
    struct active_ptr *next, *prev;
} active_ptr_t;



/* The structure type for a trinomial heap.
 *
 * trees - An array of pointers to trees at root level in the heap.  Entry i
 *         in the array points to the root node of a tree that has nodes of
 *         dimension i on the main trunk.
 * nodes - An array of pointers to nodes in the heap.  Nodes are indexed
 *         according to their vertex number.  This array can then be used to
 *         look up the node corresponding to a vertex number, and is useful
 *         when freeing space taken up by the heap.
 *
 * max_nodes - The maximum number of nodes allowed in the heap.
 * max_trees - The maximum number of trees allowed in the heap (calculated from
 *             max_nodes).
 * n     - The current number of nodes in the heap.
 * value - The binary value represented by trees in the heap.
 *         By maintaining this it is easy to keep track of the maximum rank
 *         tree in the heap.
 * t_limit - The number of active nodes required to, trigger cleanup.
 * n_active - The current number of active nodes in the heap.
 *
 * active - An array of pointers to active nodes in the heap.  No specific
 *          ordering.
 * active_ptrs - An array of pointers to queues of active nodes for each
 *               dimension.
 * candidate_ptrs - An array of pointers to entries in the candidate queue.
 * first_candidate - A pointer to the head of the candidate queue.
 *
 * key_comps - can be used for experimental purposes when counting the number
 *             of key comparisons.
 *
 * Note that the queues of candidate and active items are maintained using
 * circularly doubly linked lists.
 */
typedef struct triheap_ext {
    triheap_ext_node_t **trees;
    triheap_ext_node_t **active;
    triheap_ext_node_t **nodes;

    active_ptr_t **active_queues;
    candidate_ptr_t **candidates;
    candidate_ptr_t *first_candidate;
    
    int max_nodes, max_trees, t_limit, n, n_active, value;
    long key_comps;
} triheap_ext_t;



/*** Function prototypes. ***/

/* Trinomial heap functions. */

/* trih_ext_alloc() - creates and returns a pointer to a trinomial heap.  Argument
 * max_nodes specifies the maximum number of nodes the heap can contain.
 */
triheap_ext_t *trih_ext_alloc(int max_nodes);

/* trih_ext_free() - destroys the heap pointed to by h, freeing up any space that
 * was used by it.
 */
void trih_ext_free(triheap_ext_t *h);

/* trih_ext_insert() - creates and inserts new a node representing vertex_no with
 * key k into the heap pointed to by h.
 */
void trih_ext_insert(triheap_ext_t *h, int vertex_no, long k);

/* trih_ext_delete_min() - deletes the minimum node from the heap pointed to by h
 * and returns its vertex number.
 */
int trih_ext_delete_min(triheap_ext_t *h);

/* trih_ext_decrease_key() - For the heap pointed to by h, this function decreases
 * the key of the node corresponding to vertex_no to new_value.   No check is
 * made to ensure that new_value is in-fact less than or equal to the current
 * value, so it is up to the user of this function to ensure that it is.
 */
void trih_ext_decrease_key(triheap_ext_t *h, int vertex_no, long new_value);


/* Debugging functions. */
#if SHOW_trih
void trih_ext_dump_nodes(triheap_ext_node_t *ptr, int level);
void trih_ext_dump(triheap_ext_t *h);
#endif


/*** Alternative interface via the universal heap structure type. ***/
extern const heap_info_t TRIHEAP_EXT_info;

#endif
