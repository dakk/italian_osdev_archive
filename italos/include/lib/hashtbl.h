/*** File hashtbl.h - Hash Table ***/
/*
 *   Shane Saunders
 */
#ifndef HASHTBL_H
#define HASHTBL_H

/*** Structure Type Definitions. ***/

/* Structure type definition for items in the hash table:
 *     item - a pointer to the item.
 *     next - a pointer to the next item in the buckets linked list.
 */
typedef struct hash_tbl_item {
    void *item;
    struct hash_tbl_item *next;
} hash_tbl_item_t;

/* Structure type definition for the hash table:
 *     n_buckets - the number of buckets in the hash table.
 *     buckets - an array of pointers to head items in the linked lists of
 *               hash table buckets.
 *     hash_fn - a pointer to the function for hashing an items key.
 *     compare_fn - a pointer to the function for comparing items.
 */
typedef struct hash_tbl {
    int n_buckets;
    hash_tbl_item_t **buckets;
    int (*hash_fn)(const void *item, int n);
    int (*compare_fn)(const void *item1, const void *item2);
} hash_tbl_t;



/* hash_tbl_alloc() - Returns a pointer to a hash table with n_buckets buckets.
 * Pointers to the following functions are required:
 *     hash_fn - A hash function, which hashes the key of the item pointed to
 *               by item to a bucket number between 0 and n-1.
 *     comp_fn - A comparison function.  Should returns zero only if item1 and
 *               item2 have the same key.
 */
hash_tbl_t *hash_tbl_alloc(int n_buckets,
			   int (*hash_fn)(const void *item, int n),
			   int (*comp_fn)(const void *item1, const void *item2)
                           );

/* hash_tbl_free() - Frees up space used by the hash table pointed to by tbl.
 */
void hash_tbl_free(hash_tbl_t *tbl);
    
/* hash_tbl_insert() - Insert the item pointed to by `item' into the hash table
 * pointed to by tbl.  If an item with the same key already exists in the
 * table, a pointer to that item is returned.  If the item was inserted
 * successfully, NULL is returned.
 */
void *hash_tbl_insert(hash_tbl_t *tbl, void *item);

/* hash_tbl_find() - Returns a pointer to an item with the same key as `item'
 * in the hash table pointed to by tbl.  If an item with the same key already
 * exists in the table, a pointer to that item is returned.  If no matching
 * item is found, NULL is returned.
 */
void *hash_tbl_find(hash_tbl_t *tbl, void *item);

/* hash_tbl_delete() - Deletes an item with the same key as `item' from the
 * hash table pointed to by tbl.  Returns a pointer to the item that was
 * deleted from the hash table, or NULL if no item was deleted.
 */
void *hash_tbl_delete(hash_tbl_t *tbl, void *item);

#endif
