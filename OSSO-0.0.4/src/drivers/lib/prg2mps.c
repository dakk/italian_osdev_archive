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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/lib/prg2mps.c,v $
 * Description: Interface between programs and MPS.
 ***************************************************************************

 ***************************************************************************
 * $Id: prg2mps.c,v 1.12 2001/07/30 17:07:29 steros Exp $
 ***************************************************************************
 *
 * $Log: prg2mps.c,v $
 * Revision 1.12  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.11  2001/06/03 09:53:17  steros
 * Bug fixes.
 *
 * Revision 1.10  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.9  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.8  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.7  2001/04/29 17:40:48  steros
 * Added dynamic mps pid request on drv2mps also.
 * Improved mps and his protocols.
 * There's still problems in taking MPS pid from process MPS_Pid variable.
 *
 * Revision 1.6  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.5  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2001/04/23 14:43:05  iurlano
 * work on FAT module
 * work on FS protocol
 *
 * Revision 1.3  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
 *
 * Revision 1.2  2001/04/20 20:45:36  steros
 * Added GetDriverPid function for driver pid retrival from MPS.
 *
 * Revision 1.1  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 *
 ***************************************************************************/


#include <printf.h>

#include <processes.h>
#include <messages.h>
#include <drivers/protocols/mpsint.h>
#include <drivers/protocols/prg2mps.h>



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
 This function returns the pid of the driver pointed by the couple
 (name,category).
*/
MpsRetVal GetDriverPid(char *category, char *name,
                       PID *driver_pid, MpsWaitType wt)
{
  union prgmps_msg *msg;
  msginfo info;
  MpsRetVal status;

#ifdef DYNAMIC_MPS_PID
  #ifdef REQUEST_MPS_PID
    MPS_pid = Get_MPS_Pid();
  #endif
//  printf("<***** prg2mps; MPS pid = %d *****>\n",MPS_PID);
#endif
  /* Sending request message. */
  msg=getbmsg();
  msg->intest.Class=PRGMPS_CLASS;
  msg->intest.Version=PRG2MPS_PROTOCOL_VERSION;
  msg->intest.Release=PRG2MPS_PROTOCOL_RELEASE;
  msg->intest.Status=ERRDRV_NOERR;
  msg->intest.Type=PRGMPS_ASKPID;
  msg->askpid.wait_type=wt;
  strcpy(msg->askpid.category,category);
  strcpy(msg->askpid.name,name);
  send_msg(MPS_PID,msg,sizeof(prgmps_askpid_type));
  msg=recv_msg(&info);
/*-*
  printf("<PRG-DBG1(%d)> Got message from %d, dim %d: msg=%u\n",getpid(),
         info.mittente,info.dimensione,msg);
  printf("<PRG-DBG2(%d)> class=%d, ver=%d, rel=%d, status=%d, type=%d, pid=%d, dim=%d\n",
         getpid(),msg->intest.Class,msg->intest.Version,msg->intest.Release,msg->intest.Status,
         msg->intest.Type,msg->askpid_reply.pid,info.dimensione);
*-*/
//printf("<PRG-DBG>-+-+-+-+- 1 -+-+-+-+-\n");
  if (!msg) return ERRDRV_BADMSG;
  /* Processing reply. */
//printf("<PRG-DBG>-+-+-+-+- 2 -+-+-+-+-\n");
  if (msg->askpid_reply.intest.Status) {
//printf("<PRG-DBG>-+-+-+-+- 2b -+-+-+-+-\n");
    status=msg->askpid_reply.intest.Status;
    freemsg(msg);
    return status;
  }
//printf("<PRG-DBG>-+-+-+-+- 3 -+-+-+-+-\n");
  if (msg->askpid_reply.intest.Type!=PRGMPS_ASKPID_REPLY) {
//printf("<PRG-DBG>-+-+-+-+- 3b -+-+-+-+-\n");
    freemsg(msg);
//printf("<PRG-DBG>-+-+-+-+- 3c -+-+-+-+-\n");
    return ERRDRV_WRONGMSG;
  }
//printf("<PRG-DBG>-+-+-+-+- 4 -+-+-+-+-\n");
  *driver_pid=msg->askpid_reply.pid;
  freemsg(msg);
  return ERRDRV_NOERR;
}
