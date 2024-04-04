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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/lib/drv2mps.c,v $
 * Description: Interface between drivers and MPS.
 ***************************************************************************

 ***************************************************************************
 * $Id: drv2mps.c,v 1.13 2001/08/04 18:22:47 steros Exp $
 ***************************************************************************
 *
 * $Log: drv2mps.c,v $
 * Revision 1.13  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.12  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.11  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.10  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.9  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.8  2001/04/29 17:40:48  steros
 * Added dynamic mps pid request on drv2mps also.
 * Improved mps and his protocols.
 * There's still problems in taking MPS pid from process MPS_Pid variable.
 *
 * Revision 1.7  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.6  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.5  2001/04/20 20:45:36  steros
 * Added GetDriverPid function for driver pid retrival from MPS.
 *
 * Revision 1.4  2001/03/31 15:07:50  iurlano
 * bugfixes
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
#include "include/protocols/mpsint.h"
 *
 ***************************************************************************/


#include <processes.h>
#include <messages.h>
#include <drivers/protocols/mpsint.h>
#include <drivers/protocols/drv2mps.h>

#include <printf.h>



#ifdef DYNAMIC_MPS_PID
  #ifdef REQUEST_MPS_PID
    #define MPS_PID MPS_pid
    PID MPS_pid;
  #else
    #define MPS_PID MPS_Pid
    extern PID MPS_Pid;
  #endif
#else
  #define MPS_PID MPS_pid
  PID MPS_pid = 6;
#endif



/*
 First function to call.
 This function starts the initialization process getting data from MPS.
 It put the parameters structure in the passed variable.
 If params=0 the parameters are not returned.
 Returns 0 if ok or other in case of error.
*/
MpsRetVal DriverInit(char **params) {
  union drvmps_msg *msg;
  msginfo info;

  /* Waiting for start message. */
  msg=recv_msg(&info);
  if (!msg) return ERRDRV_BADMSG;
  if (msg->intest.Type!=DRVMPS_START) {
//    freemsg(msg);
    return ERRDRV_WRONGMSG;
  }
  /* Set return values. */
/*  *MPS_pid=msg->start.pid;  */
  if (params)
    *params=msg->start.data;
//  else
//    freemsg(msg);
  return ERRDRV_NOERR;
}


/*
 This function ends the initialization process registering to MPS.
 Returns 0 if ok or other in case of error.
*/
MpsRetVal DriverStart(char *category, char *name, bool multi_instances,
		                  int version, int release, char *description)
{
  union drvmps_msg *msg;
  msginfo info;
  int len=strlen(description);
  MpsRetVal status;

#ifdef DYNAMIC_MPS_PID
  #ifdef REQUEST_MPS_PID
    MPS_pid = Get_MPS_Pid();
  #endif
//  printf("<***** drv2mps; MPS pid = %d *****>\n",MPS_PID);
#endif
  /* Sending init message. */
  msg=getbmsg();
  msg->intest.Class=DRVMPS_CLASS;
  msg->intest.Version=DRV2MPS_PROTOCOL_VERSION;
  msg->intest.Release=DRV2MPS_PROTOCOL_RELEASE;
  msg->intest.Status=ERRDRV_NOERR;
  msg->intest.Type=DRVMPS_INIT;
  strcpy(msg->init.name,name);
  strcpy(msg->init.category,category);
  msg->init.multi_instances=(char)multi_instances;
  msg->init.driver_version=version;
  msg->init.driver_release=release;
  msg->init.protocol_version=DRV2MPS_PROTOCOL_VERSION;
  msg->init.protocol_release=DRV2MPS_PROTOCOL_RELEASE;
  strcpy(msg->init.description,description);
  send_msg(MPS_PID,msg,sizeof(drvmps_init_type)+len+1);
  /* Waiting for init reply. */
  msg=recv_msg(&info);
/*-*
  printf("<DRV-DBG1(%d)> msg=%u\n",getpid(),msg);
  printf("<DRV-DBG2(%d)> class=%d, ver=%d, rel=%d, status=%d, type=%d, pid=%d, dim=%d\n",
         getpid(),msg->intest.Class,msg->intest.Version,msg->intest.Release,msg->intest.Status,
  msg->intest.Type,MPS_PID,sizeof(drvmps_init_type)+len+1);
*-*/
  /* Processing reply. */
  if (!msg) return ERRDRV_BADMSG;
  if (msg->init_reply.intest.Status) {
    status=msg->init_reply.intest.Status;
    freemsg(msg);
    return status;
  }
  if (msg->init_reply.intest.Type!=DRVMPS_INIT_REPLY) {
    freemsg(msg);
    return ERRDRV_WRONGMSG;
  }
  freemsg(msg);
  return ERRDRV_NOERR;
}


MpsRetVal DriverExit(void) {
  union drvmps_msg *msg;

  msg=getbmsg();
  msg->intest.Class=DRVMPS_CLASS;
  msg->intest.Version=DRV2MPS_PROTOCOL_VERSION;
  msg->intest.Release=DRV2MPS_PROTOCOL_RELEASE;
  msg->intest.Status=ERRDRV_NOERR;
  msg->intest.Type=DRVMPS_TERM_NOTIFY;
  send_msg(MPS_PID,msg,sizeof(drvmps_term_notify_type));
}
