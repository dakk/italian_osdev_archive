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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/sysinfo.h,v $
 * Prototypes of System Information functions
 ***************************************************************************/

#ifndef __SYSINFO_DEFS
#define __SYSINFO_DEFS

#include <types.h>

extern word serial_ports[4];
extern word parallel_ports[3];
extern int serial_ports_number;
extern int parallel_ports_number;

extern word EBDA_segment;
extern word BIOS_InstalledHW;


void check_BIOS_32(void);

#endif
