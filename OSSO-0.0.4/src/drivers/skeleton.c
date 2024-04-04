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
 * file : drivers/skeleton.c                                               *
 * Driver skeleton                                                         *
 ***************************************************************************

 ***************************************************************************
 * $Id: skeleton.c,v 1.2 2000/08/04 17:47:58 iurlano Exp $
 ***************************************************************************
 *
 * $Log: skeleton.c,v $
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:02  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1  2000/03/21 10:43:17  iurlano
 * Spostati files dalla dir del kernel a quella dei drivers
 *
 ***************************************************************************/

#include <io.h>
#include <printf.h>
#include <processes.h>
#include <semaphores.h>
#include <drivers.h>
#include <messages.h>
#include <protocols/disk.h>
#include <protocols/init.h>
#include <devices.h>

void main(void) {
  char * msg;
  msginfo info;
  PID INIT;

  msg=recv_msg(&info);
  if ((((INIT_COMMAND*)msg)->command)!=INIT_INITIALIZE) {
    printf("Panic : Message not INIT_INITIALIZE\n");
    __asm__ ("hlt");
  }
  INIT=info.mittente;
  printf("Got message from INIT =%d\n",INIT);

  /* PROCESS PARAM STRING */


  /* DRIVER SPECIFIC INIT SEQUENCE */

  msg=getbmsg();
  ((INIT_COMMAND*)msg)->command=INIT_FINISHED;
  send_msg(INIT,msg,sizeof(INIT_COMMAND));


  while (1) {
    printf("Waiting message (zzzz)....\n");
    msg=recv_msg(&info);
    printf("Got Message!\n");
    
    /* Process message and answer */

  }
}
