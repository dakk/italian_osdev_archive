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
 * file : include/coda_pronti.h
 * Description: Definitions of coda_pronti management routines
 ***************************************************************************

 ***************************************************************************
 * $Id: coda_pronti.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: coda_pronti.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.1  2001/03/02 17:56:13  iurlano
 * changed an ld flag in the kernel Makefile. COULD BE A PROBLEM!
 * some reordering
 *
 *
 ***************************************************************************/

#ifndef __CODAPRONTI_DEFS
#define __CODAPRONTI_DEFS

#include <types.h>
#include <constants.h>

/* Queue functions */

extern Thread_idx CodaPronti[MAX_REFERENCES];

int   Add_Thread_To_CodaPronti(Thread_idx tid);
Thread_idx Get_Thread_From_CodaPronti(void);
int   Remove_Thread_From_CodaPronti(Thread_idx tid);

#endif
