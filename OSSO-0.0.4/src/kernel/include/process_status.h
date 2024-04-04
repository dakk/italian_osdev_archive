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
 * file : include/process_status.h
 * Constants for process status
 ***************************************************************************


 ***************************************************************************
 * $Id: process_status.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: process_status.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.1  2000/01/06 17:08:13  iurlano
 * *** empty log message ***
 *
 *
 ***************************************************************************/


#ifndef __PROCESS_STATUS_DEFS
#define __PROCESS_STATUS_DEFS

#define PROCESS_DEAD         0
#define PROCESS_ZOMBIE       1
#define PROCESS_RUN          2
#define PROCESS_READY        3
#define PROCESS_STOPPED      4
#define PROCESS_STOP_SEM     5


#endif


