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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/types.h,v $
 * data types definitions
 ***************************************************************************

 ***************************************************************************
 * $Id: types.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: types.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.4  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.3  2000/12/05 16:36:01  iurlano
 * Thread managementwork in progress
 *
 * Revision 1.2  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:08  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.4  1999/12/02 23:09:20  iurlano
 * bug fixes
 *
 ***************************************************************************/

#ifndef __TYPES_DEF
#define __TYPES_DEF

typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned long int dword;
typedef unsigned int size_t;

typedef unsigned long int PID;
typedef unsigned int Thread_idx;

#define NULL ((void *) 0) 

#endif

