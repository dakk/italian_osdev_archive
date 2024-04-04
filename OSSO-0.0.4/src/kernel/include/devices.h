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
 * file : include/devices.h
 * Descrizione: definizione dei dispositivi
 ***************************************************************************

 ***************************************************************************
 * $Id: devices.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: devices.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:05  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:40  pecciant
 *
 *
 * Revision 1.3  2000/01/17 23:44:41  iurlano
 * Aggiunto INIT
 *
 * Revision 1.2  2000/01/16 22:43:31  iurlano
 * Aggiunta tabella dei drivers e relative funzioni.
 * da testare
 *
 * Revision 1.1  1999/12/23 10:14:39  iurlano
 * *** empty log message ***
 *
 *
 ***************************************************************************/

#ifndef __DEVICES_DEFS
#define __DEVICES_DEFS


#include <types.h>

/* Device classes definitions */
#define DEV_HDC         1  /* Hard Disk Controller */
#define DEV_FILESYSTEM  2
#define DEV_SERIAL      3
#define DEV_PARALLEL    4
#define DEV_KEYBOARD    5
#define DEV_MOUSE       6
#define DEV_MONITOR     7
#define DEV_PRINTER     8
#define DEV_MODEM       9
#define DEV_AUDIO      10    /* Eh Eh Eh ... */

#define DEV_CLASSES 10

/* Device types definitions */
#define DEV_CHAR_DEV   1
#define DEV_BLOCK_DEV  2

typedef struct __Device {
  PID pid;
  word type;
  byte irq; /* 0 = no irq */
  byte active;
} Device;

#define MAX_DEVICES_PER_CLASS 10
typedef Device DeviceClass[MAX_DEVICES_PER_CLASS];

extern DeviceClass DEVICES[DEV_CLASSES];

void KRegister_Driver(dword class,dword irq,dword type);
void KGet_Device_Class(dword class,DeviceClass * dc);

#endif



