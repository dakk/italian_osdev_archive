/*
 *	MatrixCube.cpp - Rappresentazione matriciale del cubo.
 *
 *      Copyright (C) 2004 Andrea Righi <righiandr@users.sf.net>
 *
 *	Facce del cubo di riferimento:
 *
 *              OOOOOOOOOOOO
 *             O          OO
 *            O   -1-    O O   -4-
 *           O          O  O
 *     -3-  OOOOOOOOOOOO-2-O
 *          O          O  O
 *          O    -0-   O O
 *          O          OO
 *          OOOOOOOOOOOO
 *               -5-
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MatrixCube.h"
#include "Move.h"

//! La matrice che rappresenta il cubo di Rubik.
block_t mat_cube[6][3][3] =
{
#if 0
	// This is a particular configuration that can be used
	// to evaluate performace between different machines.
	{
		{ Green , Green , Green  },
		{ Green , Blue , Green  },
		{ Green , Green , Green  },
	},
	{
		{ Red   , Red   , Red    },
		{ Red   , Red   , Red    },
		{ Red   , Red   , Red    },
	},
	{
		{ White , White , White  },
		{ White , Yellow , White  },
		{ White , White , White  },
	},
	{
		{ Yellow, Yellow, Yellow },
		{ Yellow, White, Yellow },
		{ Yellow, Yellow, Yellow },
	},

	{
		{ Blue  , Blue  , Blue   },
		{ Blue  , Green  , Blue   },
		{ Blue  , Blue  , Blue   },
	},
	{
		{ Orange, Orange, Orange },
		{ Orange, Orange, Orange },
		{ Orange, Orange, Orange },
	},
#else
	{
		{ Red   , Red   , Red    },
		{ Red   , Red   , Red    },
		{ Red   , Red   , Red    },
	},

	{
		{ Yellow, Yellow, Yellow },
		{ Yellow, Yellow, Yellow },
		{ Yellow, Yellow, Yellow },
	},

	{
		{ Blue  , Blue  , Blue   },
		{ Blue  , Blue  , Blue   },
		{ Blue  , Blue  , Blue   },
	},
	{
		{ Green , Green , Green  },
		{ Green , Green , Green  },
		{ Green , Green , Green  },
	},
	{
		{ Orange, Orange, Orange },
		{ Orange, Orange, Orange },
		{ Orange, Orange, Orange },
	},
	{
		{ White , White , White  },
		{ White , White , White  },
		{ White , White , White  },
	},
#endif
};

// Euristica corrente da utilizzare.
volatile int heuristic = 1;

//! Rappresentazione dei colori su stdout.
static char colors[] = "RYBGOW";

//! Imposta la matrice alla configurazione iniziale (cubo risolto).
void reset()
{
	block_t init_mat[6][3][3] =
	{
		// Faccia 0 //
		{
			{ Red   , Red   , Red    },
			{ Red   , Red   , Red    },
			{ Red   , Red   , Red    },
		},
		// Faccia 1 //
		{
			{ Yellow, Yellow, Yellow },
			{ Yellow, Yellow, Yellow },
			{ Yellow, Yellow, Yellow },
		},
		// Faccia 2 //
		{
			{ Blue  , Blue  , Blue   },
			{ Blue  , Blue  , Blue   },
			{ Blue  , Blue  , Blue   },
		},
		// Faccia 3 //
		{
			{ Green , Green , Green  },
			{ Green , Green , Green  },
			{ Green , Green , Green  },
		},
		// Faccia 4 //
		{
			{ Orange, Orange, Orange },
			{ Orange, Orange, Orange },
			{ Orange, Orange, Orange },
		},
		// Faccia 5 //
		{
			{ White , White , White  },
			{ White , White , White  },
			{ White , White , White  },
		},
	};
	register int f, i, j;

	// Trasferisce il contenuto del file nella init_mat.
	for( f=0; f<6; f++ )
	{
		for( i=0; i<3; i++ )
		{
			for( j=0; j<3; j++ )
			{
				mat_cube[f][i][j] = init_mat[f][i][j];
			}
		}
	}
}

//! Ruota una singola faccia del cubo.
static __INLINE__ void rotate_face(block_t face[3][3], int sign)
{
	block_t temp[3][3];
	// Copia la matrice nella struttura temporanea.
	memcpy( temp, face, sizeof(block_t)*3*3 );
	if ( sign>0 )
	{
		// Effettua la rotazione antioraria.
		face[0][0] = temp[0][2];
		face[1][0] = temp[0][1];
		face[2][0] = temp[0][0];
		face[2][1] = temp[1][0];
		face[2][2] = temp[2][0];
		face[1][2] = temp[2][1];
		face[0][2] = temp[2][2];
		face[0][1] = temp[1][2];
	}
	else
	{
		// Effettua la rotazione oraria.
		face[0][0] = temp[2][0];
		face[0][1] = temp[1][0];
		face[0][2] = temp[0][0];
		face[1][2] = temp[0][1];
		face[2][2] = temp[0][2];
		face[2][1] = temp[1][2];
		face[2][0] = temp[2][2];
		face[1][0] = temp[2][1];
	}
}

//! Ruota la matrice sull'asse X.
static __INLINE__ void rotateX(int block, int dir)
{
	block_t temp[3];
	register int i;

	// Rotazione dei blocchi nelle facce.
	// Ricorda che le colonne e le righe della faccia #4 sono
	// speculari.
	if ( dir>0 )
	{
		for( i=0; i<3; i++ )
			temp[i] = mat_cube[0][i][block];
		for( i=0; i<3; i++ )
			mat_cube[0][i][block] = mat_cube[5][i][block];
		for( i=0; i<3; i++ )
			mat_cube[5][i][block] = mat_cube[4][2-i][2-block];
		for( i=0; i<3; i++ )
			mat_cube[4][2-i][2-block] = mat_cube[1][i][block];
		for( i=0; i<3; i++ )
			mat_cube[1][i][block] = temp[i];
	}
	else
	{
		for( i=0; i<3; i++ )
			temp[i] = mat_cube[0][i][block];
		for( i=0; i<3; i++ )
			mat_cube[0][i][block] = mat_cube[1][i][block];
		for( i=0; i<3; i++ )
			mat_cube[1][i][block] = mat_cube[4][2-i][2-block];
		for( i=0; i<3; i++ )
			mat_cube[4][2-i][2-block] = mat_cube[5][i][block];
		for( i=0; i<3; i++ )
			mat_cube[5][i][block] = temp[i];
	}

	// Rotazione delle facce laterali.
	switch( block )
	{
		case 0:
			if ( dir>0 )
			{
				// Effettua la rotazione antioraria.
				rotate_face(mat_cube[3], +1);
			}
			else
			{
				// Effettua la rotazione oraria.
				rotate_face(mat_cube[3], -1);
			}
		break;

		case 1:
		// Il blocco centrale non provoca rotazioni
		// nelle facce laterali.
		break;

		case 2:
			if ( dir>0 )
			{
				// Effettua la rotazione oraria.
				rotate_face(mat_cube[2], -1);
			}
			else
			{
				// Effettua la rotazione antioraria.
				rotate_face(mat_cube[2], +1);
			}
		break;
	}
}

//! Ruota la matrice sull'asse Y.
static __INLINE__ void rotateY(int block, int dir)
{
	block_t temp[3];
	register int i;

	// Rotazione dei blocchi nelle facce.
	// Ricorda che le colonne e le righe della faccia #4 sono
	// speculari.
	if ( dir>0 )
	{
		for( i=0; i<3; i++ )
			temp[i] = mat_cube[0][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[0][2-block][i] = mat_cube[2][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[2][2-block][i] = mat_cube[4][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[4][2-block][i] = mat_cube[3][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[3][2-block][i] = temp[i];
	}
	else
	{
		for( i=0; i<3; i++ )
			temp[i] = mat_cube[0][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[0][2-block][i] = mat_cube[3][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[3][2-block][i] = mat_cube[4][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[4][2-block][i] = mat_cube[2][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[2][2-block][i] = temp[i];
	}

	// Rotazione delle facce laterali.
	switch( block )
	{
		case 0:
			if ( dir>0 )
			{
				// Effettua la rotazione antioraria.
				rotate_face(mat_cube[5], +1);
			}
			else
			{
				// Effettua la rotazione oraria.
				rotate_face(mat_cube[5], -1);
			}
		break;

		case 1:
		// Il blocco centrale non provoca rotazioni
		// nelle facce laterali.
		break;

		case 2:
			if ( dir>0 )
			{
				// Effettua la rotazione oraria.
				rotate_face(mat_cube[1], -1);
			}
			else
			{
				// Effettua la rotazione antioraria.
				rotate_face(mat_cube[1], +1);
			}
		break;
	}
}

//! Ruota la matrice sull'asse Z.
static __INLINE__ void rotateZ(int block, int dir)
{
	block_t temp[3];
	register int i;

	// Rotazione dei blocchi nelle facce.
	// Ricorda che le colonne e le righe della faccia #4 sono
	// speculari.
	if ( dir>0 )
	{
		for( i=0; i<3; i++ )
			temp[i] = mat_cube[1][block][i];
		for( i=0; i<3; i++ )
			mat_cube[1][block][i] = mat_cube[3][2-i][block];
		for( i=0; i<3; i++ )
			mat_cube[3][i][block] = mat_cube[5][2-block][i];
		for( i=0; i<3; i++ )
			mat_cube[5][2-block][i] = mat_cube[2][2-i][2-block];
		for( i=0; i<3; i++ )
			mat_cube[2][i][2-block] = temp[i];
	}
	else
	{
		for( i=0; i<3; i++ )
			temp[i] = mat_cube[1][block][i];
		for( i=0; i<3; i++ )
			mat_cube[1][block][i] = mat_cube[2][i][2-block];
		for( i=0; i<3; i++ )
			mat_cube[2][i][2-block] = mat_cube[5][2-block][2-i];
		for( i=0; i<3; i++ )
			mat_cube[5][2-block][i] = mat_cube[3][i][block];
		for( i=0; i<3; i++ )
			mat_cube[3][i][block] = temp[2-i];
	}

	// Rotazione delle facce laterali.
	switch( block )
	{
		case 0:
			if ( dir>0 )
			{
				// Effettua la rotazione antioraria.
				rotate_face(mat_cube[4], +1);
			}
			else
			{
				// Effettua la rotazione oraria.
				rotate_face(mat_cube[4], -1);
			}
		break;

		case 1:
		// Il blocco centrale non provoca rotazioni
		// nelle facce laterali.
		break;

		case 2:
			if ( dir>0 )
			{
				// Effettua la rotazione oraria.
				rotate_face(mat_cube[0], -1);
			}
			else
			{
				// Effettua la rotazione antioraria.
				rotate_face(mat_cube[0], +1);
			}
		break;
	}
}

//! Funzione per effettuare le mosse sul cubo.
void rotate(int axis, int block, int dir)
{
	switch( axis )
	{
		case 0:
			rotateX( block, dir );
		break;
		case 1:
			rotateY( block, dir );
		break;
		case 2:
			rotateZ( block, dir );
		break;
	}
}

//! Funzione euristica H migliore (anche se non ammissibile!).
/**
 * Questa funzione restituisce un valore determinato nel modo seguente:
 * \li
 *	per ogni faccia valuta la somma delle celle diverse per righe e
 *	per colonne e prende il minore;
 * \li
 *	infine somma tutti i valori trovati.
 * \note
 *	Dividendo il valore per 6 si ottiene la media, che può essere
 *	utilizzata come stima del numero di mosse che mancano dalla
 *	soluzione. Anche se questa non è una euristica ammissibile,
 *	fornisce tuttavia una buona stima, per questo si ottengono dei
 *	risultati migliori rispetto alle altre euristiche.
 * \note
 *	Scegliendo di utilizzare questa euristica si rinuncia quindi
 *	all'ottimalità della soluzione, ma ci consente di arrivare
 *	a delle soluzioni a profondità maggiore (fino a 12-13 mosse
 *	con tempi inferiori a 2 minuti).
 */
static __INLINE__ int evaluateH_best()
{
	int htot=0;
	int solved=TRUE;
	int f, i;

	for( f=0; f<6; f++ )
	{
		register int hrow=0, hcol=0;

		for( i=0; i<3; i++ )
		{
			hrow += ( mat_cube[f][i][0]!=mat_cube[f][i][1] );
			hrow += ( mat_cube[f][i][0]!=mat_cube[f][i][2] );
			hrow += ( mat_cube[f][i][1]!=mat_cube[f][i][2] );
		}
		for( i=0; i<3; i++ )
		{
			hcol += ( mat_cube[f][0][i]!=mat_cube[f][1][i] );
			hcol += ( mat_cube[f][0][i]!=mat_cube[f][2][i] );
			hcol += ( mat_cube[f][1][i]!=mat_cube[f][2][i] );
		}

		if( hrow || hcol )
		{
			solved=FALSE;
			htot += MIN( hrow, hcol );
		}
	}

	return( solved ? 0 : (htot ? htot/6 : 1) );
}

//! Euristica basata sul calcolo delle distanze (non ammissibile).
/**
 * Questa euristica prende come riferimento le celle centrali di ogni
 * faccia. Ogni cella confronta il proprio colore con quello della
 * cella centrale; se i colori sono diversi si aggiunge +1 al numero di
 * mosse, se una faccia adiacente ha come colore del blocco centrale il
 * colore della cella; +2 se il blocco centrale della cella opposta ha
 * come colore quello della cella presa in considerazione.\n
 * Alla fine occorrerà dividere per 12 la somma totale, dato che ogni
 * volta che viene effettuata una mossa vengono spostate 12 celle.
 */
static __INLINE__ int evaluateH_dist()
{
	static unsigned char opponents[6] = { 4,5,3,2,0,1 };
	register int htot = 0;
	int f, i, j;

	for( f=0; f<6; f++ )
	{
		register int h=0;
		for( i=0; i<3; i++ )
		for( j=0; j<3; j++ )
		{
			if( mat_cube[f][i][j]!=mat_cube[f][1][1] )
			{
				if( mat_cube[f][i][j]==mat_cube[opponents[f]][1][1] )
					h+=2;
				else
					h++;
			}
		}
		htot += h;
	}
	return( htot/12 );
}

//! Funzione euristica sui colori.
/**
 * Questa funzione restituisce la media del numero di colori diversi
 * per ogni faccia. Tale valore può essere utilizzato come
 * stima del numero di mosse che mancano alla soluzione. Questa
 * euristica soddisfa l'ammissibilità, cioè non sovrastima mai il
 * numero di mosse che mancano a raggiungere l'obiettivo.
 */
static __INLINE__ int evaluateH_col()
{
	register int htot=0;
	int f, i, j;

	for( f=0; f<6; f++ )
	{
		register unsigned int diffcols=0;
		register int h=0;

		for( i=0; i<3; i++ )
		for( j=0; j<3; j++ )
			diffcols |= (1 << mat_cube[f][i][j]);

		for( i=0; i<6; i++ )
			h += ((diffcols>>i) & 1);

		htot += h;
	}
	return( htot ? (htot-6)/6 : 0 );
}

//! Determina quale funzione euristica utilizzare per il calcolo
//! della H.
int evaluateH()
{
	switch( heuristic )
	{
		case 1:
			return( evaluateH_best() );
		break;
		case 2:
			return( evaluateH_dist() );
		break;
		case 3:
			return( evaluateH_col() );
		break;
		case 4:
			return( MAX(evaluateH_col(), evaluateH_dist()) );
		break;
		case 5:
			return( MAX(evaluateH_best(), MAX(evaluateH_col(), evaluateH_dist())) );
		break;

		default:
		break;
	}
	return( 0 );
}

//! Controlla se il cubo è risolto.
int is_solved()
{
	register block_t *cmp;
	register int i, j;

	for( i=0; i<6; i++ )
	{
		for( j=1, cmp=(block_t *)&(mat_cube[i][0][1]); j<9; j++, cmp++ )
		{
			if( *cmp != *((block_t *)(mat_cube[i])) )
				return( FALSE );
		}
	}
	return( TRUE );
}

//! Stampa su standard output le 6 facce del cubo.
void printfaces()
{
	int row, face;

	// Header.
	printf(
		" Face0      Face1      Face2      Face3      Face4      Face5\n"
		"-------    -------    -------    -------    -------    -------\n"
	);
	// Valori.
	for( row=0; row<3; row++ )
	{
		for( face=0; face<6; face++ )
		{
			printf( "%c  %c  %c    ",
				colors[ mat_cube[face][row][0] ],
				colors[ mat_cube[face][row][1] ],
				colors[ mat_cube[face][row][2] ]
			);
		}
		putchar('\n');
	}
}
