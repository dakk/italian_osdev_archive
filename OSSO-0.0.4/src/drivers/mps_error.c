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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/mps_error.c,v $
 * Description: Modules Pid Server: error management.
 ***************************************************************************

 ***************************************************************************
 * $Id: mps_error.c,v 1.2 2001/07/30 17:07:29 steros Exp $
 ***************************************************************************
 *
 * $Log: mps_error.c,v $
 * Revision 1.2  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.1  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 *
 ***************************************************************************/


#include <printf.h>

#include "mps_error.h"


char *ErrorDescr[] = {
/*  0 */  "Memory allocation error",
/*  1 */  "The type of received message is wrong",
/*  2 */  "Unknown MPS message received",
/*  3 */  "Unknown driver message received",
/*  4 */  "Unknown program message received",
/*  5 */  "Unknown message class received",
/*  6 */  "Too many drivers at boot time"
};





void ErrorMsg(ErrorType e) {
  printf("[MPS] *** %s ***!!!\n",ErrorDescr[e]);
}
