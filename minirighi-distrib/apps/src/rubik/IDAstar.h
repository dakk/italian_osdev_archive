/*
 *	IDAstar.h - Header del procedimento di risoluzione IDA*.
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

#ifndef IDASTAR_H
#define IDASTAR_H

#include "MatrixCube.h"
#include "Move.h"

// --- Prototipi ------------------------------------------------------ //

int IDAstar_solver( MoveList_t **retlist );

#endif
