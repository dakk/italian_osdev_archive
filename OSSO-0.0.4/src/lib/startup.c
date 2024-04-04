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
 * file : lib/startup.c                                                    *
 * Process startup routines                                                *
 ***************************************************************************

 ***************************************************************************
 * $Id: startup.c,v 1.11 2001/08/23 19:21:11 iurlano Exp $
 ***************************************************************************
 *
 * $Log: startup.c,v $
 * Revision 1.11  2001/08/23 19:21:11  iurlano
 * Changed IDE to publish correctly each partition, whole disk included.
 * Added mount point support to vfs. Extended its protocol.
 * Changed fat driver to accept two mandatory parameters: device (the partitions with the fs) and mount (the mount point)
 * Now it is possible to mount a partition and actually see it int the FS!!
 * Works!
 * Changed fs.c in libOSSO to use the vfs for dir and file access.
 * Changed mount utility to inspect partitions and mount points state.
 * Added Documentation for FAT driver
 *
 * Revision 1.10  2001/06/26 21:21:21  iurlano
 * Started to implement vfs support in lib/fs.c
 *
 * Revision 1.9  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.8  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.7  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.6  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
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
 * Revision 1.4  2000/11/02 23:35:38  iurlano
 * Dynamic linking seems to work!!
 * Some problems at boot stage with files with extension
 *
 * Revision 1.3  2000/11/01 23:23:52  iurlano
 * Work on dynamic linking
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:19  iurlano
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
 * Revision 1.3  1999/12/30 15:33:00  iurlano
 * Sistemati processi
 *
 * Revision 1.2  1999/12/25 11:13:39  iurlano
 * *** empty log message ***
 *
 * Revision 1.1  1999/12/02 15:03:37  iurlano
 *
 * Aggiunto file per l'inizializzazione dei processi utente
 *
 *
 ***************************************************************************/

#include <printf.h>
#include <syscalls.h>

#include <processes.h>

unsigned long MPS_Pid=-1;

int main(void);

void _start(void) __attribute__ ((noreturn));
void   exit(void) __attribute__ ((noreturn));

void Init_Fs_Data(void);

void exit(void) {
  __asm__ ("int $0x40"::"a" (SYS_EXIT));
}

void _start(void) {
  printf("Process %d Starting Up [getting mps pid]...\n",getpid());
  //  while (MPS_Pid==-1) {
  MPS_Pid=Get_MPS_Pid();
  //  }
  printf("MPS Pid = %d...\n",MPS_Pid);
  Init_Fs_Data();
  main();
  printf("Process %d Finished\n",getpid());
  
  exit();
  /*    while (1) ; */
  /*    __asm__ ("leave\n" */
  /*  	   "iret\n"); */
}
