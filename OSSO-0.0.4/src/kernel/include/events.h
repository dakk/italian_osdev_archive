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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/events.h,v $
 * Descrizione: Public definitions and prototypes for events managing in OSSO
 ***************************************************************************

 ***************************************************************************
 * $Id: events.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: events.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.8  2001/04/03 16:50:04  marcopex
 * Some little event testing & debugging
 *
 * Revision 1.7  2001/03/12 15:09:54  marcopex
 * Finished (maybe) the compilation of Events
 *
 *
 ***************************************************************************/

#ifndef __EVENTS__DEFS
#define __EVENTS__DEFS

#include <types.h>
#include <semaphore.h>

/**********************************/
/*DEFINITIONS AND DATA STRUCTURES */
/**********************************/

#define NUM_PUBLIC_EVENTS 16 /* Per ora gli IRQ so quelli */
#define MAX_EVENT_NUMBER 100 /* Maximum number of private events (TOTAL!) */
#define MAX_EVENT_NAME_LENGTH 16 /* Maximum length of an event name */

/* MASKS: Ne servono altre? */
#define E_MASK_TAKEN 1  /* The event is taken by someone */
#define E_MASK_EXT   2  /* The event can be signaled from other processes */
#define E_MASK_BOOL  4  /* This is a bool event */
#define E_MASK_FIX   8  /* This event is fixed and unmodifiable */

typedef char eventname[MAX_EVENT_NAME_LENGTH+1];
typedef eventname * EVENT;
typedef byte PERMS;
typedef PID OWNER;
typedef dword VALS;
typedef void (* HANDLER)(void); /*Event Handler prototype void Handler(void)*/
typedef int TIMEOUT;

typedef 
struct __event_struct {
  eventname event;
  OWNER owner;   /* Si potrebbe usare il campo next: non dovrebbe servire... */
  PERMS perms;
  HANDLER handler;
  Semaphore semaphore;
  TIMEOUT timeout;
  struct __event_struct * next;
} Event_struct;

typedef
struct __eattr_struct{
  PERMS permessi;
  VALS valore;
  HANDLER handler;
  TIMEOUT timeout;
} Eattrib_struct;

typedef Eattrib_struct * ATTRIBUTES;

/**************/ 
/* INTERFACE: */
/**************/

/*  These are the interfaces between kernel and process for managing events.
    Events are handled by processes only by these system calls */

int getevent(char *);
int clearevent(EVENT);
int setevent(EVENT,ATTRIBUTES);

int signal(PID,EVENT);
int wait(EVENT,PID *);

/* Functions called by kernel */
int init_Events(void);

#endif








