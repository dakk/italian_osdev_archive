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
 * file : include/lock.h                                                   *
 * Lock Prototypes                                                         *
 ***************************************************************************


 ***************************************************************************
 * $Id: lock.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: lock.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.2  2001/03/06 22:52:11  iurlano
 * Added lock mechanism to P and V. To be tested
 * Added miniman xconfig Make target
 *
 * Revision 1.1  2001/03/06 11:50:26  iurlano
 * Added functions for smp multitasking (not yet tested)
 *
 *
 ***************************************************************************/



#ifndef __LOCK__DEFS
#define __LOCK__DEFS

#include <types.h>

void lock(dword * address);
void unlock(dword * address);

int lock_non_block(dword * address);


#endif
