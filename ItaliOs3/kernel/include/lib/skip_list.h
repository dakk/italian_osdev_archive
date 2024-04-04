#ifndef SKIP_LIST_H
#define SKIP_LIST_H
/*** File skip_list.h - Skip List ***/
/*
 *   Shane Saunders
 */

/* Structure type for skip list nodes.  For flexibility, nodes point to items,
 * rather than having integer keys, and the skip list uses a comparison
 * function for comparing items.
 *   item - The nodes item.
 *   forward - An array of pointers pointing forward to other nodes in the
 *             list.
 *   size - the size of the forward array.  (i.e. the number of forward
 *          pointers the node has.)
 */
typedef struct skip_node {
    void *item;
    struct skip_node **forward;
    int size;
} skip_node_t;


/* Structure type definition for a skip list.
 *   head_ptrs - An array of head pointers.
 *   update - Is used when updating pointers after a node is inserted or
 *            deleted.  An entry, update[i] points to the node whose
 *            forward[i] pointer is to be updated.  The array is allocated
 *            when the list is created.  This prevents having to allocate and
 *            free the array for each insert or delete operation.
 *   p - The probability with which a node size is allocated.  p is always less
 *       than or equal to 1.  The probability of node being allocated sizes
 *       1, 2, 3, ... , n, is  p, p^2, p^3, ... , p^n, respectively.
 *   compar - A function for comparing items in the list.
 *   n - the number of items currently stored in the list.
 */
typedef struct skip_list {
    skip_node_t **head_ptrs;
    skip_node_t ***update;
    double p;
    int max_level;
    int (* compar)(const void *, const void *);
    int n;
} skip_list_t;



/* skip_list_alloc() - Allocates space for a skip list and returns a pointer to
 * it.  The parameter, max_n, specifies the expected maximum number of nodes to
 * be stored in the list. The parameter, prob, specifies the probability for
 * height increase when generating random node heights.  The function compar
 * compares they keys of two items, and returns a negative, zero, or positive
 * integer depending on whether the first item is less than, equal to, or
 * greater than the second.  
 */
skip_list_t *skip_list_alloc(int max_n, double prob, int (* compar)(const void *, const void *));

/* skip_list_free() - Frees space used by the skip list pointed to by t. */
void skip_list_free(skip_list_t *t);

/* skip_list_insert() - Inserts an item into the skip list pointed to by t,
 * according the the value its key.  The key of an item in the skip list must
 * be unique among items in the list.  If an item with the same key already
 * exists in the list, a pointer to that item is returned.  Otherwise, NULL is
 * returned, indicating insertion was successful.
 */
void *skip_list_insert(skip_list_t *t, void *item);

/* skip_list_find() - Find an item in the skip list with the same key as the
 * item pointed to by `key_item'.  Returns a pointer to the item found, or NULL
 * if no item was found.
 */
void *skip_list_find(skip_list_t *t, void *key_item);

/* skip_list_find_min() - Returns a pointer to the minimum item in the skip
 * list pointed to by t.  If there are no items in the list a NULL pointer is
 * returned.
 */
void *skip_list_find_min(skip_list_t *t);

/* skip_list_delete() - Delete the first item found in the skip list with
 * the same key as the item pointed to by `key_item'.  Returns a pointer to the
 * deleted item, and NULL if no item was found.
 */
void *skip_list_delete(skip_list_t *t, void *item);

/* skip_list_delete_min() - Deletes the item with the smallest key from the
 * skip list pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the list.
 */
void *skip_list_delete_min(skip_list_t *t);

#endif
