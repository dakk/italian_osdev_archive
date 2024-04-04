/*** File:  sc.c - Tarjan's Strongly Connected Components Algorithm ***/
/*
 *   Shane Saunders
 */
#include <stdlib.h>
#include <stdio.h>
#include <sc.h>
#include <dgraph.h>



#define UNDEFINED -1


/* Prototypes of functions only visible within this file. */
void sc_recursive(int v);

/* Pointers to the current graphs vertices and result structure arrays,
 * accessed by sc_recursive().
 */
dgraph_vertex_t *vertices;
int *sc_vertices, *sets_s, *sets_f;

/* Stack used for generating SC components.  The sp[v] array gives the position
 * of vertex v in sc_stack[].  The top of stack is specified by tos, which
 * indexes the next unused entry in sc_stack.
 */
int *sc_stack, *sp, tos;

/* Visit numbers and low link numbers used when determining SC components. */
int *visit_nos, *low_link_nos;

/* unvisited[] contains all vertices yet to be visited.  p[v] gives the
 * position of vertex v in unvisited[].  We keep track of unvisited vertices
 * in case part of the graph is not reachable from the starting vertex.
 */
int *unvisited, *p, n_unvisited;

/* Counters for
 *  - the number of visited vertices.
 *  - the number of vertices written to the vertices[] array i9n the
 *    SC component result.
 *  - the number of sets of vertices in the SC component result.
 */
int n_visited, n_written, n_sets;




/* sc() - Generates SC components using Tarjan's algorithm.
 * The result is returned as a pointer to a sc_result_t structure.  The SC
 * components are stored as two arrays:
 *   - sets_s[i] gives the starting position in the vertices[] array of SC
 *     component i.
 *   - sets_f[i] gives the finishing position in the vertices[] array of SC
 *     component i.
 *   - vertices[] is used for storing the vertex numbers of vertices in the
 *     SC components.
 * For example if there are three SC components the vertices in each are stored
 * as follows:
 *   - SC0:  vertices[sets_s[0]] ... vertices[sets_f[0]].
 *   - SC1:  vertices[sets_s[1]] ... vertices[sets_f[1]].
 *   - SC2:  vertices[sets_s[2]] ... vertices[sets_f[2]].
 * Note that array entries sets[3] onwards are set to UNDEFINED.
 */
sc_result_t *sc(dgraph_t * g, int v)
{
	int i, n;
	sc_result_t *result;

	n = g->n;
	vertices = g->vertices;	/* accessed by sc_recursive() */

	/* Allocate space for arrays to represent the search result. */
	result = malloc(sizeof(sc_result_t));
	sc_vertices = result->vertices = malloc(n * sizeof(int));
	sets_s = result->sets_s = malloc(n * sizeof(int));
	sets_f = result->sets_f = malloc(n * sizeof(int));

	/* Allocate space for arrays used while generating the result. */
	sc_stack = malloc(n * sizeof(int));
	sp = malloc(n * sizeof(int));
	visit_nos = malloc(n * sizeof(int));
	low_link_nos = malloc(n * sizeof(int));
	unvisited = malloc(n * sizeof(int));
	p = malloc(n * sizeof(int));

	/* Initialise necessary array entries to UNDEFINED.
	 *  - sets_s[] and sets_f[] array entries are UNDEFINED, until data is
	 *    written into them.
	 *  - visit_nos[] array entries are UNDEFINED, until a vertex has been
	 *    visited,
	 *  - sp[v] is UNDEFINED unless v is in the stack.
	 * Also, for keeping track of unvisited vertices set up the unvisited[] and
	 * p[] arrays.
	 */
	for (i = 0; i < n; i++) {
		sets_s[i] = sets_f[i] = visit_nos[i] = sp[i] = UNDEFINED;
		p[i] = unvisited[i] = i;
	}

	/* Array sizes in the result structure. */
	result->size = n;

	/* Tarjan's algorithm proceeds as a recursive depth first search.  Note
	 * that the sc_recursive() function accesses the current graph through the
	 * global variable `vertices'.  If parts of the graph were not reached
	 * sc_recursive() will be called again, until all vertices have been
	 * reached.
	 */
	tos = 0;
	n_written = n_visited = 0;
	n_unvisited = n;
	n_sets = 0;
	do {
		v = unvisited[0];
		sc_recursive(v);
	} while (n_unvisited);
	result->n_sets = n_sets;

	/* Free space taken up by arrays used while generating the result. */
	free(sc_stack);
	free(sp);
	free(visit_nos);
	free(low_link_nos);
	free(unvisited);
	free(p);

	return result;
}


/* sc_recursive() - Tarjan's SC component algorithm, proceeds as a depth
 * first search from vertex v in the graph.  Updates the result structure's
 * arrays through the global variables sc_vertices, sets_s, sets_f.
 */
void sc_recursive(int v)
{
	dgraph_edge_t *edge_ptr;
	int w, replace_v;

	/* Add vertex v to the result, and increase the counter for the number of
	 * vertices in the result.
	 */
	low_link_nos[v] = visit_nos[v] = n_visited;
	n_visited++;

	/* Remove the visited vertex from the array containing unvisited vertices.
	 */
	n_unvisited--;
	replace_v = unvisited[p[v]] = unvisited[n_unvisited];
	p[replace_v] = p[v];

	/* Place v on the stack. */
	sc_stack[tos] = v;
	sp[v] = tos;
	tos++;

	/* Note the algorithm is like a recursive DFS from each w in OUT(v) that is
	 * still unvisited.
	 */
	edge_ptr = vertices[v].first_edge;
	while (edge_ptr) {
		w = edge_ptr->vertex_no;

		if (visit_nos[w] == UNDEFINED) {
			sc_recursive(w);

			/* Update low_link no. */
			if (low_link_nos[w] < low_link_nos[v])
				low_link_nos[v] = low_link_nos[w];
		} else if (visit_nos[w] < visit_nos[v] && sp[w] != UNDEFINED) {

			/* Update low_link no. */
			if (visit_nos[w] < low_link_nos[v])
				low_link_nos[v] = visit_nos[w];
		}

		edge_ptr = edge_ptr->next;
	}

	/* If all vertices in v's SC component have been found. */
	if (low_link_nos[v] == visit_nos[v]) {

		sets_s[n_sets] = n_written;

		/* The SC component vertices are stored from the top of the stack, down
		 * to v.  Remove these vertices, and write them to the result
		 * structure.
		 */
		do {
			tos--;
			replace_v = sc_vertices[n_written] = sc_stack[tos];
			sp[replace_v] = UNDEFINED;
			n_written++;
		} while (replace_v != v);

		sets_f[n_sets] = n_written - 1;
		n_sets++;
	}
}


/* sc_result_free() - Frees space used by a SC component result structure.
 */
void sc_result_free(sc_result_t * r)
{
	free(r->vertices);
	free(r->sets_s);
	free(r->sets_f);
	free(r);
}


/* sc_result_print() - Displays the SC components stored in an SC component
 * result structure.
 */
void sc_result_print(sc_result_t * r)
{
/*    int i, j, n_sets;

    n_sets = r->n_sets;
    
    printf("%d SC components:\n\n", n_sets);
    for(i = 0; i < n_sets; i++) {
	printf("SC%d =", i);
	for(j = r->sets_s[i]; j <= r->sets_f[i]; j++) {
            printf(" %3d", r->vertices[j]);
	}
	putchar('\n');
    }*/
}
