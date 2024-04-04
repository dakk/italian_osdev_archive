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


/************************* Driver's LIB O S S O ****************************
 * file : drivers/lib/drivers.c                                            *
 * Routines for drivers                                                    *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: drivers.c,v 1.8 2001/04/28 12:15:52 steros Exp $
 ***************************************************************************
 *
 * $Log: drivers.c,v $
 * Revision 1.8  2001/04/28 12:15:52  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.7  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.6  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.5  2001/02/15 21:04:31  iurlano
 * Restore keyboard functionality (expecting MarcoPex to finish his work)
 * Some Bug fixes.
 *
 * Revision 1.4  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.3  2000/10/23 23:33:44  iurlano
 * Remove ebx references from the libOSSO
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:02  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.2  2000/03/22 23:16:02  iurlano
 * Correzioni drivers
 *
 * Revision 1.1.1.1  2000/03/06 15:53:43  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:40  pecciant
 *
 *
 * Revision 1.3  2000/01/16 22:43:31  iurlano
 * Aggiunta tabella dei drivers e relative funzioni.
 * da testare
 *
 * Revision 1.2  2000/01/13 13:43:46  iurlano
 * *** empty log message ***
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
#include <drivers/drivers.h>

SemaphoreAddress get_HW_Sem(dword irq_number) {
  SemaphoreAddress ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_GET_HW_SEM), "c" (irq_number));
  return ret;
}

void Print_proc_info(void) {
  __asm__ ("int $0x40"::"a" (DRV_PRINT_PROC_INFO));
}

/*
  void Register_Driver(dword class,dword irq,dword type) {
  __asm__("pushl %%ebx\n"
  "movl %%edi,%%ebx\n"
  "int $0x40\n"
  "popl %%ebx"::"a" (DRV_REGISTER),"D" (class),"c" (irq),"d" (type));
  }

  void Get_Device_Class(dword class,DeviceClass * dc) {
  __asm__("int $0x40"::"a" (DRV_GET_CLASS),"c" (class),"d" ((dword)dc));
  }

*/

void Video_Scroll_UP(void) {
  __asm__ ("int $0x40"::"a" (DRV_SCROLL_UP));
}

void Video_Scroll_DOWN(void) {
  __asm__ ("int $0x40"::"a" (DRV_SCROLL_DOWN));
}
