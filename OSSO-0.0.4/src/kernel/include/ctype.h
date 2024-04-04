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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/ctype.h,v $
 * Descrizione: Definizioni di dati
 ***************************************************************************

 ***************************************************************************
 * $Id: ctype.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: ctype.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:05  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:40  pecciant
 *
 *
 * Revision 1.2  1999/11/25 13:08:53  pecciant
 * Standardizzati gli header dei .h
 *
 *
 ***************************************************************************/

#ifndef __CTYPE_DEFS
#define __CTYPE_DEFS

/* Identical to the ones of the libc */

int isalnum (int c); 
int isalpha (int c); 
int isascii (int c);
int isblank (int c);
int iscntrl (int c);
int isdigit (int c);
int isgraph (int c);
int islower (int c);
int isprint (int c);
int ispunct (int c);
int isspace (int c);
int isupper (int c);
int isxdigit (int c);

#endif
