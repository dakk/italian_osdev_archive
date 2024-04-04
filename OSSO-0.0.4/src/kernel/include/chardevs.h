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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/chardevs.h,v $
 * Descrizione: Header delle funzioni per dispositivi a caratteri
 ***************************************************************************

 ***************************************************************************
 * $Id: chardevs.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: chardevs.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.2  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.1.1.1  2000/05/30 08:11:05  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#ifndef __DEF_CHARDEVS
#define __DEF_CHARDEVS

#include <process.h>

#define CHARBUFMAXSIZE 1024 /* for security */
#define MAXCHARDEVS 10  /* non influisce sull'allocazione, indica
			    solo la dimensione corrente */

/* questo un ci dovrebbe sta' qui' porca... */
typedef struct{
  char present;
  char * buffer;
  PID ownerpid;
  unsigned int size;
  unsigned int head;
  unsigned int tail;
  Bool_Semaphore semaforo;
}Chardev;

/* variabili globali */
Chardev * chardevs;      /* vettore di dispositivi a caratteri */
Bool_Semaphore CH_Mutex; /* mutua esclusione alle strutture condivise */


/* inizializzazione */
void initchardevs(void);

/* ritorna un buffer di dimensione dim, -1 in caso di errore */
int getcharbuffer(unsigned int  dim);

/* rilascia un buffer precedentemente acquisito */
int releasecharbuffer(unsigned int fd);

typedef int ssize_t;

ssize_t read(int fd, void *buf, ssize_t count);
ssize_t write(int fd, const void *buf, ssize_t count);

#endif






