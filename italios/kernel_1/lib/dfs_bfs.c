/*** File:  dfs_bfs.c - Depth First Search and Breadth First Search ***/
/*
 *   Shane Saunders
 */
#include <stdlib.h>
#include <stdio.h>
#include <dfs_bfs.h>
#include <dgraph.h>



#define UNDEFINED -1


/* Prototypes of functions only visible within this file. */
void dfs_recursive(int v);


typedef struct bfs_queue_item {
	int vertex_no;
	struct bfs_queue_item *next;
} bfs_queue_item_t;


dfs_bfs_result_t *dfs_result;
dgraph_t *current_graph;


/* dfs() - Performs a depth first search on the graph pointed to by g, and
 * returns an array of edges with a length equal to the number of vertices in
 * g.  The search begins at vertex number v.
 */
dfs_bfs_result_t *dfs(dgraph_t * g, int v)
{
	int i, n;
	int *vertices, *parents, *visit_nos;

	n = g->n;

	/* Allocate space for arrays to represent the search result. */
	dfs_result = malloc(sizeof(dfs_bfs_result_t));
	dfs_result->vertices = malloc(n * sizeof(int));
	dfs_result->parents = malloc(n * sizeof(int));
	dfs_result->visit_nos = malloc(n * sizeof(int));

	/* Initialise all array entries to UNDEFINED. */
	vertices = dfs_result->vertices;
	parents = dfs_result->parents;
	visit_nos = dfs_result->visit_nos;
	for (i = 0; i < n; i++) {
		vertices[i] = parents[i] = visit_nos[i] = UNDEFINED;
	}
	dfs_result->size = n;

	/* We start with no vertices in the result. */
	dfs_result->n = 0;

	/* Proceed with a recursive depth first search.  Note that the
	 * dfs_recursive() function accesses the current graph through the
	 * global variable current_graph.
	 */
	current_graph = g;
	dfs_recursive(v);

	return dfs_result;
}


/* dfs_recursive() - Recursively performs depth first search from vertex v in
 * the graph.
 */
void dfs_recursive(int v)
{
	dgraph_edge_t *edge_ptr;
	int w;

	/* Add vertex v to the result, and increase the counter for
	 * the number of vertices in the result.
	 */
	dfs_result->vertices[dfs_result->n] = v;
	dfs_result->visit_nos[v] = dfs_result->n++;

	/* Perform DFS recursively from each w in OUT(v) that is still
	 * unvisited.
	 */
	edge_ptr = current_graph->vertices[v].first_edge;
	while (edge_ptr) {
		w = edge_ptr->vertex_no;
		if (dfs_result->visit_nos[w] == UNDEFINED) {

			/* Note we update the parents[] array before the recursive call
			 * which updates the corresponding entry in the vertices[] array.
			 */
			dfs_result->parents[dfs_result->n] = v;
			dfs_recursive(w);
		}
		edge_ptr = edge_ptr->next;
	}
}



/* bfs() - performs a breadth first search on the graph pointed to by g, and
 * returns an array of edges with a length equal to the number of vertices in
 * g.  The search begins at vertex v.
 */
dfs_bfs_result_t *bfs(dgraph_t * g, int v)
{
	dfs_bfs_result_t *result;
	bfs_queue_item_t *q_head, *q_tail, *q_item;
	dgraph_edge_t *edge_ptr;
	int *vertices, *parents, *visit_nos;
	int i, c, w, n;

	n = g->n;

	/* Allocate space for arrays to represent the search result. */
	result = malloc(sizeof(dfs_bfs_result_t));
	result->vertices = malloc(n * sizeof(int));
	result->parents = malloc(n * sizeof(int));
	result->visit_nos = malloc(n * sizeof(int));

	/* Initialise all array entries to UNDEFINED. */
	vertices = result->vertices;
	parents = result->parents;
	visit_nos = result->visit_nos;
	for (i = 0; i < n; i++) {
		vertices[i] = parents[i] = visit_nos[i] = UNDEFINED;
	}
	result->size = n;

	/* Begin with the starting vertex on the queue. */
	q_item = malloc(sizeof(bfs_queue_item_t));
	q_item->vertex_no = v;
	q_item->next = NULL;
	q_head = q_tail = q_item;
	vertices[0] = v;
	visit_nos[v] = 0;
	c = 1;			/* vertex count */

	do {
		v = q_head->vertex_no;

		/* Add to the queue each w in OUT(v) that is still unvisited. */
		edge_ptr = g->vertices[v].first_edge;
		while (edge_ptr) {
			w = edge_ptr->vertex_no;
			if (visit_nos[w] == UNDEFINED) {
				/* Add w to the queue. */
				q_item = malloc(sizeof(bfs_queue_item_t));
				q_item->vertex_no = w;
				q_item->next = NULL;
				q_tail = q_tail->next = q_item;

				parents[c] = v;
				vertices[c] = w;
				visit_nos[w] = c++;
			}
			edge_ptr = edge_ptr->next;
		}

		/* Replace the head of the queue with the next item along. */
		q_item = q_head;
		q_head = q_head->next;
		free(q_item);

	} while (q_head);


	result->n = c;

	return result;
}



/* dfs_bfs_result_free() - Frees space used by a dfs/bfs search result
 * structure.
 */
void dfs_bfs_result_free(dfs_bfs_result_t * r)
{
	free(r->vertices);
	free(r->parents);
	free(r->visit_nos);
	free(r);
}


/* dfs_bfs_result_print() - Displays the result of a depth first or breadth
 * first search.
 */
void dfs_bfs_result_print(dfs_bfs_result_t * r)
{
/*    int i, n;

    n = r->size;
    
    printf("parents   =");
    for(i = 0; i < n; i++) {
        if(r->parents[i] == UNDEFINED) {
            printf("   -");
	}
	else {
            printf(" %3d", r->parents[i]);
	}
    }
    putchar('\n');

    printf("vertices  =");
    for(i = 0; i < n; i++) {
        if(r->vertices[i] == UNDEFINED) {
            printf("   -");
	}
	else {
            printf(" %3d", r->vertices[i]);
	}
    }
    putchar('\n');

    printf("visit_nos =");
    for(i = 0; i < n; i++) {
        if(r->visit_nos[i] == UNDEFINED) {
            printf("   -");
	}
	else {
            printf(" %3d", r->visit_nos[i]);
	}
    }
    putchar('\n');*/
}
