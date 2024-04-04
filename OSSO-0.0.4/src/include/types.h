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
 * file : $Source: /cvsroot/osso/OSSO/src/include/types.h,v $
 * Definition of data types used in OSSO
 ***************************************************************************

 ***************************************************************************
 * $Id: types.h,v 1.9 2001/07/30 17:07:29 steros Exp $
 ***************************************************************************
 *
 * $Log: types.h,v $
 * Revision 1.9  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.8  2001/06/23 18:14:38  steros
 * Restructurated FAT to manage FAT32 and improve performance.
 *
 * Revision 1.7  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.6  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:21  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.2  1999/12/30 15:33:00  iurlano
 * Sistemati processi
 *
 * Revision 1.4  1999/12/02 23:09:20  iurlano
 * correzione include
 *
 * Revision 1.3  1999/12/02 17:18:01  iurlano
 * Prima versione di header di definizione di messaggi
 *
 * Revision 1.2  1999/11/25 13:09:00  pecciant
 * Standardizzati gli header dei .h
 *
 *
 ***************************************************************************/

#ifndef __TYPES_DEF
#define __TYPES_DEF

#define NULL ((void *) 0)

typedef enum { false=0, true=1 } bool;

typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned long int dword;
typedef unsigned long int size_t;

typedef unsigned long  ulong;
typedef unsigned short uint16;
typedef unsigned int   uint32;

typedef unsigned long int PID;

#endif
