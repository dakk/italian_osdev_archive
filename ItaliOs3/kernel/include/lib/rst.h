#ifndef RST_H
#define RST_H
#include <dict_info.h>  /* Defines the universal dictionary structure type. */

/* This radix search trie implementation has code for two different
 * rearrangement schemes after deleting a node.  Use #define RST_FAST_DELETE 1
 * to use the code which replaces the deleted node with a descendant leaf node.
 * Otherwise use #define RST_FAST_DELETE 0 to use the code which replaces
 * the deleted node, by sifting up nodes below it.
 */
#define RST_FAST_DELETE 0


/* Structure type definition for nodes in the radix search trie.  The
 * (j+1)th bit in p->child_links indicates the kind of pointer for p->a[j].
 * For example, if bit 0 is 1 then p->a[0] points to another rst_note
 * structure.  Otherwise p->a[0] is NULL or points to a data item.
 */
typedef struct rst_node {
    int child_links;
    void *a[2];
} rst_node_t;

/* Structure type definition for a radix search trie. */
typedef struct rst {
    rst_node_t *root;
    int n;
    int max_b;
    unsigned int (* get_value)(const void *);
    rst_node_t **stack;
    int *path_info;
} rst_t;


/* rst_alloc() - Allocates space for a radix search trie and returns a
 * pointer to it.  The function get_value() returns the key value for a data
 * item.
 */
rst_t *rst_alloc(unsigned int (* get_value)(const void *), int max_b);

/* rst_free() - Frees space used by the radix search trie pointed to by t.
 */
void rst_free(rst_t *t);

/* rst_insert() - Inserts an item into the radix search trie pointed to by
 * t, according the the value its key.  The key of an item in the radix
 * search trie tree must be unique among items in the tree.  If an item with
 * the same key already exists in the tree, a pointer to that item is returned.
 * Otherwise, NULL is returned, indicating insertion was successful.
 */
void *rst_insert(rst_t *t, void *item);

/* rst_find() - Find an item in the radix search trie with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the item found, or
 * NULL if no item was found.
 */
void *rst_find(rst_t *t, void *key_item);

/* rst_find_min() - Returns a pointer to the minimum item in the radix
 * search trie pointed to by t.  If there are no items in the tree, a NULL
 * pointer is returned.
 */
void *rst_find_min(rst_t *t);

/* rst_delete() - Delete the first item found in the radix search trie with
 * the same key as the item pointed to by `key_item'.  Returns a pointer to the
 * deleted item, and NULL if no item was found.
 */
void *rst_delete(rst_t *t, void *key_item);

/* rst_delete_min() - Deletes the item with the smallest key from the radix
 * search trie pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *rst_delete_min(rst_t *t);

/*** Alternative interface via the universal dictionary structure type. ***/
extern const dict_info_t RST_info;

#endif
