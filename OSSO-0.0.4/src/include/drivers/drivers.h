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


/********************** Driver's LIB O S S O *******************************
 * file : drivers/lib/include/drivers.h                                    *
 * Definitions for Routines for drivers                                    *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: drivers.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: drivers.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.3  2000/08/25 13:39:37  iurlano
 * Added CHANGELOG
 * more translations
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:03  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#ifndef __DRIVERS_DRV_LIB
#define __DRIVERS_DRV_LIB

#include <types.h>
#include <semaphores.h>
//#include <devices.h>

SemaphoreAddress get_HW_Sem(dword irq_number);
void Print_proc_info(void);
//void Register_Driver(dword class,dword irq,dword type);
//void Get_Device_Class(dword class,DeviceClass * dc);

void Video_Scroll_UP(void);
void Video_Scroll_DOWN(void);


#endif
