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
 * file : $Source: /cvsroot/osso/OSSO/src/include/drivers/protocols/drv2mps.h,v $
 * Description: Interface between drivers and MPS.
 ***************************************************************************

 ***************************************************************************
 * $Id: drv2mps.h,v 1.6 2001/08/04 18:22:47 steros Exp $
 ***************************************************************************
 *
 * $Log: drv2mps.h,v $
 * Revision 1.6  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.5  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
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
 * Revision 1.4  2001/04/20 20:45:36  steros
 * Added GetDriverPid function for driver pid retrival from MPS.
 *
 * Revision 1.3  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 * Revision 1.2  2001/03/23 21:05:49  iurlano
 *
 * I've finally found the bug! it is in process.c in init_process_structures
 * the problem is the for that initializes the mutexes of the processes
 * I can't figure why it is wrong, yet.
 *
 * Revision 1.1  2001/03/10 17:08:48  steros
 * Added header and lib functions to interface driver to MPS.
 *
 *
 ***************************************************************************/


#ifndef _DRV2MPS_PROTOCOL_
#define _DRV2MPS_PROTOCOL_


#include <drivers/protocols/mpsint.h>


#define DRV2MPS_PROTOCOL_VERSION 0
#define DRV2MPS_PROTOCOL_RELEASE 1


/* Driver interface message types. */

typedef enum {
  DRVMPS_START,     /* This message has no reply. */
  DRVMPS_INIT,
  DRVMPS_INIT_REPLY,
//  DRVMPS_KILL,
//  DRVMPS_KILL_REPLY,
  DRVMPS_TERM_NOTIFY
} drvmps_request_type;


#define DRV_I_MULTI true
#define DRV_I_MONO  false

typedef struct {
  mps_class_type      Class;
  int                 Version;
  int                 Release;
  MpsRetVal           Status;
  drvmps_request_type Type;
} drvmps_intest_type;


typedef struct {
  drvmps_intest_type intest;
  PID                pid;
  char data[1];     /* This field can be of variable dimension! */
} drvmps_start_type;



typedef struct {
  drvmps_intest_type intest;
  int                protocol_version;
  int                protocol_release;
  char               name[MAX_DRIVER_NAME_LEN];
  char               category[MAX_DRIVER_CATEGORY_LEN];
  char               multi_instances;
  int                driver_version;
  int                driver_release;
  char               description[1];
                         /* This field can be of variable dimension! */
} drvmps_init_type;


typedef struct {
  drvmps_intest_type intest;
  int                reply;
} drvmps_init_reply_type;



typedef struct {
  drvmps_intest_type intest;
} drvmps_term_notify_type;



union drvmps_msg {
  drvmps_intest_type      intest;
  drvmps_start_type       start;
  drvmps_init_type        init;
  drvmps_init_reply_type  init_reply;
  drvmps_term_notify_type term_notify;
};



/* Implemented functions. */

MpsRetVal DriverInit(char **params);
MpsRetVal DriverStart(char *category, char *name,  bool multi_instances,
                      int version, int release, char *description);
MpsRetVal DriverExit(void);


#endif
