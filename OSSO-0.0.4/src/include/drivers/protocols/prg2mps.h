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
 * file : $Source: /cvsroot/osso/OSSO/src/include/drivers/protocols/prg2mps.h,v $
 * Description: Interface between programs and MPS.
 ***************************************************************************

 ***************************************************************************
 * $Id: prg2mps.h,v 1.4 2001/05/01 13:47:39 steros Exp $
 ***************************************************************************
 *
 * $Log: prg2mps.h,v $
 * Revision 1.4  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.3  2001/04/29 17:40:48  steros
 * Added dynamic mps pid request on drv2mps also.
 * Improved mps and his protocols.
 * There's still problems in taking MPS pid from process MPS_Pid variable.
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.2  2001/04/20 20:45:36  steros
 * Added GetDriverPid function for driver pid retrival from MPS.
 *
 * Revision 1.1  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 *
 ***************************************************************************/


#ifndef _PRG2MPS_PROTOCOL_
#define _PRG2MPS_PROTOCOL_


#include <drivers/protocols/mpsint.h>


#define PRG2MPS_PROTOCOL_VERSION 0
#define PRG2MPS_PROTOCOL_RELEASE 1


typedef enum {
  MPS_NOWAIT,     /* No wait, MPS reply immediately.           */
  MPS_WAITRUN,    /* If loading, wait for driver to be ready.  */
  MPS_WAITLOAD,   /* If not present, force the loading of the
                     driver and wait for it to be ready.       */
} MpsWaitType;


/* Program interface message types. */

typedef enum {
  PRGMPS_ASKPID,
  PRGMPS_ASKPID_REPLY
} prgmps_request_type;

typedef struct {
  mps_class_type      Class;
  int                 Version;
  int                 Release;
  MpsRetVal           Status;
  prgmps_request_type Type;
} prgmps_intest_type;


typedef struct {
  prgmps_intest_type intest;
  MpsWaitType        wait_type;
  char               name[MAX_DRIVER_NAME_LEN];
  char               category[MAX_DRIVER_CATEGORY_LEN];
} prgmps_askpid_type;


typedef struct {
  prgmps_intest_type intest;
  PID                pid;
} prgmps_askpid_reply_type;


union prgmps_msg {
  prgmps_intest_type       intest;
  prgmps_askpid_type       askpid;
  prgmps_askpid_reply_type askpid_reply;
};



MpsRetVal GetDriverPid(char *category, char *name,
                       PID *driver_pid, MpsWaitType wt);


#endif
