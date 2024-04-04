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
 * file : kernel/global_data.c                                             *
 * Global System Data to be used by processes                              *
 ***************************************************************************

 ***************************************************************************
 * $Id: global_data.c,v 1.1 2001/04/15 14:15:43 iurlano Exp $
 ***************************************************************************
 *
 * $Log: global_data.c,v $
 * Revision 1.1  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 *
 ***************************************************************************/



#include <types.h>
#include <sys.h>
#include <errors.h>
#include <process.h>

static PID MPS_Pid=-1;


PID Get_MPS_Pid(void) {
  return MPS_Pid;
}

int Set_MPS_Pid(PID pid) {
  /* Permission check */
  if (Processo_In_Esecuzione->level==SuperVisor_Level) {
    MPS_Pid=pid;
    return E_SUCCESS;
  } else return E_PERM_DENIED;
}


