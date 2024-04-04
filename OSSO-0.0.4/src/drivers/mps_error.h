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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/mps_error.h,v $
 * Description: Modules Pid Server: error management header.
 ***************************************************************************

 ***************************************************************************
 * $Id: mps_error.h,v 1.2 2001/07/30 17:07:29 steros Exp $
 ***************************************************************************
 *
 * $Log: mps_error.h,v $
 * Revision 1.2  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.1  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 *
 ***************************************************************************/


#ifndef _MPS_ERROR_H_
#define _MPS_ERROR_H_


typedef enum {
/*  0 */  ERR_NO_MEM,
/*  1 */  ERR_WRONGTYPE,
/*  2 */  ERR_UNKNOWN_MPS,
/*  3 */  ERR_UNKNOWN_DRV,
/*  4 */  ERR_UNKNOWN_PRG,
/*  5 */  ERR_UNKNOWN_CLASS,
/*  6 */  ERR_TOOMANY_BOOT
} ErrorType;



void ErrorMsg(ErrorType e);


#endif
