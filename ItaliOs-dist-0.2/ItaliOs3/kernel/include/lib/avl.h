/*** File avl.h - AVL Tree ***/
/*
 *   Shane Saunders
 */
#ifndef AVL_H
#define AVL_H
#include <dict_info.h>  /* Defines the universal dictionary structure type. */


/* Size of the stack used for storing path travesersed through the tree.
 * This should be defined to be plenty large enough to accomodate the maximum
 * practical tree depth which could be expected.
 */
#define AVL_STACK_SIZE 1000


/* Structure type for nodes in the AVL tree. */
typedef struct avl_node {
    void *item;
    struct avl_node *left, *right;
    signed char balance;
} avl_node_t;

/* Structure type for the AVL tree. */
typedef struct avl {
    avl_node_t *root;
    int n;
    int (* compar)(const void *, const void *);
    avl_node_t **stack;
    signed char *path_info;
} avl_t;



/* avl_alloc() - Allocates space for a AVL tree and returns a pointer to it.
 * The function compar compares they keys of two items, and returns a negative,
 * zero, or positive integer depending on whether the first item is less than,
 * equal to, or greater than the second.
 */
avl_t *avl_alloc(int (* compar)(const void *, const void *));

/* avl_free() - Frees space used by the AVL tree pointed to by t. */
void avl_free(avl_t *t);

/* avl_insert() - Inserts an item into the AVL tree pointed to by t,
 * according the the value its key.  The key of an item in the AVL tree must
 * be unique among items in the tree.  If an item with the same key already
 * exists in the tree, a pointer to that item is returned.  Otherwise, NULL is
 * returned, indicating insertion was successful.
 */
void *avl_insert(avl_t *t, void *item);

/* avl_find() - Find an item in the AVL tree with the same key as the
 * item pointed to by `key_item'.  Returns a pointer to the item found, or NULL
 * if no item was found.
 */
void *avl_find(avl_t *t, void *key_item);

/* avl_find_min() - Returns a pointer to the minimum item in the AVL
 * tree pointed to by t.  If there are no items in the tree a NULL pointer is
 * returned.
 */
void *avl_find_min(avl_t *t);

/* avl_delete() - Delete the first item found in the AVL tree with
 * the same key as the item pointed to by `key_item'.  Returns a pointer to the
 * deleted item, and NULL if no item was found.
 */
void *avl_delete(avl_t *t, void *key_item);

/* avl_delete_min() - Deletes the item with the smallest key from the AVL
 * tree pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *avl_delete_min(avl_t *t);

/*** Alternative interface via the universal dictionary structure type. ***/
extern const dict_info_t AVL_info;

#endif
