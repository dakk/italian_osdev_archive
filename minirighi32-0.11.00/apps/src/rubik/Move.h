/*
 *	Move.h - Header per le mosse.
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

#ifndef MOVE_H
#define MOVE_H

#define TRUE		1
#define FALSE		0

#define NULL		0

//! This macro returns the minimum value between a and b.
#define MIN( a, b )       ((a) < (b) ? (a) : (b))

//! This macro returns the maximum value between a and b.
#define MAX( a, b )       ((a) > (b) ? (a) : (b))

//! Esplora ogni possibile mossa a partire da uno stato.
#define FOREACH_MOVE( c ) \
	for( c.axis=0; c.axis<3; c.axis++ ) \
	for( c.block=0; c.block<3; c.block+=2 ) \
	for( c.dir=-1; c.dir<2; c.dir+=2 )

//! Struttura usata per memorizzare una mossa.
typedef struct move
{
	int	axis;
	int	block;
	int	dir;
	int	h;
} move_t;

//! Lista linkata per le mosse finali.
typedef struct MoveList
{
	move_t move;
	struct MoveList *next;
} MoveList_t;

#endif
