/*
 * OSSO - A Micro Kernel OS
 * Copyright (c) 2000 Alessandro Iurlano.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */


/************************************ OSSO ************************************
 *  File: timer.h
 *  Descrizione: Prototipi e definizioni per la gestione del timer
 ******************************************************************************/

#ifndef __TIMER_DEFS

#define __TIMER_DEFS

#include <types.h>


/*EXPERIMENTAL!!*/
#define time_quantum 50;   /* time quantum of a running process */
#define maxtime MAXINT;    /* e' definito? 0xFFFFFFFF */


/* definizioni relative al PIT */

#define timerclock 0; /* timer channel */
#define ms 1193;      /* about 1 ms in timer ticks */

/* PIT I/O Ports */
#define ctrlport 0x43
#define ch0port  0x40
#define ch1port  0x41
#define ch2port  0x42

/* PIT control word */ /* mmm questa e' da riaggiustare.*/
typedef struct __PITCtrlWrd{
  unsigned char valore;
  unsigned char channel:2;  /* Channel to program; 3 means read-back */
  unsigned char bytemode:2; /* 1:LSB, 2:MSB, 3:LSB/MSB */
  unsigned char mode:3;     /* PIT operating mode */
  unsigned char code:1;     /* 0:binary, 1:bcd */
} ctrlwrd;

/* funzioni di controllo pit */

int getcurrtime (byte channel);

int setbinterval (byte channel, byte lsb, byte msb, byte mode);

#endif
