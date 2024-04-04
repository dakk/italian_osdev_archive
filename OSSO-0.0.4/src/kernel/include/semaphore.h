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
 * file : include/semaphore.h                                              *
 * Semaphore Prototypes                                                    *
 ***************************************************************************


 ***************************************************************************
 * $Id: semaphore.h,v 1.2 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: semaphore.h,v $
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:24:57  steros
 * Rearranging directory: added kernel/include files.
 *
 * Revision 1.10  2001/04/05 21:06:13  iurlano
 * Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
 *
 * Revision 1.9  2001/04/05 14:10:54  iurlano
 * xconfig changes
 *
 * Revision 1.8  2001/04/03 17:14:26  iurlano
 * work on dynamic semaphores
 *
 * Revision 1.7  2001/04/03 11:42:13  iurlano
 * Added a #define for Dynamic Semaphores
 *
 * Revision 1.6  2001/04/03 07:56:14  iurlano
 * Dynamic Semaphores. Alpha code.
 *
 * Revision 1.5  2001/03/12 15:09:54  marcopex
 * Finished (maybe) the compilation of Events
 *
 * Revision 1.4  2001/03/06 22:52:11  iurlano
 * Added lock mechanism to P and V. To be tested
 * Added miniman xconfig Make target
 *
 * Revision 1.3  2000/12/06 21:08:50  iurlano
 * Thread work in progress
 * Include files work
 * Cut some warnings
 *
 * Revision 1.2  2000/11/09 14:30:31  iurlano
 * Added pci.c module for accessing pci with configuration mechanism 1
 *    without BIOS
 *
 * Revision 1.1.1.1  2000/05/30 08:11:07  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/


#ifndef __SEMAPHORE__DEFS
#define __SEMAPHORE__DEFS

#include "../../Config_Kernel.h"

#include <types.h>
#include <constants.h>



typedef struct __Semaphore {
  dword lock;
  dword Status;
#ifdef DYNAMIC_SEMAPHORE
  Thread_idx Head_Thread,Tail_Thread;
#else
  Thread_idx Processi[SEM_MAX_PROCESSES]; // Queue of sleeping processes 
  dword head,tail;
#endif
} Semaphore;

typedef struct __Bool_Semaphore {
  dword lock;
  dword Status;
#ifdef DYNAMIC_SEMAPHORE
  Thread_idx Head_Thread,Tail_Thread;
#else
  Thread_idx Processi[SEM_MAX_PROCESSES]; /* Queue of sleeping processes */
  dword head,tail;
#endif
} Bool_Semaphore;

void Init_Semaphore(Semaphore * Sem,dword value);
void Init_Bool_Semaphore(Bool_Semaphore * Sem,dword value);

void P(Semaphore * Sem);
void V(Semaphore * Sem);

void Bool_P(Bool_Semaphore * Sem);
void Bool_V(Bool_Semaphore * Sem);

int Non_Block_P(Semaphore * Sem);
int Non_Block_Bool_P(Bool_Semaphore * Sem);

#endif





