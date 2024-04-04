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
 * file : $Source: /cvsroot/osso/OSSO/src/include/events.h,v $
 * Description: Events management routines
 ***************************************************************************

 ***************************************************************************
 * $Id: events.h,v 1.11 2001/04/28 12:15:52 steros Exp $
 ***************************************************************************
 *
 * $Log: events.h,v $
 * Revision 1.11  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.10  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.1  2001/04/04 07:04:38  marcopex
 * Forgot some files...
 *
 *
 ***************************************************************************/

#ifndef __EVENTS__DEFS
#define __EVENTS__DEFS

#include <types.h>

#define MAX_EVENT_NAME_LENGTH 16 /* Maximum length of an event name */

/* MASKS: Ne servono altre? */
#define E_EXT   2  /* The event can be signaled from other processes */
#define E_BOOL  4  /* This is a bool event */

typedef char eventname[MAX_EVENT_NAME_LENGTH+1];
typedef eventname * EVENT;
typedef byte PERMS;
typedef dword VALS;
typedef void (* HANDLER)(void); /*Event Handler prototype void Handler(void)*/
typedef int TIMEOUT;

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

#endif


