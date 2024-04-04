#ifndef DST_H
#define DST_H
#include <dict_info.h>  /* Defines the universal dictionary structure type. */

/* This digital search tree implementation has code for two different
 * rearangement schemes after deleting a node.  Use #define DST_FAST_DELETE 1
 * to use the code which replaces the deleted node with a decendant leaf node.
 * Otherwise use #define DST_FAST_DELETE 0 to use the code which replaces
 * the deleted node, by sifting up nodes below it.
 */
#define DST_FAST_DELETE 0


typedef struct dst_node {
    int key;
    void *item;
    struct dst_node *a[2];
} dst_node_t;


typedef struct dst {
    dst_node_t *root;
    int n;
    int max_b;
    unsigned int (* get_value)(const void *);
} dst_t;


/* dst_alloc() - Allocates space for a digital search tree and returns a
 * pointer to it.  The function get_value() returns the key value for a data
 * item.
 */
dst_t *dst_alloc(unsigned int (* get_value)(const void *), int max_b);

/* dst_free() - Frees space used by the digital search tree pointed to by t.
 */
void dst_free(dst_t *t);

/* dst_insert() - Inserts an item into the digital search tree pointed to by
 * t, according the the value its key.  The key of an item in the digital
 * search tree tree must be unique among items in the tree.  If an item with
 * the same key already exists in the tree, a pointer to that item is returned.
 * Otherwise, NULL is returned, indicating insertion was successful.
 */
void *dst_insert(dst_t *t, void *item);

/* dst_find() - Find an item in the digital search tree with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the item found, or
 * NULL if no item was found.
 */
void *dst_find(dst_t *t, void *key_item);

/* dst_find_min() - Returns a pointer to the minimum item in the digital
 * search tree tree pointed to by t.  If there are no items in the tree, a NULL
 * pointer is returned.
 */
void *dst_find_min(dst_t *t);

/* dst_delete() - Delete the first item found in the digital search tree with
 * the same key as the item pointed to by `key_item'.  Returns a pointer to the
 * deleted item, and NULL if no item was found.
 */
void *dst_delete(dst_t *t, void *key_item);

/* dst_delete_min() - Deletes the item with the smallest key from the digital
 * search tree pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *dst_delete_min(dst_t *t);

/*** Alternative interface via the universal dictionary structure type. ***/
extern const dict_info_t DST_info;

#endif
