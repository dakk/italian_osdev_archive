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
 * file : $Source: /cvsroot/osso/OSSO/src/kernel/include/gdt.h,v $
 * Descrizione: Definizioni e prototipi per l'uso delle GDT
 ***************************************************************************

 ***************************************************************************
 * $Id: gdt.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: gdt.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.2  2000/08/13 20:00:33  iurlano
 * Translation in progress.
 * Corrected some warnings.
 *
 * Revision 1.1.1.1  2000/05/30 08:11:06  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.4  1999/12/02 23:09:20  iurlano
 * bug fixes include
 *
 * Revision 1.3  1996/09/12 05:04:44  iurlano
 * Riordino directory test e lib
 * adesso test compila con lib
 * prove di gestione di segnale a livello utente
 *
 * Revision 1.2  1999/11/25 13:08:54  pecciant
 * Standardized headers 
 *
 *
 ***************************************************************************/

#ifndef __GDT_DEFS
#define __GDT_DEFS

#include <types.h>
#include <sys.h>
#include <constants.h>

extern SegmentDes * GDT;

void free_gdt_entry(dword idx);
int get_free_gdt_entries(void);
dword prenote_gdt_entry(void);
void init_gdt(int numdes);

#endif
