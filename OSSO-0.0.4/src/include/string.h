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
 * file : lib/include/string.h
 * Header for string functions
 ***************************************************************************

 ***************************************************************************
 * $Id: string.h,v 1.7 2001/06/25 20:23:01 steros Exp $
 ***************************************************************************
 *
 * $Log: string.h,v $
 * Revision 1.7  2001/06/25 20:23:01  steros
 * Added strcat.
 *
 * Revision 1.6  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.5  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.3  2000/12/04 17:17:46  steros
 * Implemented kernel level strcpy and lib/kernel level sprintf.
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:21  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.3  2000/03/26 13:20:23  iurlano
 * Completamento protocollo di INIT
 * Da completarne il test
 *
 * Revision 1.2  2000/03/20 12:02:16  iurlano
 * lavoro su drivers
 *
 * Revision 1.1  2000/03/20 08:39:18  iurlano
 * Aggiunto string.h agli header della libreria
 *
 ***************************************************************************/

#ifndef __STRING_LIB
#define __STRING_LIB

char  *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, unsigned int n);
int  strcmp(const char *s1, const char *s2);
int  strcasecmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, unsigned int n);
unsigned int strlen(const char *s);
char *strcat(char *dest, const char *src);
void *memcpy(void *dest, const void *src, unsigned int n);
int sprintf(char *str, char *format, ...);

#endif
