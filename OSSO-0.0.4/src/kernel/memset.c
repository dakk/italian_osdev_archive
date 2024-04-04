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


/******************************* O S S O ***********************************
 * file : kernel/memset.c                                                  *
 * Compiler used function to init stack memory                             *
 ***************************************************************************

 ***************************************************************************
 * $Id: memset.c,v 1.2 2000/07/27 12:13:07 iurlano Exp $
 ***************************************************************************
 *
 * $Log: memset.c,v $
 * Revision 1.2  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:11  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 ***************************************************************************/

void *memset(void *s, int c, unsigned int n){
  int i;
  char b=c; /* Conversion */
  for (i=0;i<n;i++) ((char *)s)[i]=b; /* Filling */
  return s; /* Returns the pointer */
}
