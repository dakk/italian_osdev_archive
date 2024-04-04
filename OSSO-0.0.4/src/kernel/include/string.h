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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/string.h,v $
 * Descrizione: Prototipi di funzione di stringa
 ***************************************************************************

 ***************************************************************************
 * $Id: string.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: string.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.3  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.2  2000/12/04 17:17:45  steros
 * Implemented kernel level strcpy and lib/kernel level sprintf.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.2  2000/03/07 21:37:52  iurlano
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.2  1999/11/25 13:08:58  pecciant
 * Standardizzati gli header dei .h
 *
 *
 ***************************************************************************/


#ifndef __STRING_DEFS
#define __STRING_DEFS

#include <types.h>

int strcmp(const char * s1,const char * s2);
int strcasecmp(const char *s1, const char *s2);
int strncmp(const char * s1,const char * s2, unsigned int n);
void *memcpy(void *dest, const void *src, size_t n);
char *strcpy(char *dest, const char *src);
int sprintf(char *str, char *format, ...);

#endif
