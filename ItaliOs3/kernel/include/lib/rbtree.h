/*** File rbtree.h - Red-Black Tree ***/
/*
 *   Shane Saunders
 */
#ifndef RBTREE_H
#define RBTREE_H
#include <dict_info.h>  /* Defines the universal dictionary structure type. */


#define RBTREE_STACK_SIZE 1000


typedef enum {Red,Black} rbtree_colour_t;


/* Structure type for nodes in the red-black tree. */
typedef struct rbtree_node {
    void *item;
    struct rbtree_node *left, *right;
    rbtree_colour_t colour;
} rbtree_node_t;

/* Structure type for the red-black tree. */
typedef struct rbtree {
    rbtree_node_t *root;
    int n;
    int (* compar)(const void *, const void *);
    rbtree_node_t **stack;
} rbtree_t;


/* rbtree_alloc() - Allocates space for a red-black tree and returns a pointer
 * to it.  The function compar compares they keys of two items, and returns a
 * negative, zero, or positive integer depending on whether the first item is
 * less than, equal to, or greater than the second.
 */
rbtree_t *rbtree_alloc(int (* compar)(const void *, const void *));

/* rbtree_free() - Frees space used by the red-black tree pointed to by t. */
void rbtree_free(rbtree_t *t);

/* rbtree_insert() - Inserts an item into the red-black tree pointed to by t,
 * according the the value its key.  The key of an item in the red-black
 * tree must be unique among items in the tree.  If an item with the same key
 * already exists in the tree, a pointer to that item is returned.  Otherwise,
 * NULL is returned, indicating insertion was successful.
 */
void *rbtree_insert(rbtree_t *t, void *item);

/* rbtree_find() - Find an item in the red-black tree with the same key as the
 * item pointed to by `key_item'.  Returns a pointer to the item found, or NULL
 * if no item was found.
 */
void *rbtree_find(rbtree_t *t, void *key_item);

/* rbtree_find_min() - Returns a pointer to the minimum item in the red-black
 * tree pointed to by t.  If there are no items in the tree a NULL pointer is
 * returned.
 */
void *rbtree_find_min(rbtree_t *t);

/* rbtree_delete() - Delete an item in the red-black tree with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the  deleted item,
 * and NULL if no item was found.
 */
void *rbtree_delete(rbtree_t *t, void *key_item);

/* rbtree_delete_min() - Deletes the item with the smallest key from the binary
 * search tree pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *rbtree_delete_min(rbtree_t *t);

/*** Alternative interface via the universal dictionary structure type. ***/
extern const dict_info_t RBTREE_info;

#endif
