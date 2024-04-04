/*
 *	MatrixCube.h - Header per la rappresentazione matriciale del cubo.
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

#ifndef MATRIXCUBE_H
#define MATRIXCUBE_H

//! Enumerazione dei colori per le facce.
enum colors { Red, Yellow, Blue, Green, Orange, White } __attribute__ ((packed));

//! Tipo di dato: cella del cubo.
typedef unsigned char block_t;

// --- Prototipi ------------------------------------------------------ //

void rotate(int axis, int block, int dir);
int evaluateH();
int is_solved();
void printfaces();

#endif
