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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/messages.h,v $
 * Description: Messages definitions
 ***************************************************************************

 ***************************************************************************
 * $Id: messages.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: messages.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.5  2001/04/26 22:09:09  iurlano
 * fat work (again)
 * working snapshot ?
 *
 * Revision 1.4  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.3  2000/12/07 11:24:20  marcopex
 * try to compile...
 *
 * Revision 1.2  2000/10/16 14:12:16  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#ifndef __DEF__MSG__
#define __DEF__MSG__ 

#include <types.h>
#include <constants.h>


/* Should be optimized for memory alignment */
typedef 
struct __MSG_HEADER
{
  PID mittente;
  size_t dimensione;
  byte * pagina_fisica;
} MSG_HEADER;

typedef
struct __msg_info {
  PID mittente;
  size_t dimensione;
} msginfo;

#define MAX_MESSAGES (DIM_PAGE/sizeof(MSG_HEADER))

#define NO_BLOCK 100

void * getbmsg(void);
int sendmsg(PID Destinatario,size_t Dimensione,void * MsgAddr);
void * recmsg(int flags,msginfo * ppp);

int freemsg(void * msg);

/* PROPOSTA DI PROTOCOLLI NUOVI

ask();
reply();

receive/receivefrom/nbreceive (volendo e' la stessa a - di parametri)

send (ha un senso a questo punto prevederla bloccante?
l'unico che mi viene e' che potrebbe servire per sapere qundo il
messaggio viene ricevuto, ma e' una banale sincronizzazione che si
puo' fare ad esempio con eventi, l'unica cosa si fa prima: ha un 
senso?)

getmesg
freemesg




 */

#endif

