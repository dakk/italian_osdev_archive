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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/PnP.h,v $
 * Description: Definitions and prototypes for PnP*
 ***************************************************************************

 ***************************************************************************
 * $Id: PnP.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: PnP.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.4  2000/11/21 23:03:08  iurlano
 * Added pci.h with prototypes for pci
 * Added prototype of boot loader wrote with bcc
 * Added bootfiles version modified by steros
 *
 * Revision 1.3  2000/10/16 14:12:16  iurlano
 * Translation in progress
 *
 *
 ***************************************************************************/


#ifndef __PnP_DEFS
#define __PnP_DEFS

#include <types.h>

int Check_PnP(void);
int Check_BIOS32(void);

#define PnP_SIGNATURE "$PnP"


#endif
