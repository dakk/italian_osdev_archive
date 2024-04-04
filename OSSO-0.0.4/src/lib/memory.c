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
 * file : lib/memory.c                                                     *
 * Memory managemente routines                                             *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: memory.c,v 1.5 2001/06/26 21:00:41 iurlano Exp $
 ***************************************************************************
 *
 * $Log: memory.c,v $
 * Revision 1.5  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.4  2001/04/23 15:33:19  iurlano
 * work on FAT and fs protocol
 *
 * Revision 1.3  2000/10/23 23:33:44  iurlano
 * Remove ebx references from the libOSSO
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:18  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.2  2000/03/26 13:20:23  iurlano
 * Completamento protocollo di INIT
 * Da completarne il test
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.2  1999/12/21 17:42:05  iurlano
 * *** empty log message ***
 *
 * Revision 1.1  1999/12/17 22:46:06  iurlano
 *
 * I messaggi sembrano funzionare !!!!
 *
 *
 ***************************************************************************/

#include <syscalls.h>
#include <types.h>

void * mallosso(unsigned long dimensione) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_MALLOSSO),"c" (dimensione));
  return (void*)ret;
}

void freeosso(void * ptr) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_FREEOSSO),"c" (ptr));
  return ;
}

int get_free_pages(void) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_GET_MEM_FREE_PAGES));
  return ret;
}

int get_mem_info(void) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_GET_MEM_INFO));
  return ret;
}

