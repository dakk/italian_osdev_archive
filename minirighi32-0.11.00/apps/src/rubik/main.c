/*
 ***************************************************************************
 *      Rubik                                                              *
 *                                                                         *
 *      - A Rubik's cube heuristic solver.                                 *
 *                                                                         *
 *      Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>                 *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IDAstar.h"

//! Logo iniziale :)
#define WINDOW_TITLE	"\n<--  R u B i K' s   C u B e   S o L v E r  -->\n"

//! Numero totale di mosse effettuate.
int moves = 0;

//! Contatore per il numero totale di nodi espansi.
volatile int count = 0;

//! Seleziona l'euristica da utilizzare.
extern int heuristic;

//! \brief
//!	Mischia il cubo effettuando casualmente un determinato
//!	numero di mosse e restituisce l'ultima mossa fatta.
void scramble(int scramble_moves)
{
	move_t ret_mov;
	int i;

	printf("Scrambling cube with %u move%c!\n",
		scramble_moves, (scramble_moves!=1) ? 's' : ' '
	);
	for( i=0; i<scramble_moves; i++ )
	{
		ret_mov.axis = ( rand() % 3 );
		ret_mov.block = ( rand() % 2 ) ? 0 : 2;
		ret_mov.dir = ( rand() % 2 ) ? 1 : -1;
		// Effettua la mossa.
		rotate( ret_mov.axis, ret_mov.block, ret_mov.dir );
	}
}

//! Prova a risolvere il cubo utilizzando il metodo di ricerca IDA*.
void solve_using_IDAstar()
{
	MoveList_t *head=NULL, *list=NULL;

	// Azzera il numero di mosse.
	moves = count = 0;

	printf( "Resolving with IDA* (heuristic #%u)...\n", heuristic );

	// Risolvi.
	IDAstar_solver( &head );

	// Cubo risolto!!! Stampa il risultato.
	printf( "Cube solved ( %u node expanded )\n", count );
	printf( "\nStarting configuration:\n");
	printfaces();
	printf( "Press a key to continue...\n" );
	(void)getchar();
	if( head != NULL )
	{
		// Stampa la lista delle mosse.
		list = head;
		for(;;)
		{
			printf( "\nmove%3i ( axis=%i block=%i dir=%3i F=%i )\n",
				++moves,
				list->move.axis, list->move.block, list->move.dir,
				list->move.h );

			rotate( list->move.axis, list->move.block, list->move.dir );
			printfaces();
			list = list->next;
			if( list==NULL )
				break;
			printf( "Press a key to continue...\n" );
			(void)getchar();
		}

		// Libera risorse.
		list = head;
		while( head!=NULL )
		{
			head = head->next;
			free( head );
		}
	}
	printf( "Solved! %u move%c done.\n", moves, (moves==1) ? ' ' : 's' );
}

//! Programma principale.
int main(int argc, char *const *argv)
{
	// Prende in esame i valori passati da linea di comando.
	if( argc>=2 )
	{
		if( strcmp(argv[1], "--help")==0 )
		{
			printf( "Usage: %s <scramble_moves> [heuristic]\n\n",
				argv[0] );
			printf( "The value [heuristic] can be:\n"
				"\t- 0 = do not use heuristics!\n"
				"\t- 1 = BEST: not admissible heuristic, but faster than others.\n"
				"\t- 2 = DISTANCE: admissible, evaluate cube distances.\n"
				"\t- 3 = COLORS: admissible, evaluate cube colors.\n"
				"\t- 4 = MAX( COLORS, DISTANCES )\n"
				"\t- 5 = MAX( BEST, COLORS, DISTANCES ) not admissible!\n"
				"\nDefault value for [heuristic] = %u\n", heuristic );
			exit( EXIT_SUCCESS );
		}
		if( argc>2 )
		{
			heuristic = atoi( argv[2] );
			if( heuristic<0 || heuristic>5 )
			{
				printf( "%s: unknown value for heuristic!\n", argv[0] );
				printf( "Type %s --help for a detailed help.\n", argv[0] );
				exit( EXIT_FAILURE );
			}
		}
	}
	else
	{
		printf( "Usage: %s <scramble_moves> [heuristic]\n", argv[0] );
		printf( "Type %s --help for a detailed help.\n", argv[0] );
		exit( EXIT_SUCCESS );
	}

	// Initialize the random seed with the system ticks value.
	srand( times(NULL) );

	// Help.
	printf( "%s\n", WINDOW_TITLE );
	(void)scramble( atoi(argv[1]) );
	solve_using_IDAstar();

	// Fine! :)
	printf( "Bye!\n" );
	return( EXIT_SUCCESS );
}
