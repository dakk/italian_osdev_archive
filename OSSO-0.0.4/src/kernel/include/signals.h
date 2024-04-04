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


/****************************** O S S O  ***********************************
 * file : include/signals.h
 * Signal definition
 ***************************************************************************

 ***************************************************************************
 * $Id: signals.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: signals.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.2  2000/11/28 15:14:52  iurlano
 * Minor changes
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 ***************************************************************************/

#ifndef __SIGNALS_DEFS
#define __SIGNALS_DEFS

#include <sys.h>


/* Processware signals : */

#define SIGNAL_TERM          0 /* Richiesta di terminazione */ /* DA FARE */
#define SIGNAL_USER1         1 /* Segnale utente */ /* DA FARE */
#define SIGNAL_USER2         2 /* Segnale utente */ /* DA FARE */
#define SIGNAL_ZERO_DIVIDE   3 /* Notifica di divisione per 0 */ /* DA FARE */
#define SIGNAL_OVERFLOW      4 /* Notifica di Overflow */ /* DA FARE */


/* Processware signal table size */
#define SIGNAL_NUMBER 5

/* Kernelware signals : */

#define SIGNAL_DESTROY      20 /* Distruzione completa del processo */
#define SIGNAL_STOP         21 /* Ferma il processo */ /* DA FARE */
#define SIGNAL_CONT         22 /* Fa ripartire il processo */ /* DA FARE */


#define MAX_SIGNALS_PENDING 20

typedef void (*Signal_Handler)(void);

void Process_Signals(void);

#endif
