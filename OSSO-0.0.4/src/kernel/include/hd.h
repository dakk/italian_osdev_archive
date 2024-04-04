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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/hd.h,v $
 * Descrizione: Definizioni per l'uso basilare dell'Hard Disk
 ***************************************************************************

 ***************************************************************************
 * $Id: hd.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: hd.h,v $
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
 * Revision 1.1.1.1  2000/05/30 08:11:06  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#ifndef __HD_DEFS
#define __HD_DEFS

int read_sector(unsigned int device,unsigned int logical_sector,char * data);
int write_sector(unsigned int device,unsigned int logical_sector,char * data);

int read_sector_hw(unsigned int device,unsigned int cyl,
		   unsigned int head,unsigned int sec,char * dat);

int write_sector_hw(unsigned int device,unsigned int cyl,
		    unsigned int head,unsigned int sec,char * dat);

void check_hd(void);
void get_BIOS_geometry(void);

#endif
