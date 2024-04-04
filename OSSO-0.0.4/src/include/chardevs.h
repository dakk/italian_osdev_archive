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
 * file : $Source: /cvsroot/osso/OSSO/src/include/chardevs.h,v $
 * Header of chardevs functions
 ***************************************************************************

 ***************************************************************************
 * $Id: chardevs.h,v 1.5 2001/04/28 12:15:52 steros Exp $
 ***************************************************************************
 *
 * $Log: chardevs.h,v $
 * Revision 1.5  2001/04/28 12:15:52  steros
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
 * Revision 1.1  1999/12/21 13:55:55  pecciant
 * Aggiunti dispositivi a caratteri
 *
 *
 ***************************************************************************/

#ifndef __DEF_CHARDEVS
#define __DEF_CHARDEVS

#define CHARBUFMAXSIZE 1024 /* for security */
#define MAXCHARDEVS 10  /* indicates current size */

/* returns a buffer of size dim, -1 in case of error */
int getcharbuffer(unsigned int  dim);

/* releases a buffer */
int releasecharbuffer();

#endif

