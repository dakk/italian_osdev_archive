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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/paging.h,v $
 * System definitions and constants for Paging
 ***************************************************************************/


/***************************************************************************/
/*
 * $Id: paging.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 */
/***************************************************************************/
/*
 * $Log: paging.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.2  2000/12/07 11:24:20  marcopex
 * try to compile...
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:44  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:41  pecciant
 *
 *
 * Revision 1.5  1999/11/25 13:08:56  pecciant
 * Standardizzati gli header dei .h
 *
 * Revision 1.4  1999/11/11 12:18:42  iurlano
 * Cambiate leggermente le funzioni
 *
 * Revision 1.3  1999/10/25 08:09:37  iurlano
 *
 * Inizializzazione paginazione
 *
 * Revision 1.2  1999/10/22 10:41:02  iurlano
 * Correzioni per i bit-field
 *
 * Revision 1.1  1999/10/21 15:38:41  iurlano
 * Aggiustamento commenti
 * Aggiunta header per la paginazione
 *
 */
/***************************************************************************/

#include <types.h>

#ifndef __PAGING_DEFS
#define __PAGING_DEFS

/* Page Directory Entry */
typedef
struct __PDE {
  unsigned Present:1;
  unsigned Read_Write:1;
  unsigned User:1;
  unsigned Write_Through:1;
  unsigned Cache_Disable:1;
  unsigned Accessed:1;
  unsigned Reserved:1;
  unsigned Page_4MB:1;
  unsigned Global:1;
  unsigned Available:3;
  unsigned Page_Table_Base_Addr:20;

} PDE;


#define PDE_Present                   0x1
#define PDE_Read_Write                0x2
#define PDE_User                      0x4
#define PDE_Write_Through             0x8
#define PDE_Cache_Disable             0x10
#define PDE_Accessed                  0x20
#define PDE_Reserved                  0x40
#define PDE_4MB_Page                  0x80
#define PDE_Global                    0x100
#define PDE_Available_Mask            (0x200|0x400|0x800)
#define PDE_Page_Table_Addr_Mask      (0xFFFFF000) 


typedef
struct __PTE {
  unsigned Present:1;
  unsigned Read_Write:1;
  unsigned User:1;
  unsigned Write_Through:1;
  unsigned Cache_Disable:1;
  unsigned Accessed:1;
  unsigned Dirty:1;
  unsigned Reserved:1;
  unsigned Global:1;
  unsigned Available:3;
  unsigned Page_Base_Addr:20;

} PTE;


#define PTE_Present                   0x1
#define PTE_Read_Write                0x2
#define PTE_User                      0x4
#define PTE_Write_Through             0x8
#define PTE_Cache_Disable             0x10
#define PTE_Accessed                  0x20
#define PTE_Dirty                     0x40
#define PTE_Reserved                  0x80
#define PTE_Global                    0x100
#define PTE_Available_Mask            (0x200|0x400|0x800)
#define PTE_Page_Addr_Mask            (0xFFFFF000) 


void init_paging(void);
extern PDE * Kernel_Page_Dir;
void init_PDE(PDE * Page_Dir);

#endif







