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
 * file : lib/processes.c                                                  *
 * Process management routines                                             *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: processes.c,v 1.10 2001/08/13 23:55:28 iurlano Exp $
 ***************************************************************************
 *
 * $Log: processes.c,v $
 * Revision 1.10  2001/08/13 23:55:28  iurlano
 * FDC Work in progress, but needs DMA
 * DMA work started.
 * Some warnings fixed in kernel code
 *
 * Revision 1.9  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.8  2001/06/12 11:02:26  iurlano
 * Added interface fucntion to fat.c fs.c and others (not working)
 *
 * Revision 1.7  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.6  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.5  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.4  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
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
 * Revision 1.3  1999/12/20 17:34:08  iurlano
 * Corretto bug per cui non stampava lo 0
 *
 * Revision 1.2  1999/12/20 13:00:42  iurlano
 * Aggiunta Fork()
 *
 * Revision 1.1  1999/12/17 17:26:58  iurlano
 * Aggiunte alcune syscall, tipo getpid
 * Lavoro su sistema di messaggi
 * Migliorata libreria
 *
 *
 ***************************************************************************/

#include <processes.h>
#include <syscalls.h>
#include <types.h>
#include <fs.h>
#include <memory.h>

/* Determines process identifier
 * Returns calling process' pid 
 */
int getpid(void) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_GETPID));
  return ret;
}

/* Returns calling process' father's pid */
int getppid(void) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_GETPPID));
  return ret;
}

/* Forks current process into father and son */
int fork(void) {
  int ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_FORK));
  return ret;

}

int ThreadStart(void) {
  int tid;
  dword ret;
  Thread_Function start;
  __asm__ ("":"=a" (tid),"=c" (start));
  start(tid);
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_EXIT_THREAD));
  return 0;
}

/* Spawns a thread with a given function and a parameter */
int Create_Thread(Thread_Function function,dword start_value) {
  dword ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_CREATE_THREAD),
	   "c" ((dword)ThreadStart), "d" ((dword)function),"D" (start_value));
  return ret;
}

/* Creates a new process from an ELF buffer */
dword Create_Process_From_ELF(void * buffer,dword level) {
  dword ret;
  __asm__ ("int $0x40":"=a" (ret):"a" (SYS_CREATE_PROCESS_ELF),
	   "c" ((dword)buffer),"d"(level));
  return ret;
}


unsigned long int Get_MPS_Pid(void) {
  unsigned long int a;
  __asm__ ("int $0x40":"=a"(a):"a" (SYS_GETMPSPID));
  return a;
}

int Set_MPS_Pid(unsigned long int pid) {
  unsigned long int ret;
  __asm__ ("int $0x40":"=a"(ret):"a" (SYS_SETMPSPID),"c" (pid));
  return ret;
}

int exec(char * path) {

}
