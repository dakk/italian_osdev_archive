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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/mps_struct.h,v $
 * Description: Modules Pid Server: drivers data management header.
 ***************************************************************************

 ***************************************************************************
 * $Id: mps_struct.h,v 1.6 2001/08/04 18:22:47 steros Exp $
 ***************************************************************************
 *
 * $Log: mps_struct.h,v $
 * Revision 1.6  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.5  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.4  2001/07/08 11:12:50  steros
 * Converted MPS to dynamic allocation and other little fix.
 *
 * Revision 1.3  2001/05/03 17:24:06  steros
 * Improved MPS boot phase.
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
#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>

#ifndef _MPS_STRUCT_H_
#define _MPS_STRUCT_H_


int InitStruct(void);
int DriverPidPresent(PID pid);
PID FindDriverPid(char *category, char *name, PID PidForWait);
MpsRetVal AddNewDriver(char *category, char *name, PID pid,
                       char multi_instances, int version,
                       int release, char *description);
MpsRetVal DeleteInstance(PID pid);
void CloseBootPhase(void);
int RunDriver(char *category, char *name, PID pid);

/* FOR DEBUG */
void DBGViewData(void);


#endif
