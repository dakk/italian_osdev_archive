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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/mps.c,v $
 * Description: Modules Pid Server
 ***************************************************************************

 ***************************************************************************
 * $Id: mps.c,v 1.18 2001/08/06 20:14:13 steros Exp $
 ***************************************************************************
 *
 * $Log: mps.c,v $
 * Revision 1.18  2001/08/06 20:14:13  steros
 * Fixes.
 *
 * Revision 1.17  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.16  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.15  2001/07/08 11:12:50  steros
 * Converted MPS to dynamic allocation and other little fix.
 *
 * Revision 1.14  2001/06/10 17:16:23  steros
 * Fixes.
 *
 * Revision 1.13  2001/06/09 18:30:23  steros
 * Fixes.
 *
 * Revision 1.12  2001/06/03 09:53:17  steros
 * Bug fixes.
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
 * Revision 1.7  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
 *
 * Revision 1.6  2001/04/20 20:45:36  steros
 * Added GetDriverPid function for driver pid retrival from MPS.
 *
 * Revision 1.5  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.4  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 * Revision 1.3  2001/03/10 17:08:48  steros
 * Added header and lib functions to interface driver to MPS.
 *
 * Revision 1.2  2001/01/11 10:04:29  steros
 * Fixed headers.
 *
 *
 ***************************************************************************/


#include <printf.h>
#include <messages.h>
#include <processes.h>
#include <malloc.h>
#include <drivers/protocols/mpsint.h>
#include <drivers/protocols/drv2mps.h>
#include <drivers/protocols/prg2mps.h>

#include "mps_boot.h"
#include "mps_error.h"
#include "mps_struct.h"


//#define DEBUG_MSG     /* Enable debug printing. */


#ifdef DEBUG_MSG
  #define DBGprintf(a...); { printf("[MPS:%s:%u] ",__FILE__,__LINE__); printf(a); }
#else
  #define DBGprintf(a...);
#endif


mps_msg_type *msg;
msginfo info;



/*---------------------------------------------------------------------*/
/*                               Checks.                               */
/*---------------------------------------------------------------------*/

/*
 Restituisce un messaggio di errore.
*/
void ReplyError(MpsRetVal err) {
  msg->Version=DRV2MPS_PROTOCOL_VERSION;  /* Already present! */
  msg->Release=DRV2MPS_PROTOCOL_RELEASE;
  msg->Status=err;
DBGprintf("ReplyError> send_msg: class=%d, pid=%d\n",
          (mps_msg_type*)msg->Class,info.mittente);
  send_msg(info.mittente,msg,sizeof(mps_msg_type));
}


/*
 Check the version.
 Return 0 if version is compatible.
*/
int CheckVersion(int class) {
  if (msg->Version!=
      (class==DRVMPS_CLASS ? DRV2MPS_PROTOCOL_VERSION :
                             PRG2MPS_PROTOCOL_VERSION))
  {
    msg->Class=class;   /* Already present! */
    msg->Version=DRV2MPS_PROTOCOL_VERSION;
    ReplyError(ERRDRV_WRONGVER);
DBGprintf("CheckVersion> Wrong version!\n");
    return 1;
  }
  return 0;
}



/*---------------------------------------------------------------------*/
/*                        Messages management.                         */
/*---------------------------------------------------------------------*/


/*
 Send the AskPid reply message.
*/
void Send_AskPid_Reply(PID program_pid, PID driver_pid) {
  union prgmps_msg *msg;

DBGprintf("Send_AskPid_Reply> prg_pid=%d, drv_pid=%d\n",program_pid,
          driver_pid);
  msg=getbmsg();
  msg->askpid_reply.intest.Class=PRGMPS_CLASS;
  msg->askpid_reply.intest.Version=PRG2MPS_PROTOCOL_VERSION;
  msg->askpid_reply.intest.Release=PRG2MPS_PROTOCOL_VERSION;
  msg->askpid_reply.intest.Status=
   (driver_pid ? ERRDRV_NOERR : ERRDRV_PM_DRVNOTFOUND);
  msg->askpid_reply.intest.Type=PRGMPS_ASKPID_REPLY;
  msg->askpid_reply.pid=driver_pid;
DBGprintf("Send_AskPid_Reply> send_msg: class=%d, pid=%d\n",
          ((mps_msg_type*)msg)->Class,program_pid);
  send_msg(program_pid,msg,sizeof(prgmps_askpid_reply_type));
}



/*---------------------------------------------------------------------*/
/*                     Program messages management.                    */
/*---------------------------------------------------------------------*/

void Program_AskPid(union prgmps_msg *pmsg) {
  prgmps_askpid_type *msg=&(pmsg->askpid);
  PID pid;

DBGprintf("Program_AskPid> ASKPID message received!\n");
DBGprintf("Program_AskPid> Request for category = %s; name = %s, mitt=%d\n",
          msg->category,msg->name,info.mittente);

  // ^^^ This case must be considered. Disabled for now!!!
  if (msg->wait_type==MPS_WAITLOAD) {
    msg->wait_type=MPS_WAITRUN;   //^^^
    // if (!BootRegisterFinished) ?!?
    // RunDriver(msg->category,msg->name,info.mittente);
    // return;   // Postpone the reply.
  }

  pid=FindDriverPid(msg->category,msg->name,
        (msg->wait_type==MPS_WAITRUN ? info.mittente : 0));
  switch (pid) {
    case  0:   /* NOT FOUND */
      if (msg->wait_type==MPS_WAITRUN && !BootRegisterFinished) {
        if (Boot_WaitForDriver(info.mittente,msg->category,msg->name)) {
DBGprintf("Program_AskPid> Driver not found.\n");
          ReplyError(ERRDRV_PM_DRVNOTFOUND);
        } else {
DBGprintf("Program_AskPid> Driver loading.\n");
        }
      } else {
DBGprintf("Program_AskPid> Driver not found.\n");
        ReplyError(ERRDRV_PM_DRVNOTFOUND);
      }
      break;
    case -1:   /* IN LOADING: POSTPONE THE REPLY */
DBGprintf("Program_AskPid> Driver loading.\n");
      break;
    default:   /* FOUND */
DBGprintf("Program_AskPid> Driver found with pid = %d\n",pid);
      /* Reply. */
      Send_AskPid_Reply(info.mittente,pid);
  }
DBGprintf("Program_AskPid> End\n");
}



/*---------------------------------------------------------------------*/
/*                     Driver messages management.                     */
/*---------------------------------------------------------------------*/

void Driver_Init(union drvmps_msg *dmsg) {
  MpsRetVal err;

DBGprintf("Driver_Init> INIT message received!\n");
DBGprintf("Driver_Init> (%s/%s), pid=%d, driver: %d.%d, protocol: %d.%d\n",dmsg->init.category,dmsg->init.name,info.mittente,dmsg->init.driver_version,dmsg->init.driver_release,dmsg->init.protocol_version,dmsg->init.protocol_release);
DBGprintf("Driver_Init> (%s/%s), description: %s\n",dmsg->init.category,dmsg->init.name,dmsg->init.description);

  /* Update structures. */
  err=AddNewDriver(dmsg->init.category,dmsg->init.name,info.mittente,
                   dmsg->init.multi_instances,dmsg->init.driver_version,
                   dmsg->init.driver_release,dmsg->init.description);
  if (err) {
    ReplyError(err);
    return;
  }

  /* Update boot-time structures if necessary. */
//  Boot_RegisterDriver(dmsg->init.category,dmsg->init.name,info.mittente);

  freemsg(dmsg);
}


/*
 Reply to a driver init message.
*/
void Driver_Init_Reply(void) {
  union drvmps_msg *dmsg;

  dmsg=getbmsg();
  dmsg->init_reply.intest.Class=DRVMPS_CLASS;
  dmsg->init_reply.intest.Version=DRV2MPS_PROTOCOL_VERSION;
  dmsg->init_reply.intest.Release=DRV2MPS_PROTOCOL_VERSION;
  dmsg->init_reply.intest.Status=ERRDRV_NOERR;
  dmsg->init_reply.intest.Type=DRVMPS_INIT_REPLY;
  dmsg->init_reply.reply=0;
DBGprintf("Driver_Init> send_msg: class=%d, pid=%d\n",
          ((mps_msg_type*)dmsg)->Class,info.mittente);
  send_msg(info.mittente,dmsg,sizeof(drvmps_init_reply_type));
}


void Driver_Term(void) {
  MpsRetVal err;

DBGprintf("Driver_Term> pid=%d\n",info.mittente);

  /* Update structures. */
  err=DeleteInstance(info.mittente);
  if (err) {
    ReplyError(err);
    return;
  }
}



/*---------------------------------------------------------------------*/
/*                            Main function.                           */
/*---------------------------------------------------------------------*/

int main(void) {
DBGprintf("Started with PID=%d!\n",getpid());
DBGprintf("Waiting START message...\n");
  if (Set_MPS_Pid(getpid())!=0) {
    printf(" **************** *** MPS *** Error declaring my pid ********\n");
  }
  msg=recv_msg(&info);
  if (((union drvmps_msg *)msg)->intest.Type!=DRVMPS_START) {
    printf("[MPS] ***** ERROR: message received not START!!!\n");
    while (1);
  }
DBGprintf("Ok. START message arrived!\n");
  InitStruct();
  while (1) {
DBGprintf("Waiting for a message...\n");
    msg=recv_msg(&info);
    switch (msg->Class) {
      case MPS_CLASS:
        {
          mps_msg_t *mmsg=(mps_msg_t *)msg;

DBGprintf("MPS_CLASS message arrived!\n");
          switch (mmsg->type) {
            case MPS_WAITFORDRIVER:
DBGprintf("MPS_CLASS:WAITFORDRIVER %d.\n",mmsg->pid);
              Boot_AddDriver(mmsg->pid);
              break;
            case MPS_INIT_COMPLETE:
DBGprintf("MPS_CLASS:INIT_COMPLETE.\n");
              Boot_Complete();
              break;
            case MPS_DEBUG_REQUEST:
              DBGViewData();
              mmsg->Class=MPS_CLASS;
              mmsg->type=MPS_DEBUG_REQUEST_REPLY;
              mmsg->pid=0;
              send_msg(info.mittente,mmsg,sizeof(mps_msg_t));
              break;
            default:
              ErrorMsg(ERR_UNKNOWN_MPS);
          }
          freemsg(msg);
        }
        break;
      case DRVMPS_CLASS:
        {
          union drvmps_msg *dmsg=(union drvmps_msg *)msg;

DBGprintf("DRVMPS msg arrived (ver=%d, rel=%d, status=%d, type=%d, pid=%d, dim=%d)!\n",msg->Version,msg->Release,msg->Status,dmsg->intest.Type,info.mittente,info.dimensione);
          if (!CheckVersion(DRV2MPS_PROTOCOL_VERSION)) {
            switch (dmsg->intest.Type) {
              case DRVMPS_INIT:
                Driver_Init(dmsg);
                break;
              case DRVMPS_INIT_REPLY:
                ErrorMsg(ERR_WRONGTYPE);
                break;
              case DRVMPS_TERM_NOTIFY:
                Driver_Term();
                break;
              default:
                ErrorMsg(ERR_UNKNOWN_DRV);
            }
          }
        }
	      break;
      case PRGMPS_CLASS:
        {
          union prgmps_msg *pmsg=(union prgmps_msg *)msg;

DBGprintf("PRG->MPS CLASS message arrived (ver=%d, rel=%d, status=%d)!\n",msg->Version,msg->Version,msg->Release,msg->Status);
  	      if (!CheckVersion(PRG2MPS_PROTOCOL_VERSION)) {
    	      switch (pmsg->intest.Type) {
              case PRGMPS_ASKPID:
                Program_AskPid(pmsg);
                break;
              case PRGMPS_ASKPID_REPLY:
                ErrorMsg(ERR_WRONGTYPE);
                break;
              default:
                ErrorMsg(ERR_UNKNOWN_PRG);
            }
DBGprintf("PRG->MPS CLASS message processed!\n");
          }
        }
	      break;
      default:
        ErrorMsg(ERR_UNKNOWN_CLASS);
    }
  }
DBGprintf("End!\n");
}
