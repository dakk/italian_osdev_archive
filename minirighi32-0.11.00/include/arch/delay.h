/*!	\file include/arch/delay.h
 *	\brief Precise dalay loops for i386.
 *	\date Last update: 2004-03-31
 *	\note Copyright (C) 1997 Linus Torvalds.
 */

#ifndef DELAY_H
#define DELAY_H

// --- Prototypes ----------------------------------------------------- //

void udelay( unsigned long usecs );
void ndelay( unsigned long nsecs );

void calibrate_loops_per_tick( void );

#endif

