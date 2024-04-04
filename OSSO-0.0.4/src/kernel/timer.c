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


/************************************ OSSO *********************************
 *  File: timer.c    				        	           *
 *                                                                         *
 *  Timer header file: functions for time management.                      *
 *  Ps: come mi sta sul culo commentare in inglese...                      *
 ***************************************************************************

 ***************************************************************************
 * $Id: timer.c,v 1.2 2000/08/03 15:57:44 iurlano Exp $
 ***************************************************************************
 *
 * $Log: timer.c,v $
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:17  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.4  1999/11/25 11:22:32  iurlano
 * Pulizia e riordino
 *
 *
 ***************************************************************************/


#include <timer.h>
#include <io.h>
#include <sys.h>

/* extern(??) int scheduletime;  da determinare realmente */
#define scheduletime 50


/* returns timer's actual value */

ctrlwrd operazione;

int getcurrtime (byte channel){

  static byte lsb,msb;
  static int valore; 
 
  operazione.channel=3;
  operazione.bytemode=3;
  operazione.mode=2; /* should not be needed */
  operazione.code=0;

  outb(operazione.valore,ctrlport);
  lsb=inb(ch0port);
  msb=inb(ch0port);

  valore=0;
  valore=valore|(msb << 8);
  valore=valore|lsb;
  
  return valore;
  
}

int setbinterval (byte channel, byte lsb, byte msb, byte mode){

  operazione.channel=channel;
  operazione.bytemode=3;
  operazione.mode=mode;
  operazione.code=0;

  cli();
  outb(operazione.valore,ctrlport);
  outb(lsb,ch0port);
  outb(msb,ch0port);
  sti();
 
  return 1;
}
