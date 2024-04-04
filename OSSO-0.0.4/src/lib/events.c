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
 * file : $Source: /cvsroot/osso/OSSO/src/lib/events.c,v $                                                         *
 * Description: Events management routines                                 *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: events.c,v 1.1 2001/04/04 07:04:37 marcopex Exp $
 ***************************************************************************
 *
 * $Log: events.c,v $
 * Revision 1.1  2001/04/04 07:04:37  marcopex
 * Forgot some files...
 *
 *
 ***************************************************************************/

#include <syscalls.h>
#include <types.h>
#include <events.h>

int getevent(char * nome) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_GETEVENT),"c" (nome));
  return ret;
}

int clearevent(EVENT evento) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_CLEAREVENT),"c" (evento));
  return ret;
}

int setevent(EVENT evento, ATTRIBUTES attributi) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_SETEVENT),"c" (evento),"d" (attributi));
  return ret;
}
int signal(PID pid, EVENT evento){
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_ESIGNAL),"c" (pid),"d" (evento));
  return ret;
}
int wait(EVENT evento, PID * pid) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_EWAIT),"c" (evento), "d" (pid));
  return ret;
}


