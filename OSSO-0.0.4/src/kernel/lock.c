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
 * file : kernel/lock.c
 * Locking function for multiprocessing support
 ***************************************************************************
 

 ***************************************************************************
 * $Id: lock.c,v 1.3 2001/04/10 23:10:26 iurlano Exp $
 ***************************************************************************
 *
 * $Log: lock.c,v $
 * Revision 1.3  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.2  2001/03/06 22:52:11  iurlano
 * Added lock mechanism to P and V. To be tested
 * Added miniman xconfig Make target
 *
 * Revision 1.1  2001/03/06 11:50:26  iurlano
 * Added functions for smp multitasking (not yet tested)
 *
 *
 ***************************************************************************/


#include <types.h>
#include <lock.h>
#include <panic.h>
#include <errors.h>

void lock(dword * address) {
  int i,t,k;
  i=0;
  while (i==0) {
    __asm__ __volatile__ ("xchg %%eax,(%%edx)":"=a"(i):"a"(0), "d" (address));
    for (t=0;t<100;t++) k=k+1;
  }
  if (i!=1) panic("lock returned a non one value!");
}

void unlock(dword * address) {
  int i;
  i=1;
  __asm__ __volatile__ ("xchg %%eax,(%%edx)":"=a"(i):"a"(1), "d" (address));
  if (i!=0) panic("unlock called on a nonzero value!");
}


int lock_non_block(dword * address) {
  int i;
  i=0;
  __asm__ __volatile__ ("xchg %%eax,(%%edx)":"=a"(i):"a"(0), "d" (address));
  if (i!=1) 
    return E_SHOULD_BLOCK;
  else 
    return E_SUCCESS;
}

