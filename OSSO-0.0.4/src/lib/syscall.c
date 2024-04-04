
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
 * file : lib/syscall.c                                                    *
 * System calls                                                            *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: syscall.c,v 1.2 2000/08/03 15:57:44 iurlano Exp $
 ***************************************************************************
 *
 * $Log: syscall.c,v $
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:19  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.2  1999/12/02 15:02:43  iurlano
 *
 * Aggiustato files nella directory lib
 *
 *
 ***************************************************************************/

#include <syscalls.h>
#include <signals.h>

void set_signal_handler(unsigned long int sigg,Signal_Handler kk) {
  __asm__ ("int $0x40"::"a" (SYS_SIGNAL_HANDLER),"b" (sigg),"d" (kk));
}

