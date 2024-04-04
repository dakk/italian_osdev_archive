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


/*************************** LIB O S S O ***********************************
 * file : lib/include/messages.h                                           *
 * Header for messages                                                     *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: messages.h,v 1.8 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: messages.h,v $
 * Revision 1.8  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.7  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.3  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:20  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.3  2000/03/20 23:12:56  iurlano
 * Correzione ricezione messaggi
 *
 * Revision 1.2  1999/12/17 22:46:06  iurlano
 *
 * I messaggi sembrano funzionare !!!!
 *
 * Revision 1.1  1999/12/17 17:26:58  iurlano
 * Aggiunte alcune syscall, tipo getpid
 * Lavoro su sistema di messaggi
 * Migliorata libreria
 *
 *
 ***************************************************************************/

#ifndef __MESSAGES_LIB
#define __MESSAGES_LIB

#include <types.h>

typedef
struct __msg_info {
  PID mittente;     /* Sender */
  size_t dimensione; /* Size */
} msginfo;


int send_msg(dword dest,void * buf,dword dim);
void * recv_msg(msginfo * ppp);
void * getbmsg(void);
int freemsg(void * msg);
#endif
