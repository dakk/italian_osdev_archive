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


/*************************** lib O S S O  ***********************************
 * file : lib/include/processes.h
 * Header for process managemente functions
 ***************************************************************************

 ***************************************************************************
 * $Id: processes.h,v 1.5 2001/08/09 10:37:53 iurlano Exp $
 ***************************************************************************
 *
 * $Log: processes.h,v $
 * Revision 1.5  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.4  2001/06/26 21:21:21  iurlano
 * Started to implement vfs support in lib/fs.c
 *
 * Revision 1.3  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.2  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.1  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.3  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:20  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.4  1999/12/30 15:33:00  iurlano
 * Sistemati processi
 *
 * Revision 1.3  1999/12/20 17:34:32  iurlano
 * Aggiunta getppid()
 *
 * Revision 1.2  1999/12/20 13:00:48  iurlano
 * Aggiunta Fork()
 *
 * Revision 1.1  1999/12/17 17:26:58  iurlano
 * Aggiunte alcune syscall, tipo getpid
 * Lavoro su sistema di messaggi
 * Migliorata libreria
 *
 *
 ***************************************************************************/

#ifndef __PROCESSES_LIB
#define __PROCESSES_LIB

#include <types.h>

int getpid(void);
int getppid(void);

int fork(void);
void exit(void);

typedef void (* Thread_Function)(unsigned int);

int Create_Thread(Thread_Function function,unsigned long int start_value);

dword Create_Process_From_ELF(void *buffer,dword level);

unsigned long int Get_MPS_Pid(void);

int Set_MPS_Pid(unsigned long int pid);


#define MAX_CMD_LINE 20
typedef struct __ps_info {
  dword pid;
  dword ppid;
  dword level;
  dword status;
  char command_line[MAX_CMD_LINE];
} ps_info;

#define SuperVisor_Level 0
#define User_Level       1

#endif
