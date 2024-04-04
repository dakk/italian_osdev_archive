#ifndef TREE23_H
#define TREE23_H
/*** File 2_3_tree_ext.h - 2-3 Tree Header File. ***/
/*
 *   Shane Saunders
 */

/* This 2-3 tree implementation supports O(1) worst case time for delete_min.
 */

/* There are two kinds of child pointers for nodes in the 2-3 tree.  The first
 * kind are pointers to other nodes.  The second kind are pointers to the leaf
 * items (data items) in the tree.
 */
struct tree23_node;
typedef union tree23_link {
    struct tree23_node *node;  /* link to internal node... */
    void *item;                   /* ... or a link to a data item. */
} tree23_link_t;

/* A node in the 2-3 tree maintains two keys and three child pointers.
 * A node in the 2-3 tree has between two and three children, except for the
 * special case where it is the root node of a tree containing only 0 or 1
 * nodes.  When a nodes has two children its left and middle child pointers are
 * used, and its right child pointer is NULL.  For a node which has three
 * children all child pointer are used.
 *
 * A node, x, in the 2-3 tree has two keys:
 *    key_node1 - points to the parent node of the smallest item among items
 *                in the x's middle-child subtree.  If x's middle child pointer
 *                specifies a data items rather than a node, then key_node1 is
 *                NULL.
 *    key_node2 - points to the item with the smallest key among items in the
 *                nodes right-child subtree.  If x has no right child, or x's
 *                right child pointer specifies a data item rather than a node,
 *                then key_node2 is NULL.
 *
 * NOTE:  The key_node1 field of x indicates the kind of child pointers x has.
 *        If the key_node1 field of x is NULL, then key_node2 is also NULL, and
 *        the children of x are data items.  Otherwise the children are nodes.
 *
 * The properties of child trees of a node in a 2-3 tree are:
 *     - data items i stored under the right-child subtree have
 *       key(key_node2->left.item) <= key(i).
 *     - data items i stored under the middle-child subtree have
 *       key(key_node1->left.item) <= key(i) < key(key_node2->left.item).
 *     - data items i stored under the left-child subtree have
 *       key(i) < key(key_node2->left.item).
 */
typedef struct tree23_node {
    struct tree23_node *key_node1, *key_node2;
    struct tree23_node *parent;
    tree23_link_t left, middle, right;
} tree23_node_t;

/* The 2-3 tree structure type has the following fields:
 *   root - a pointer to the root node.
 *   compar - a pointer to the function to use for comparing the keys of items
 *            in the tree.
 *   n - the number of data items stored in the tree.
 *   height - the height of the tree.
 *   min_node - a pointer to the parent node of the minimum item in the 2-3
 *              tree.
 */
typedef struct tree23 {
    tree23_node_t *root, *min_node;
    int (* compar)(const void *, const void *);
    int n, height;
} tree23_t;



/* tree23_alloc() - Allocates space for a 2-3 tree and returns a pointer to
 * it.  The function compar compares they keys of two items, and returns a
 * negative, zero, or positive integer depending on whether the first item is
 * less than, equal to, or greater than the second.
 */
tree23_t *tree23_alloc(int (* compar)(const void *, const void *));

/* tree23_free() - Frees space used by the 2-3 tree pointed to by t. */
void tree23_free(tree23_t *t);

/* tree23_insert() - Inserts an item into the 2-3 tree pointed to by t,
 * according the the value its key.  The key of an item in the 2-3 tree must
 * be unique among items in the tree.  If an item with the same key already
 * exists in the tree, a pointer to that item is returned.  Otherwise, NULL is
 * returned, indicating insertion was successful.
 */
void *tree23_insert(tree23_t *t, void *item);

/* tree23_find() - Find an item in the 2-3 tree with the same key as the
 * item pointed to by `key_item'.  Returns a pointer to the item found, or NULL
 * if no item was found.
 */
void *tree23_find(tree23_t *t, void *key_item);

/* tree23_find_min() - Returns a pointer to the minimum item in the 2-3 tree
 * pointed to by t.  If there are no items in the tree a NULL pointer is
 * returned.
 */
void *tree23_find_min(tree23_t *t);

/* tree23_delete() - Delete the item in the 2-3 tree with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the deleted item,
 * and NULL if no item was found.
 */
void *tree23_delete(tree23_t *t, void *key_item);

/* tree23_delete_min() - Deletes the item with the smallest key from the
 * binary search tree pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *tree23_delete_min(tree23_t *t);

#endif
