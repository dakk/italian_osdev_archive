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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/io.h,v $
 * Description: Definitions of hardware io
 ***************************************************************************

 ***************************************************************************
 * $Id: io.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: io.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.3  2000/10/17 22:57:30  iurlano
 * io changes (added outd & ind)
 * printc expansion (added %ld & %lx in format)
 * work on rtl8139
 *
 * Revision 1.2  2000/10/16 14:12:16  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:06  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#ifndef __IO_DEFS
#define __IO_DEFS


#include <types.h>

void outb(byte value, word port);
void outw(word value, word port);
void outd(dword value, word port);
byte inb(word port);
word inw(word port);
dword ind(word port);

#endif
