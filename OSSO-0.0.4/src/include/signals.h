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


/*************************** lib O S S O  ***********************************
 * file : include/lib/signals.h
 * Header with signals definitions
 ***************************************************************************

 ***************************************************************************
 * $Id: signals.h,v 1.5 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: signals.h,v $
 * Revision 1.5  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.4  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:20  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.2  1999/12/21 13:59:28  iurlano
 * Non funzionano
 *
 * Revision 1.1  1996/09/12 05:04:45  iurlano
 * Riordino directory test e lib
 * adesso test compila con lib
 * prove di gestione di segnale a livello utente
 *
 *
 ***************************************************************************/

#ifndef __SIGNALS_LIB
#define __SIGNALS_LIB


#define SIGNAL_TERM          0 /* Termination request */ /* To do */
#define SIGNAL_USER1         1 /* User signal */ /* To do */
#define SIGNAL_USER2         2 /* User signal */ /* To do */
#define SIGNAL_ZERO_DIVIDE   3 /* Division by 0 */ /* To do */
#define SIGNAL_OVERFLOW      4 /* Overflow */ /* To do */


/* Processaware signal table size */
#define SIGNAL_NUMBER 5


#define SIGNAL_DESTROY      20 /* Complete desctruction of the process */
#define SIGNAL_STOP         21 /* Process stop */ /* To do */
#define SIGNAL_CONT         22 /* Process continue */ /* To do */


#include <types.h>

typedef void (*Signal_Handler)(void);

void set_signal_handler(unsigned long int sigg,Signal_Handler kk);
int Signal(PID Processo,dword segnale);

#endif
