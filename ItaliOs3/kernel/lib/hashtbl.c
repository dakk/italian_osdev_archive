/*** File hashtbl.c - Hash Table ***/
/*
 *   Shane Saunders
 */
#include <stdlib.h>
#include <hashtbl.h>


/* hash_tbl_alloc() - Returns a pointer to a hash table with n_buckets buckets.
 * Pointers to the following functions are required:
 *     hash_fn - A hash function, which hashes the key of the item pointed to
 *               by item to a bucket number between 0 and n-1.
 *     comp_fn - A comparison function.  Should returns zero only if item1 and
 *               item2 have the same key.
 */
hash_tbl_t *hash_tbl_alloc(int n_buckets,
			   int (*hash_fn) (const void *item, int n),
			   int (*comp_fn) (const void *item1,
					   const void *item2)
	)
{
	hash_tbl_t *tbl;

	tbl = malloc(sizeof(hash_tbl_t));
	tbl->buckets = malloc(n_buckets * sizeof(hash_tbl_item_t *));
	tbl->n_buckets = n_buckets;
	tbl->hash_fn = hash_fn;
	tbl->compare_fn = comp_fn;

	return tbl;
}


/* hash_tbl_free() - Frees up space used by the hash table pointed to by tbl.
 */
void hash_tbl_free(hash_tbl_t * tbl)
{
	int i, n;
	hash_tbl_item_t **buckets, *item_ptr, *free_item;

	/* First free space used by any of the items in the has table. */
	n = tbl->n_buckets;
	buckets = tbl->buckets;
	for (i = 0; i < n; i++) {
		if ((item_ptr = buckets[i])) {
			while (item_ptr) {
				free_item = item_ptr;
				item_ptr = item_ptr->next;
				free(free_item);
			}
		}
	}

	/* Now free the hash table and its buckets. */
	free(buckets);
	free(tbl);
}


/* hash_tbl_insert() - Insert the item pointed to by `item' into the hash table
 * pointed to by tbl.  If an item with the same key already exists in the
 * table, a pointer to that item is returned.  If the item was inserted
 * successfully, NULL is returned.
 */
void *hash_tbl_insert(hash_tbl_t * tbl, void *item)
{
	int pos;
	hash_tbl_item_t *new_item, *item_ptr, *next_item, **buckets;
	hash_tbl_item_t **update_ptr;

	buckets = tbl->buckets;

	/* Get the bucket position for the item being inserted. */
	pos = tbl->hash_fn(item, tbl->n_buckets);
	next_item = buckets[pos];
	if (next_item) {
		do {
			item_ptr = next_item;
			if (!tbl->compare_fn(item_ptr->item, item)) {
				/* An item with this key already exists. */
				return item_ptr->item;
			}
			next_item = item_ptr->next;
		} while (next_item);
		update_ptr = &item_ptr->next;
	} else {
		update_ptr = &buckets[pos];
	}

	/* Create a new hash table item for the item being inserted. */
	new_item = malloc(sizeof(hash_tbl_item_t));
	new_item->item = item;
	new_item->next = NULL;
	*update_ptr = new_item;

	return NULL;
}


/* hash_tbl_find() - Returns a pointer to an item with the same key as `item'
 * in the hash table pointed to by tbl.  If an item with the same key already
 * exists in the table, a pointer to that item is returned.  If no matching
 * item is found, NULL is returned.
 */
void *hash_tbl_find(hash_tbl_t * tbl, void *item)
{
	int pos;
	hash_tbl_item_t *item_ptr, *next_item, **buckets;

	buckets = tbl->buckets;

	/* Get the bucket position for the item being inserted. */
	pos = tbl->hash_fn(item, tbl->n_buckets);
	next_item = buckets[pos];
	if (next_item) {
		do {
			item_ptr = next_item;
			if (!tbl->compare_fn(item_ptr->item, item)) {
				/* An item with this key already exists. */
				return item_ptr->item;
			}
			next_item = item_ptr->next;
		} while (next_item);
	}

	return NULL;
}


/* hash_tbl_delete() - Deletes an item with the same key as `item' from the
 * hash table pointed to by tbl.  Returns a pointer to the item that was
 * deleted from the hash table, or NULL if no item was deleted.
 */
void *hash_tbl_delete(hash_tbl_t * tbl, void *item)
{
	int pos;
	hash_tbl_item_t *item_ptr, *next_item, *prev_item, **buckets;
	hash_tbl_item_t *return_item;

	buckets = tbl->buckets;

	/* Get the bucket position for the item being inserted. */
	pos = tbl->hash_fn(item, tbl->n_buckets);
	next_item = buckets[pos];
	if (next_item) {
		item_ptr = NULL;
		do {
			prev_item = item_ptr;
			item_ptr = next_item;
			if (!tbl->compare_fn(item_ptr->item, item)) {
				/* An item with this key already exists. */
				return_item = item_ptr->item;
				free(item_ptr);
				if (prev_item) {
					prev_item->next = next_item;
				} else {
					buckets[pos] = next_item;
				}
				return return_item;
			}
			next_item = item_ptr->next;
		} while (next_item);
	}

	return NULL;
}
