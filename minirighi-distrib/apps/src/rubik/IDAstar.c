/*
 *      IDAstar.cpp - Risolutore con procedimento IDA*.
 *
 *      Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>
 *
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "MatrixCube.h"
#include "Move.h"
#include "IDAstar.h"

extern int count;

//! Limite di costo corrente per espandere i nodi.
static int fstar=0;

//! Lista delle mosse per arrivare alla soluzione.
static MoveList_t *list=NULL, *head=NULL;

//! Funzione ricorsiva per la risoluzione col metodo IDA*.
/**
 * Algoritmo iterativo variante di A*. Ogni iterazione è una ricerca
 * in profondità; al contrario della classica ricerca a profondità
 * limitata viene utilizzato un costo limite f* al posto di una
 * profondità massima. Quindi ad ogni iterazione vengono espansi
 * tutti i nodi dentro la frontiera che hanno un costo corrente f()
 * minore del limite f*. Se la soluzione non viene trovata il
 * procedimento viene reiterato incrementando il valore limite f*
 * di 1 unità.
 * \note
 *	<b>Potatura dell'albero</b>:
 *	per come è implementato l'algoritmo IDA*, questo ci consente
 *	di effettuare delle particolari potature sulla base delle
 *	mosse precedentemente effettuate.
 *	\li <b>potatura #1 - </b>
 *	&Egrave; possibile eliminare il sottoalbero generato dalla
 *	mossa che riporta ogni nodo nel proprio stato padre. Sotto
 *	queste condizioni al primo passo ogni nodo viene espanso in
 *	18 figli (tali sono le mosse effettuabili in un determinato
 *	stato); per gli stati successivi una espansione genererà
 *	soltanto 17 figli.
 *	\li <b>potatura #2 - </b>
 *	Dato che IDA* è sostanzialmente un'analisi in profondità dei
 *	nodi aventi \f$ f()<=f^* \f$, è possibile grazie a meccanismi di
 *	ricorsione tenere traccia del numero di volte che si effettua
 *	la stessa mossa (questo è realizzabile passando come argomento
 *	il valore \p mcount). Se osserviamo bene il cubo possiamo notare
 *	che non ha senso effettuare per più di 2 volte la stessa mossa:
 *	tre mosse uguali coincidono con una rotazione di \f$270^\circ\f$;
 *	tale sequenza di mosse è equivalente ad effettuare una singola
 *	rotazione di \f$-90^\circ\f$ gradi.
 */
static __INLINE__ int IDAstar( move_t tabu, int depth, int mcount )
{
	move_t curr;

	count++;
	FOREACH_MOVE( curr )
	{
		// Potature sull'albero di ricerca.
		if( curr.axis==tabu.axis )
		{
			if( curr.block==tabu.block )
			{
				if( curr.dir==tabu.dir )
				{
					// Mossa uguale alla precedente.
					if( mcount==2 )
						continue;
					// Non autorizzare più di una
					// rotazione consecutiva di -90°.
					// Possiamo ricondurci allo stesso
					// stato effettuando 2 mosse consecutive
					// di +90°.
					if( curr.dir==-1 )
						continue;
				}
				else
				{
					// Mossa opposta alla precedente.
					continue;
				}
			}
		}

		rotate( curr.axis, curr.block, curr.dir );

		curr.h = evaluateH() + depth;
		// Controlla se il cubo è risolto.
		if( curr.h==depth )
		{
			// Controlla per sicurezza se il cubo è
			// effettivamente risolto.
			if( is_solved() )
			{
				// OK! Il cubo è risolto.
				list = malloc( sizeof(MoveList_t) );
				list->move = curr;
				list->next = head;
				head = list;

				rotate( curr.axis, curr.block, -curr.dir );
				return( TRUE );
			}
		}
		// Controlla se il nodo deve essere espanso.
		if( curr.h<=fstar )
		{
			register int solved = FALSE;
			// Cerca di effettuare delle potature sulla base
			// delle mosse precedentemente effettuate.
			if( (curr.axis==tabu.axis) && (curr.block==tabu.block) && (curr.dir==tabu.dir) )
			{
				// Per due volte di seguito ho
				// effettuato la stessa mossa.
				solved = IDAstar( curr, (depth+1), 2 );
			}
			else
			{
				// Espansione normale del nodo.
				solved = IDAstar( curr, (depth+1), 1 );
			}

			if( solved )
			{
				// La soluzione è stata trovata.
				list = malloc( sizeof(MoveList_t) );
				list->move = curr;
				list->next = head;
				head = list;

				rotate( curr.axis, curr.block, -curr.dir );
				return( TRUE );
			}
		}

		rotate( curr.axis, curr.block, -curr.dir );
	}
	return( FALSE );
}

//! Risolutore IDA*.
/**
 * Vedere IDAstar() per maggiori informazioni.
 */
int IDAstar_solver( MoveList_t **retlist )
{
	move_t start_move = { -1, -1, 0, ULONG_MAX };

	if( is_solved() )
		return( -1 );

	list = head = NULL;
	fstar = evaluateH();
	while( TRUE )
	{
		printf( "trying with F* = %i\n", fstar );
		if( IDAstar( start_move, 1, 1 ) )
		{
			break;
		}
		fstar++;
	}

	*retlist = head;
	return( 0 );
}
