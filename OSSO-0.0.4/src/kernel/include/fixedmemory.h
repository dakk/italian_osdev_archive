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


/*********************************** OSSO ************************************
 *  File: $Source: /cvsroot/osso/OSSO/src/kernel/include/fixedmemory.h,v $
 *  Memory header file: Costants for kernel memory addresses
 *****************************************************************************/

/*****************************************************************************
 *
 * $Id: fixedmemory.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 *
 *****************************************************************************
 *
 * $Log: fixedmemory.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.3  2000/10/16 14:12:16  iurlano
 * Translation in progress
 *
 *
 *****************************************************************************/

#ifndef __FIXMEM_DEFS
#define __FIXMEM_DEFS

#include <types.h>
#include <sys.h>
#include <gdt.h>

#define IDT_BASE_ADDRESS ((byte *)0x0)
#define IDT_ENTRIES 0x50

#define GDT_BASE_ADDRESS ((byte *)0x1000)
#define GDT_ENTRIES 256

#define KERNEL_Page_Dir_BASE_ADDRESS ((byte*)0x2000)

#define KERNEL_STACK_BASE ((byte*)0x90000)
#define KERNEL_STACK_TOP ((byte*)0x9FFF0)

#define BASE_PAGE_ADDR 0x100000 /* start address of paged memory */

#endif






