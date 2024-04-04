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
 * file : lib/semaphores.c                                                 *
 * Semaphores Routines                                                     *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: semaphores.c,v 1.5 2001/04/28 12:15:53 steros Exp $
 ***************************************************************************
 *
 * $Log: semaphores.c,v $
 * Revision 1.5  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.4  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.3  2000/10/23 23:33:44  iurlano
 * Remove ebx references from the libOSSO
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:19  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.1  2000/01/06 17:00:03  iurlano
 * Aggiunto albero directory 'drivers'
 * Aggiunte alcune syscalls
 *
 *
 ***************************************************************************/

#include <syscalls.h>
#include <types.h>
#include <semaphores.h>

void Init_Semaphore(SemaphoreAddress Sem,dword value) {
  __asm__ ("int $0x40"::"a" (SYS_INIT_SEM),"c" (Sem),"d" (value));
}

void Init_Bool_Semaphore(Bool_SemaphoreAddress Sem,dword value) {
  __asm__ ("int $0x40"::"a" (SYS_INIT_BOOL_SEM),"c" (Sem),"d" (value));
}

void P(SemaphoreAddress Sem) {
  __asm__ ("int $0x40"::"a" (SYS_P_SEM), "c" (Sem));
}


void V(SemaphoreAddress Sem) {
  __asm__ ("int $0x40"::"a" (SYS_V_SEM), "c" (Sem));
}

void Bool_P(Bool_SemaphoreAddress Sem) {
  __asm__ ("int $0x40"::"a" (SYS_BOOL_P_SEM), "c" (Sem));
}

void Bool_V(Bool_SemaphoreAddress Sem) {
  __asm__ ("int $0x40"::"a" (SYS_BOOL_V_SEM), "c" (Sem));
}

int Non_Block_P(SemaphoreAddress Sem) {
  dword res;
  __asm__ ("int $0x40":"=a" (res):"a" (SYS_NBLK_P_SEM), "c" (Sem));
  return res;
}

int Non_Block_Bool_P(Bool_SemaphoreAddress Sem){
  dword res;
  __asm__ ("int $0x40":"=a" (res):"a" (SYS_NBLK_BOOL_P_SEM), "c" (Sem));
  return res;
}



