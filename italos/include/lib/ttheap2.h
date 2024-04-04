#ifndef TTHEAP2_H
#define TTHEAP2_H

/*** Header File for the 2-3 Heap Implementation ***/
/*
 *   Shane Saunders
 */

/* This implementation uses a node-pair structure. */
#include <heap_info.h>  /* Defines the uiversal heap structure type. */



/*** Option to print debugging information.  Use 1 for yes, or 0 for no. ***/
#define TTHEAP_DUMP 0



/*** Structure Types ***/

/* The structure type used for node pairs is very similar to that used for
 * the Fibonacci heap.  The main difference is that a partner pointer is used
 * which points to a nodes partner in a (2nd, 3rd) node-pair.  We use the
 * boolean variable `extra' to identify whether a node is the extra node in
 * the node-pair.  An extra node only uses the `partner' and `child' pointer,
 * and relies on the left, right and parent pointers of its partner. Also,
 * extra nodes are only pointed to by a `partner' pointer.
 *
 * Note that while a node is an extra node its left, right, and parent pointers
 * are undefined.  When an extra node becomes the main node of the pair, the
 * correct left, right and parent pointers should be assigned to it.
 *
 * For this description, we refer to the child pointers of a node and the extra
 * node in a node pair as child1 and child2 respectively.
 * Suppose the dimension of the two nodes in a pair is i.  The left pointer of
 * child1, and the child pointers child1 and child2 point to the three
 * (2nd,3rd) node pairs of dimension i-1 in the lower work space.
 *
 * Suppose we have a pointer to one nodes in the work space.  All six
 * dimension i nodes in the workspace can be accessed by following the
 * parent pointer to the dimension i+1 node pair, then using the three pointers
 * `left', child1, and child2.
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
typedef struct ttheap_node {
    struct ttheap_node *parent;
    struct ttheap_node *left, *right;
    struct ttheap_node *child;
    struct ttheap_node *partner;

    int extra;    
    int dim;

    long key;
    int vertex_no;
} ttheap_node_t;



/* The structure type for a 2-3 heap.
 *
 * trees - An array of pointers to trees at root level in the heap.  Entry i
 *         in the array points to the root node of a tree that has nodes of
 *         dimension i on the main trunk.
 * nodes - An array of pointers to nodes in the heap.  Nodes are indexed
 *         according to their vertex number.  This array can then be used to
 *         look up the node for corresponding to a vertex number, and is
 *         useful when freeing space taken up by the heap.
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
typedef struct ttheap {
    ttheap_node_t **trees;
    ttheap_node_t **nodes;
    int max_nodes, max_trees, n, value;
    long key_comps;
} ttheap_t;



/*** Function prototypes. ***/

/* 2-3 heap functions. */

/* tth_alloc() - creates and returns a pointer to a 2-3 heap.  Argument
 * max_nodes specifies the maximum number of nodes the heap can contain.
 */
ttheap_t *tth_alloc(int max_nodes);

/* tth_free() - destroys the heap pointed to by h, freeing up any space that
 * was used by it.
 */
void tth_free(ttheap_t *h);

/* tth_insert() - creates and inserts new a node representing vertex_no with
 * key k into the heap pointed to by h.
 */
void tth_insert(ttheap_t *h, int vertex_no, long k);

/* tth_delete_min() - deletes the minimum node from the heap pointed to by h
 * and returns its vertex number.
 */
int tth_delete_min(ttheap_t *h);

/* tth_decrease_key() - For the heap pointed to by h, this function decreases
 * the key of the node corresponding to vertex_no to new_value.   No check is
 * made to ensure that new_value is in-fact less than or equal to the current
 * value, so it is up to the user of this function to ensure that this holds.
 */
void tth_decrease_key(ttheap_t *h, int vertex_no, long new_value);


/* Debugging Functions. */
#if TTHEAP_DUMP
void tth_dump(ttheap_t *h);
void tth_dump_nodes(ttheap_node_t *ptr, int level);
#endif


/*** Alternative interface via the universal heap structure type. ***/
extern const heap_info_t TTHEAP_info;

#endif
