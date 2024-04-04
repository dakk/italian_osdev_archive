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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/mps_boot.h,v $
 * Description: Modules Pid Server: boot phase header.
 ***************************************************************************

 ***************************************************************************
 * $Id: mps_boot.h,v 1.3 2001/07/30 17:07:29 steros Exp $
 ***************************************************************************
 *
 * $Log: mps_boot.h,v $
 * Revision 1.3  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.2  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.1  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 *
 ***************************************************************************/


#include <processes.h>


#ifndef _MPS_BOOT_H_
#define _MPS_BOOT_H_


extern int BootRegisterFinished;


int  Boot_AddDriver(PID pid);
int  Boot_WaitForDriver(PID pid, char *category, char *name);
void Boot_RegisterDriver(char *category, char *name, PID pid,
                         PID *waiting, int *num_free);
void Boot_Complete();



#endif