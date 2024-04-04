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
 * file : kernel/devices.c
 * definizione dei dispositivi
 ***************************************************************************

 ***************************************************************************
 * $Id: devices.c,v 1.1.1.1 2000/05/30 08:11:08 iurlano Exp $
 ***************************************************************************
 *
 * $Log: devices.c,v $
 * Revision 1.1.1.1  2000/05/30 08:11:08  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.2  2000/01/16 22:43:32  iurlano
 * Aggiunta tabella dei drivers e relative funzioni.
 * da testare
 *
 * Revision 1.1  1999/12/23 10:14:42  iurlano
 * *** empty log message ***
 *
 *
 ***************************************************************************/


#include <devices.h>
#include <process.h>
#include <panic.h>
#include <printk.h>

DeviceClass DEVICES[DEV_CLASSES];

void KRegister_Driver(dword class,dword irq,dword type) {
  int i;
  DeviceClass * dc;
  dc=&(DEVICES[class]);
  printk("REGISTER DRIVER!\n");
  for (i=0;i<MAX_DEVICES_PER_CLASS;i++)
    if ((*dc)[i].active==0) {
      (*dc)[i].active=1;
      (*dc)[i].pid=Processo_In_Esecuzione->pid;
      (*dc)[i].irq=irq; /* Da rivedere concettualmente */
      (*dc)[i].type=type;
      return ;
    }
  panic("MAX_DEVICES_PER_CLASS reached.");
}

void KGet_Device_Class(dword class,DeviceClass * dc) {
  int i;
  for (i=0;i<MAX_DEVICES_PER_CLASS;i++)
    (*dc)[i]=DEVICES[class][i];
}
