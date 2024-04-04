/*** File tree23.c - 2-3 Tree Implementation. ***/
/*
 *   Shane Saunders
 */
#include <stdlib.h>
#include <tree23.h>


/* tree23_alloc() - Allocates space for a 2-3 tree and returns a pointer to
 * it.  The function compar compares they keys of two items, and returns a
 * negative, zero, or positive integer depending on whether the first item is
 * less than, equal to, or greater than the second.
 */
tree23_t *tree23_alloc(int (*compar) (const void *, const void *))
{
	tree23_t *t;
	tree23_node_t *r;

	t = malloc(sizeof(tree23_t));
	t->n = 0;
	t->min_item = NULL;
	t->compar = compar;
	t->stack = malloc(TREE23_STACK_SIZE * sizeof(tree23_node_t *));
	t->path_info = malloc(TREE23_STACK_SIZE * sizeof(signed char));
	r = t->root = malloc(sizeof(tree23_node_t));
	r->key_item1 = r->key_item2 = NULL;
	r->link_kind = LEAF_LINK;
	r->left.item = r->middle.item = r->right.item = NULL;

	return t;
}


/* tree23_free() - Frees space used by the 2-3 tree pointed to by t. */
void tree23_free(tree23_t * t)
{
	int tos;
	tree23_node_t *p, **stack;

	/* In order to free all nodes in the tree a depth first search is performed
	 * This is implemented using a stack.
	 */

	stack = malloc(2 * TREE23_STACK_SIZE * sizeof(tree23_node_t *));
	stack[0] = t->root;
	tos = 1;

	while (tos) {
		p = stack[--tos];
		if (p->link_kind == INTERNAL_LINK) {
			stack[tos++] = p->left.node;
			stack[tos++] = p->middle.node;
			if (p->right.node)
				stack[tos++] = p->right.node;
		}
		free(p);
	}

	free(stack);

	free(t->stack);
	free(t->path_info);
	free(t);
}


/* tree23_insert() - Inserts an item into the 2-3 tree pointed to by t,
 * according the the value its key.  The key of an item in the 2-3 tree must
 * be unique among items in the tree.  If an item with the same key already
 * exists in the tree, a pointer to that item is returned.  Otherwise, NULL is
 * returned, indicating insertion was successful.
 */
void *tree23_insert(tree23_t * t, void *item)
{
	int (*compar) (const void *, const void *);
	int cmp_result;
	void *key_item1, *key_item2, *temp_item, *x_min, *return_item;
	tree23_node_t *new_node, *x, *p;
	tree23_node_t **stack;
	int tos;
	signed char *path_info;


	p = t->root;
	compar = t->compar;

	/* Special case: only zero or one items in the tree already. */
	if (t->n <= 1) {
		if (t->n == 0) {	/* 0 items --> 1 item */
			t->min_item = p->left.item = item;
		} else {	/* 1 item --> 2 items */
			cmp_result = compar(item, p->left.item);

			/* Check that an item with the same key does not already exist. */
			if (cmp_result == 0)
				return p->left.item;
			/* else */

			/* Determine insertion position. */
			if (cmp_result > 0) {	/* Insert as middle child. */
				p->key_item1 = p->middle.item = item;
			} else {	/* Insert as left child. */
				p->key_item1 = p->middle.item = p->left.item;
				t->min_item = p->left.item = item;
			}
		}

		t->n++;
		return NULL;	/* Insertion successful. */
	}

	/* Stacks for storing the sequence of nodes traversed when
	 * locating the insertion position.
	 */
	stack = t->stack;
	path_info = t->path_info;
	tos = 0;

	/* Search the tree to locate the insertion position. */
	while (p->link_kind != LEAF_LINK) {
		stack[tos] = p;
		if (p->key_item2 && compar(item, p->key_item2) >= 0) {
			p = p->right.node;
			path_info[tos] = 1;
		} else if (compar(item, p->key_item1) >= 0) {
			p = p->middle.node;
			path_info[tos] = 0;
		} else {
			p = p->left.node;
			path_info[tos] = -1;
		}
		tos++;
	}

	key_item1 = p->key_item1;
	key_item2 = p->key_item2;

	/* Insert at the leaf items of node p.   Note that key_item1 is the same as
	 * p->middle.item and key_item2 is the same as p->right.item.
	 */
	if (key_item2 && (cmp_result = compar(item, key_item2)) >= 0) {
		/* Insert beside right branch. */

		/* Check if the same key already exists. */
		if (cmp_result == 0) {
			return key_item2;	/* Insertion failed. */
		}
		/* else */

		/* Create a new node.  Node p's right child becomes the left child
		 * of the new node.  The inserted item is the middle child of the
		 * new node.
		 */
		new_node = malloc(sizeof(tree23_node_t));
		new_node->link_kind = LEAF_LINK;
		new_node->left.item = p->right.item;
		new_node->key_item1 = new_node->middle.item = item;
		new_node->key_item2 = new_node->right.item = NULL;
		p->key_item2 = p->right.item = NULL;

		/* Insertion for new_node will continue higher up in the tree. */
	} else if ((cmp_result = compar(item, key_item1)) >= 0) {
		/* Insert beside the middle branch. */

		/* Check if the same key already exists. */
		if (cmp_result == 0) {
			return key_item1;	/* Insertion failed. */
		}
		/* else */

		/* Insertion depends on the number of children node p currently has. */
		if (key_item2) {	/* Node p has three children. */

			/* Create a new node.  The inserted item is the left child of
			 * the new node.  Node p's right child becomes the middle
			 * child of the new node.
			 */
			new_node = malloc(sizeof(tree23_node_t));
			new_node->link_kind = LEAF_LINK;
			new_node->left.item = item;
			new_node->key_item1 = new_node->middle.item =
				p->right.item;
			new_node->key_item2 = new_node->right.item = NULL;
			p->key_item2 = p->right.item = NULL;

			/* Insertion for new_node will continue higher up in the tree. */
		} else {	/* Node p has two children. */

			/* The item is inserted as the right child of node p. */
			p->key_item2 = p->right.item = item;

			/* No need to insert higher up. */
			t->n++;
			return NULL;	/* Insertion successful. */
		}
	} else {
		/* Insert beside the left branch. */

		/* Account for the special case, where the item being inserted is
		 * smaller than any other item in the tree.
		 */
		if ((cmp_result = compar(item, p->left.item)) <= 0) {

			/* Check if the same key already exists in the tree. */
			if (cmp_result == 0) {
				return p->left.item;	/* Insertion failed. */
			}
			/* else */

			/* The item being inserted is smaller than any other item in the
			 * tree.  Treat p's left child as the item begin inserted.  This
			 * done by replacing p's left child with the item being inserted.
			 */
			temp_item = item;
			item = p->left.item;
			t->min_item = p->left.item = temp_item;
		}

		/* Insertion depends on the number of children node p currently has. */
		if (key_item2) {	/* Node p has three children. */

			/* Create a new node.  Node p's middle child becomes the left
			 * child of the new node.  Node p's right child becomes the middle
			 * child of the new node.  The item being inserted becomes the
			 * middle child of node p.
			 */
			new_node = malloc(sizeof(tree23_node_t));
			new_node->link_kind = LEAF_LINK;
			new_node->left.item = p->middle.item;
			new_node->key_item1 = new_node->middle.item =
				p->right.item;
			new_node->key_item2 = new_node->right.item = NULL;
			p->key_item1 = p->middle.item = item;
			p->key_item2 = p->right.item = NULL;

			/* Insertion for new_node will continue higher up in the tree. */
		} else {	/* Node p has two children. */

			/* The middle child of node p becomes node p's right child, and
			 * the item being inserted becomes the middle child.
			 */
			p->key_item2 = p->right.item = p->middle.item;
			p->key_item1 = p->middle.item = item;

			/* No need to insert higher up. */
			t->n++;
			return NULL;	/* Insertion successful. */
		}
	}

	return_item = NULL;	/* Insertion successful. */
	t->n++;

	/* x points to the node being inserted into the tree.  x_min keeps track of
	 * the minimum item in the subtree rooted at the node x.
	 */
	x = new_node;
	x_min = new_node->left.item;

	/* Insertion of new nodes can keep propagating up one level in the tree.
	 * This stops when an insertion does not result in a new node at the next
	 * level up, or the root level (tos == 0) is reached.
	 */
	while (tos) {
		p = stack[--tos];	/* p is the parent node for x. */

		/* Determine the insertion position of x under p. */
		if (path_info[tos] > 0) {	/* Insert beside the right branch. */

			/* Create a new node.  Node p's right child becomes the left child
			 * of the new node.  Node x is the middle child of the new node.
			 */
			new_node = malloc(sizeof(tree23_node_t));
			new_node->link_kind = INTERNAL_LINK;
			new_node->left.node = p->right.node;
			new_node->middle.node = x;
			new_node->key_item1 = x_min;
			new_node->right.node = NULL;
			new_node->key_item2 = NULL;
			x_min = p->key_item2;
			p->right.node = NULL;
			p->key_item2 = NULL;

			/* Insertion for new_node will continue higher up in the tree. */
		} else if (path_info[tos] == 0) {	/* Insert beside the middle branch. */

			/* Insertion depends on the number of children node p currently
			 * has.
			 */
			if (p->key_item2) {	/* Node p has three children. */

				/* Create a new node.  Node x is the left child of the new
				 * node.  Node p's right child becomes the middle child of the
				 * new node.
				 */
				new_node = malloc(sizeof(tree23_node_t));
				new_node->link_kind = INTERNAL_LINK;
				new_node->left.node = x;	/* x_min does not change. */
				new_node->middle.node = p->right.node;
				new_node->key_item1 = p->key_item2;
				new_node->right.node = NULL;
				new_node->key_item2 = NULL;
				p->right.node = NULL;
				p->key_item2 = NULL;

				/* Insertion for new_node will continue higher up in the tree.
				 */
			} else {	/* Node p has two children. */

				/* Node x is inserted as the right child of node p. */
				p->right.node = x;
				p->key_item2 = x_min;

				/* No need to insert higher up. */
				return return_item;
			}
		} else {	/* Insert beside the left branch. */

			/* Insertion depends on the number of children node p currently
			 * has.
			 */
			if (p->key_item2) {	/* Node p has three children. */

				/* Create a new node.  Node p's middle child becomes the left
				 * child of the new node.  Node p's right child becomes the
				 * middle child of the new node.  Node x becomes the middle
				 * child of node p.
				 */
				new_node = malloc(sizeof(tree23_node_t));
				new_node->link_kind = INTERNAL_LINK;
				new_node->left.node = p->middle.node;
				new_node->middle.node = p->right.node;
				new_node->key_item1 = p->key_item2;
				new_node->right.node = NULL;
				new_node->key_item2 = NULL;
				p->middle.node = x;
				temp_item = p->key_item1;
				p->key_item1 = x_min;
				x_min = temp_item;
				p->right.node = NULL;
				p->key_item2 = NULL;

				/* Insertion for new_node will continue higher up in the tree.
				 */
			} else {	/* Node p has two children. */

				/* The middle child of node p becomes node p's right child, and
				 * node x becomes the middle child.
				 */
				p->right.node = p->middle.node;
				p->key_item2 = p->key_item1;
				p->middle.node = x;
				p->key_item1 = x_min;

				/* No need to insert higher up. */
				return return_item;
			}
		}

		x = new_node;
	}

	/* This point is only reached if the root node was split.  A new root node
	 * will be created, with the child nodes pointed to by p (old root node)
	 * and x (inserted node).
	 */
	new_node = malloc(sizeof(tree23_node_t));
	new_node->link_kind = INTERNAL_LINK;
	new_node->left.node = p;
	new_node->middle.node = x;
	new_node->key_item1 = x_min;
	new_node->right.node = NULL;
	new_node->key_item2 = NULL;
	t->root = new_node;


	return return_item;
}



/* tree23_find() - Find an item in the 2-3 tree with the same key as the
 * item pointed to by `key_item'.  Returns a pointer to the item found, or NULL
 * if no item was found.
 */
void *tree23_find(tree23_t * t, void *key_item)
{
	int (*compar) (const void *, const void *);
	int cmp_result;
	tree23_node_t *p;
	void *key_item1, *key_item2;

	p = t->root;
	compar = t->compar;

	/* First check for the special cases where the tree contains contains only
	 * zero or one nodes.
	 */
	if (t->n <= 1) {
		if (t->n && (compar(key_item, p->left.item) == 0))
			return p->left.item;
		/* else */

		return NULL;
	}

	/* Search the tree to locate the item with key key_item. */
	while (p->link_kind != LEAF_LINK) {
		if (p->key_item2 && compar(key_item, p->key_item2) >= 0) {
			p = p->right.node;
		} else if (compar(key_item, p->key_item1) >= 0) {
			p = p->middle.node;
		} else {
			p = p->left.node;
		}
	}

	key_item1 = p->key_item1;
	key_item2 = p->key_item2;

	/* Find a leaf item of node p.   Note key_item1 is the same as
	 * p->middle.item and key_item2 is the same as p->right.item.
	 */
	if (key_item2 && (cmp_result = compar(key_item, key_item2)) >= 0) {
		/* Item may be right child. */

		if (cmp_result) {
			/* Find failed - matching item does not exist in the tree. */
			return NULL;
		}
		/* else */
		return key_item2;	/* Item found. */
	} else if ((cmp_result = compar(key_item, key_item1)) >= 0) {
		/* Item may be middle child. */

		if (cmp_result) {
			/* Find failed - Matching item does not exist in the tree. */
			return NULL;
		}
		/* else */
		return key_item1;	/* Item found. */
	} else {
		/* Item may be left child. */

		if (compar(key_item, p->left.item)) {
			/* Find failed - matching item does not exist in the tree. */
			return NULL;
		}
		/* else */
		return p->left.item;	/* item found. */
	}
}



/* tree23_find_min() - Returns a pointer to the minimum item in the 2-3 tree
 * pointed to by t.  If there are no items in the tree a NULL pointer is
 * returned.
 */
void *tree23_find_min(tree23_t * t)
{
	return t->min_item;
}



/* tree23_delete() - Delete the item in the 2-3 tree with the same key as
 * the item pointed to by `key_item'.  Returns a pointer to the deleted item,
 * and NULL if no item was found.
 */
void *tree23_delete(tree23_t * t, void *key_item)
{
	int (*compar) (const void *, const void *);
	int cmp_result;
	void *key_item1, *key_item2, *return_item, *merge_item;
	void **min_key_ptr;
	tree23_node_t *p, *q, *parent, *merge_node;
	tree23_node_t **stack;
	int tos;
	signed char *path_info;


	p = t->root;
	compar = t->compar;

	/* Special cases: 0, 1, or 2 items in the tree. */
	if (t->n <= 2) {
		if (t->n <= 1) {
			if (t->n == 0) {
				return NULL;	/* Tree empty.  Delete failed. */
			}
			/* else: one item in the tree... */

			/* Check if the item is the left child. */
			if (compar(key_item, p->left.item) == 0) {
				return_item = p->left.item;	/* Item found. */
				t->min_item = p->left.item = NULL;
				t->n--;
				return return_item;
			}
			/* else */

			return NULL;	/* Item not found. */
		}
		/* else: two items in the tree... */

		/* Check if the item may be the middle child. */
		if ((cmp_result = compar(key_item, p->middle.item)) >= 0) {

			/* check if the item is the middle child. */
			if (cmp_result == 0) {
				return_item = p->middle.item;	/* Item found. */
				p->key_item1 = p->middle.item = NULL;
				t->n--;
				return return_item;
			}
			/* else */

			return NULL;	/* Item not found. */
		}

		/* Check if the item is the left child. */
		if (compar(key_item, p->left.item) == 0) {
			return_item = p->left.item;	/* Item found. */
			t->min_item = p->left.item = p->key_item1;
			p->key_item1 = p->middle.item = NULL;
			t->n--;
			return return_item;
		}
		/* else */

		return NULL;	/* Item not found. */
	}

	/* Allocate stacks for storing information about the path from the root
	 * to the node to be deleted.
	 */
	stack = t->stack;
	path_info = t->path_info;
	tos = 0;
	min_key_ptr = NULL;

	/* Search the tree to locate the node pointing to the leaf item to be
	 * deleted from the 2-3 tree.
	 */
	while (p->link_kind != LEAF_LINK) {
		stack[tos] = p;
		if (p->key_item2 && compar(key_item, p->key_item2) >= 0) {
			min_key_ptr = &p->key_item2;
			p = p->right.node;
			path_info[tos] = 1;
		} else if (compar(key_item, p->key_item1) >= 0) {
			min_key_ptr = &p->key_item1;
			p = p->middle.node;
			path_info[tos] = 0;
		} else {
			p = p->left.node;
			path_info[tos] = -1;
		}
		tos++;
	}

	key_item1 = p->key_item1;
	key_item2 = p->key_item2;

	/* Delete the appropriate leaf item of node p.  Note that key_item1 is the
	 * same as p->middle.item and key_item2 is the same as p->right.item.
	 */
	if (key_item2 && (cmp_result = compar(key_item, key_item2)) >= 0) {
		/* Item may be right child. */

		/* Check whether the item to be deleted was found. */
		if (cmp_result) {
			/* Item not found. */
			return_item = NULL;
		} else {
			/* Item found. */
			return_item = key_item2;
			t->n--;
			p->key_item2 = p->right.item = NULL;
		}

		/* No need for merge since node p still has two items. */
		return return_item;
	} else if ((cmp_result = compar(key_item, key_item1)) >= 0) {
		/* Item may be middle child. */

		/* Check whether the item to be deleted was found. */
		if (cmp_result) {
			/* Item not found. */
			return NULL;
		}
		/* else */

		return_item = key_item1;	/* Item found. */
		t->n--;

		/* If node p has three children, two are left after the delete, and no
		 * further rearrangement is needed.
		 */
		if (key_item2) {
			p->key_item1 = p->middle.item = key_item2;
			p->key_item2 = p->right.item = NULL;
			return return_item;
		}
		/* else */

		/* Node p has only its left child remaining.  The remaining child will
		 * be merged with a child from a sibling of node p.
		 */
		merge_item = p->left.item;
	} else {
		/* Item may be left child. */

		if (compar(key_item, p->left.item)) {
			/* Delete failed - matching item does not exist in the tree. */
			return NULL;
		}

		return_item = p->left.item;	/* item found. */
		t->n--;

		/* Check if a key in the tree changes after deletion. */
		if (min_key_ptr) {
			*min_key_ptr = p->middle.item;	/* Change key. */
		} else {
			t->min_item = key_item1;	/* Minimum node in the tree changes. */
		}

		/* If node p has three children, two are left after the delete, and no
		 * further rearrangement is needed.
		 */
		if (key_item2) {
			p->left.item = key_item1;
			p->key_item1 = p->middle.item = key_item2;
			p->key_item2 = p->right.item = NULL;
			return return_item;
		}
		/* else */

		/* Node p has only its middle child remaining.  The remaining child
		 * will be merged with a child from a sibling of node p.
		 */
		merge_item = p->middle.item;
	}


	/* If the function has not exited by this point, then the remaining child
	 * item of node p is to be merged with a child from a sibling of node p.
	 * Node p is not the root node, since this falls under the special case
	 * delete (handled at the start of this function).
	 */

	parent = stack[--tos];	/* Node p's parent. */

	/* The following code performs the leaf level merging of merge_item.  Note
	 * that unless node p was a left child, it always has a sibling to its
	 * left.
	 */
	if (path_info[tos] > 0) {
		/* p was the right child. */
		q = parent->middle.node;	/* Sibling to the left of node p. */

		/* Merging depends on how many children node q currently has. */
		if (q->key_item2) {	/* Node q has three children. */

			/* Keep node p by assigning it the right child of q as the sibling
			 * of merge_item.
			 */
			parent->key_item2 = p->left.item = q->key_item2;
			p->key_item1 = p->middle.item = merge_item;
			q->key_item2 = q->right.item = NULL;

			return return_item;	/* Node p is not deleted. */
		} else {	/* Node q has two children. */

			/* Make merge_item a child of node q, and delete p. */
			q->key_item2 = q->right.item = merge_item;
			parent->right.node = NULL;
			parent->key_item2 = NULL;
			free(p);

			return return_item;	/* The parent still has two children. */
		}
	} else if (path_info[tos] == 0) {
		/* p was the middle child. */
		q = parent->left.node;	/* Sibling to the left of node p. */

		/* Merging depends on how many children node q currently has. */
		if (q->key_item2) {	/* Node q has three children. */

			/* Keep node p by assigning it the right child of q as the sibling
			 * of merge_item.
			 */
			parent->key_item1 = p->left.item = q->key_item2;
			p->key_item1 = p->middle.item = merge_item;
			q->key_item2 = q->right.item = NULL;

			return return_item;	/* Node p is not deleted. */
		} else {	/* Node q has two children. */

			/* Make merge_item a child of node q, and delete p. */
			q->key_item2 = q->right.item = merge_item;
			free(p);

			/* If the parent of p and q had three children, then two will be
			 * left after the merge, and merging will not be needed at the next
			 * level up.
			 */
			if (parent->key_item2) {
				parent->middle.node = parent->right.node;
				parent->key_item1 = parent->key_item2;
				parent->right.node = NULL;
				parent->key_item2 = NULL;
				return return_item;
			}
			/* else */

			/* The parent only has child q remaining.  The remaining child
			 * will be merged with a child from a sibling of the parent.
			 */
			merge_node = q;
			p = parent;
		}
	} else {
		/* p was the left child. */
		q = parent->middle.node;	/* Sibling to the right of node p. */

		/* Merging depends on how many children node q currently has. */
		if (q->key_item2) {	/* Node q has three children. */

			/* Keep node p by assigning it the left child of q as the sibling
			 * of merge_item.
			 */
			p->left.item = merge_item;
			p->key_item1 = p->middle.item = q->left.item;
			parent->key_item1 = q->left.item = q->key_item1;
			q->key_item1 = q->middle.item = q->key_item2;
			q->key_item2 = q->right.item = NULL;

			return return_item;
		} else {	/* Node q has two children. */

			/* Make merge_item a child of node q, and delete p.
			 */
			q->key_item2 = q->right.item = q->key_item1;
			q->key_item1 = q->middle.item = q->left.item;
			q->left.item = merge_item;
			free(p);

			/* If the parent of p and q had three children, then two will be
			 * left after the merge, and merging will not be needed at the next
			 * level up.
			 */
			if (parent->key_item2) {
				parent->left.node = q;
				parent->middle.node = parent->right.node;
				parent->key_item1 = parent->key_item2;
				parent->right.node = NULL;
				parent->key_item2 = NULL;
				return return_item;
			}
			/* else */

			/* The parent only has child q remaining.  The remaining child
			 * will be merged with a child from a sibling of the parent.
			 */
			merge_node = q;
			p = parent;
		}
	}

	/* Merging of nodes can keep propagating up one level in the tree.  This
	 * stops when the result of a merge does not require a merge at the next
	 * level up, or the root level (tos == 0) is reached.
	 */
	while (tos) {
		/* The following code merges node p's single child, merge_node, with
		 * a children from node p's sibling, q.
		 */
		parent = stack[--tos];	/* Node p's parent. */

		/* Merging depends on which child p is. */
		if (path_info[tos] > 0) {
			/* p was the right child. */
			q = parent->middle.node;	/* Sibling to the left of node p. */

			/* Merging depends on how many children node q currently has. */
			if (q->key_item2) {	/* Node q has three children. */

				/* Keep node p by assigning it the right child of q as the
				 * sibling of merge_node.
				 */
				p->left.node = q->right.node;
				p->middle.node = merge_node;
				p->key_item1 = parent->key_item2;	/* merge_min */
				parent->key_item2 = q->key_item2;
				q->right.node = NULL;
				q->key_item2 = NULL;

				return return_item;	/* Node p is not deleted. */
			} else {	/* Node q has two children. */

				/* Make merge_node a child of node q, and delete p. */
				q->right.node = merge_node;
				q->key_item2 = parent->key_item2;	/* merge_min */
				parent->right.node = NULL;
				parent->key_item2 = NULL;
				free(p);

				return return_item;	/* The parent still has two children. */
			}
		} else if (path_info[tos] == 0) {
			/* p was the middle child. */
			q = parent->left.node;	/* Sibling to the left of node p. */

			/* Merging depends on how many children node q currently has. */
			if (q->key_item2) {	/* Node q has three children. */

				/* Keep node p by assigning it the right child of q as the
				 * sibling of merge_node.
				 */
				p->left.node = q->right.node;
				p->middle.node = merge_node;
				p->key_item1 = parent->key_item1;	/* merge_min */
				parent->key_item1 = q->key_item2;
				q->right.node = NULL;
				q->key_item2 = NULL;

				return return_item;	/* p is not deleted. */
			} else {	/* Node q has two children. */

				/* Make merge_node a child of node q, and delete p. */
				q->right.node = merge_node;
				q->key_item2 = parent->key_item1;	/* merge_min */
				free(p);

				/* If the parent of p and q had three children, then two will
				 * be left after the merge, and merging will not be needed at
				 * the next level up.
				 */
				if (parent->key_item2) {
					parent->middle.node =
						parent->right.node;
					parent->key_item1 = parent->key_item2;
					parent->right.node = NULL;
					parent->key_item2 = NULL;
					return return_item;
				}
				/* else */

				/* The parent only has child q remaining.  The remaining child
				 * will be merged with a child from a sibling of the parent.
				 */
				merge_node = q;
				p = parent;
			}
		} else {
			/* p was the left child. */
			q = parent->middle.node;	/* Sibling to the right of node p. */

			/* Merging depends on how many children node q currently has. */
			if (q->key_item2) {	/* Node q has three children. */

				/* Keep node p by assigning it the left child of q as the
				 * sibling of merge_node.
				 */
				p->left.node = merge_node;
				p->middle.node = q->left.node;
				p->key_item1 = parent->key_item1;
				/* Equals minimum item in tree(q->left.node). */
				q->left.node = q->middle.node;
				parent->key_item1 = q->key_item1;
				q->middle.node = q->right.node;
				q->key_item1 = q->key_item2;
				q->right.node = NULL;
				q->key_item2 = NULL;
				return return_item;
			} else {	/* Node q has two children. */

				/* Make merge_node a child of node q, and delete p.
				 */
				q->right.node = q->middle.node;
				q->key_item2 = q->key_item1;
				q->middle.node = q->left.node;
				q->key_item1 = parent->key_item1;
				/* Equals minimum item in tree(q->left.node). */
				q->left.node = merge_node;
				free(p);

				/* If the parent of p and q had three children, then two will
				 * be left after the merge, and merging will not be needed at
				 * the next level up.
				 */
				if (parent->key_item2) {
					parent->left.node = q;
					parent->middle.node =
						parent->right.node;
					parent->key_item1 = parent->key_item2;
					parent->right.node = NULL;
					parent->key_item2 = NULL;
					return return_item;
				}
				/* else */

				/* The parent only has child q remaining.  The remaining child
				 * will be merged with a child from a sibling of the parent.
				 */
				merge_node = q;
				p = parent;
			}
		}
	}


	/* Remove the old root node, p, making node merge_node the new root node.
	 */
	free(p);
	t->root = merge_node;


	return return_item;
}


/* tree23_delete_min() - Deletes the item with the smallest key from the
 * binary search tree pointed to by t.  Returns a pointer to the deleted item.
 * Returns a NULL pointer if there are no items in the tree.
 */
void *tree23_delete_min(tree23_t * t)
{
	void *return_item, *merge_item;
	tree23_node_t *p, *q, *parent, *merge_node;
	tree23_node_t **stack;
	int tos;


	p = t->root;

	/* Special cases: 0, 1, or 2 items in the tree. */
	if (t->n <= 2) {
		if (t->n <= 1) {
			if (t->n == 0) {
				return NULL;	/* Tree empty.  Delete failed. */
			}
			/* else: one item in the tree... */

			return_item = p->left.item;	/* Item found. */
			t->min_item = p->left.item = NULL;
		} else {	/* Two items in the tree. */

			return_item = p->left.item;	/* Item found. */
			t->min_item = p->left.item = p->key_item1;
			p->key_item1 = p->middle.item = NULL;
		}
		t->n--;
		return return_item;
	}
	/* else */

	/* Allocate a stack to store nodes along the path to the minimum item. */
	stack = t->stack;
	tos = 0;

	/* Search the tree to locate the node which points to the minimum item. */
	while (p->link_kind != LEAF_LINK) {
		stack[tos] = p;
		p = p->left.node;
		tos++;
	}

	return_item = p->left.item;	/* Item found. */
	t->n--;
	t->min_item = p->key_item1;	/* Minimum item in the 2-3 tree changes. */

	/* If node p has three children, two are left after the delete, and no
	 * further rearrangement is needed.
	 */
	if (p->key_item2) {
		p->left.item = p->key_item1;
		p->key_item1 = p->middle.item = p->key_item2;
		p->key_item2 = p->right.item = NULL;
		return return_item;
	}
	/* else */

	/* Node p has only its middle child remaining.  The remaining child
	 * will be merged with a child from a sibling of node p.
	 */
	merge_item = p->middle.item;

	/* If the function has not exited by this point, then the remaining child
	 * item of node p is to be merged with a child from a sibling of node p.
	 * Node p is not the root node, since this falls under the special case
	 * delete (handled at the start of this function).
	 */

	parent = stack[--tos];	/* Node p's parent. */

	/* p was the left child. */
	q = parent->middle.node;	/* Sibling to the right of node p. */

	/* Merging depends on how many children node q currently has. */
	if (q->key_item2) {	/* Node q has three children. */
		/* Keep node p by assigning it the left child of q as the sibling
		 * of merge_item.
		 */
		p->left.item = merge_item;
		p->key_item1 = p->middle.item = q->left.item;
		parent->key_item1 = q->left.item = q->key_item1;
		q->key_item1 = q->middle.item = q->key_item2;
		q->key_item2 = q->right.item = NULL;

		return return_item;
	} else {		/* node q has two children... */

		/* Make merge_item a child of node q, and delete p.
		 */
		q->key_item2 = q->right.item = q->key_item1;
		q->key_item1 = q->middle.item = q->left.item;
		q->left.item = merge_item;
		free(p);

		/* If the parent of p and q had three children, then two will be left
		 * after the merge, and merging will not be needed at the next level
		 * up.
		 */
		if (parent->key_item2) {
			parent->left.node = q;
			parent->middle.node = parent->right.node;
			parent->key_item1 = parent->key_item2;
			parent->right.node = NULL;
			parent->key_item2 = NULL;

			merge_node = q;
			p = parent;

			return return_item;
		}
		/* else */

		/* The parent only has child q remaining.  The remaining child
		 * will be merged with a child from a sibling of the parent.
		 */
		merge_node = q;
		p = parent;
	}


	/* Merging of nodes can keep propagating up one level in the tree.  This
	 * stops when the result of a merge does not require a merge at the next
	 * level up, or the root level (tos == 0) is reached.
	 */
	while (tos) {
		parent = stack[--tos];

		/* p is always a left child, and always has a sibling to its right. */
		q = parent->middle.node;	/* Sibling to the right of node p. */

		/* Merging depends on how many children node q currently has. */
		if (q->key_item2) {	/* Node q has three children. */

			/* Keep node p by assigning it the left child of q as the
			 * sibling of merge_node.
			 */
			p->left.node = merge_node;
			p->middle.node = q->left.node;
			p->key_item1 = parent->key_item1;
			/* Equals minimum item in tree(q->left.node). */
			q->left.node = q->middle.node;
			parent->key_item1 = q->key_item1;
			q->middle.node = q->right.node;
			q->key_item1 = q->key_item2;
			q->right.node = NULL;
			q->key_item2 = NULL;
			return return_item;
		} else {	/* Node q has two children. */

			/* Make merge_node a child of node q, and delete p. */
			q->right.node = q->middle.node;
			q->key_item2 = q->key_item1;
			q->middle.node = q->left.node;
			q->key_item1 = parent->key_item1;
			/* Equals minimum item in tree(q->left.node). */
			q->left.node = merge_node;
			free(p);

			/* If the parent of p and q had three children, then two will
			 * be left after the merge, and merging will not be needed at the
			 * next level up.
			 */
			if (parent->key_item2) {
				parent->left.node = q;
				parent->middle.node = parent->right.node;
				parent->key_item1 = parent->key_item2;
				parent->right.node = NULL;
				parent->key_item2 = NULL;
				return return_item;
			}
			/* else */

			/* The parent only has child q remaining.  The remaining child
			 * will be merged with a child from a sibling of the parent.
			 */
			merge_node = q;
			p = parent;
		}
	}


	/* Remove the old root node, p, making node x the new root node. */
	free(p);
	t->root = merge_node;


	return return_item;
}


/*** Implement the universal dictionary structure type ***/

/*** 2-3 tree wrapper functions. ***/

void *_tree23_alloc(int (*compar) (const void *, const void *),
		    unsigned int (*getval) (const void *))
{
	return tree23_alloc(compar);
}

void _tree23_free(void *t)
{
	tree23_free((tree23_t *) t);
}

void *_tree23_insert(void *t, void *item)
{
	return tree23_insert((tree23_t *) t, item);
}

void *_tree23_delete(void *t, void *key_item)
{
	return tree23_delete((tree23_t *) t, key_item);
}

void *_tree23_delete_min(void *t)
{
	return tree23_delete_min((tree23_t *) t);
}

void *_tree23_find(void *t, void *key_item)
{
	return tree23_find((tree23_t *) t, key_item);
}

void *_tree23_find_min(void *t)
{
	return tree23_find_min((tree23_t *) t);
}

/* 2-3 tree info. */
const dict_info_t TREE23_info = {
	_tree23_alloc,
	_tree23_free,
	_tree23_insert,
	_tree23_delete,
	_tree23_delete_min,
	_tree23_find,
	_tree23_find_min
};
