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
 * file : $Source: /cvsroot/osso/OSSO/src/drivers/mps_boot.c,v $
 * Description: Modules Pid Server: boot phase structures and functions.
 ***************************************************************************

 ***************************************************************************
 * $Id: mps_boot.c,v 1.8 2001/08/04 18:22:47 steros Exp $
 ***************************************************************************
 *
 * $Log: mps_boot.c,v $
 * Revision 1.8  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.7  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.6  2001/06/08 18:47:56  steros
 * Bug fix.
 *
 * Revision 1.5  2001/06/03 09:53:17  steros
 * Bug fixes.
 *
 * Revision 1.4  2001/05/03 17:45:23  steros
 * Sorry, last version was wrong.
 *
 * Revision 1.3  2001/05/03 17:24:06  steros
 * Improved MPS boot phase.
 *
 * Revision 1.2  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.1  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 *
 ***************************************************************************/


#include <processes.h>

#include "mps_struct.h"
#include "mps_error.h"


//#define DEBUG   /* Enable debug printing. */

#ifdef DEBUG
  #include <printf.h>
  #define DBGprintf(a...); { printf("[MPS:%s:%u] ",__FILE__,__LINE__); printf(a); }
#else
  #define DBGprintf(a...);
#endif



/* Prototypes for mps.c */
void Send_AskPid_Reply(PID program_pid, PID driver_pid);


#define MAX_BOOT_DRIVER_NUM 32
#define MAX_BOOT_WAIT_QUEUE 32


PID BootDRV[MAX_BOOT_DRIVER_NUM];
int numBootDRV=0;             /* Number of elements in BootDRV. */

int BootDRVremaining=0;       /* Num. of drivers that are executed but
                                 yet registered. */
int BootInitFinished=0;       /* Set if there are no more drivers to
                                 execute. */
int BootRegisterFinished=0;   /* Set when the boot phase is complete:
                                 there are no more drivers to execute
                                 and all drivers are registered. */

typedef struct {
  PID  pid;
  char category[MAX_DRIVER_CATEGORY_LEN];
  char name[MAX_DRIVER_NAME_LEN];
} BootWaitQueue_T;

int NumBootWaiting=0;   /* Number of elements in wait queue. */
BootWaitQueue_T BootWaitQueue[MAX_BOOT_WAIT_QUEUE];





/*
 Put a process in the wait queue of boot.
 Return 0 if all ok.
*/
int Boot_WaitForDriver(PID pid, char *category, char *name) {
  BootWaitQueue_T *elem;

DBGprintf("Boot_WaitForDriver> 1: pid=%d, cat=<%s>, drv=<%s>\n",
          pid,category,name);
DBGprintf("Boot_WaitForDriver> a1: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
  if (BootRegisterFinished) return 1;
  if (NumBootWaiting<MAX_BOOT_WAIT_QUEUE) {
    elem=&(BootWaitQueue[NumBootWaiting]);
    elem->pid=pid;
    strcpy(elem->category,category);
    strcpy(elem->name,name);
    NumBootWaiting++;
DBGprintf("Boot_WaitForDriver> 1b\n");
    return 0;
  }
DBGprintf("Boot_WaitForDriver> a2: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
DBGprintf("Boot_WaitForDriver> 2\n");
  return 1;
}


/*
 Find the processes waiting for a certain driver and send them
 the pid of the driver.
*/
void Boot_UnlockWaitingProc(char *category, char *name, PID pid,
                            PID *waiting, int *num_free) {
  BootWaitQueue_T *elem=BootWaitQueue, *el2;
  int c;

DBGprintf("Boot_UnlockWaitingProc> 1: cat=<%s>, drv=<%s>, pid=%d\n",
          category,name,pid);
DBGprintf("Boot_UnlockWaitingProc> a1: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
  if (BootRegisterFinished) return;
  for (c=0; c<NumBootWaiting; ) {
    while (!strcmp(elem->category,category) &&
           !strcmp(elem->name,name) && c<NumBootWaiting)
    {
      if (!*num_free) return; // ^^^... ERRORE: space finished!!!
      *(waiting++)=elem->pid;
      (*num_free)--;

      /* Shift remaining elements. */
      NumBootWaiting--;
      el2=&(BootWaitQueue[NumBootWaiting]);
      elem->pid=el2->pid;
      strcpy(elem->category,el2->category);
      strcpy(elem->name,el2->name);
    }
    elem++;
    c++;
  }
DBGprintf("Boot_UnlockWaitingProc> a2: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
DBGprintf("Boot_UnlockWaitingProc> 2: waiting=%d, num_free=%d\n",
          *waiting,*num_free);
}


/*
 Add a driver in the list of driver to wait at boot time.
*/
int Boot_AddDriver(PID pid) {
DBGprintf("Boot_AddDriver> 1: pid=%d\n",pid);
DBGprintf("Boot_AddDriver> a1: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
  if (BootRegisterFinished) return 1;
  if (!DriverPidPresent(pid)) {
    if (numBootDRV<MAX_BOOT_DRIVER_NUM) {
      BootDRV[numBootDRV]=pid;
      numBootDRV++;
      BootDRVremaining++;
DBGprintf("Boot_AddDriver> 1a\n");
      return 0;
    } else {
      ErrorMsg(ERR_TOOMANY_BOOT);
DBGprintf("Boot_AddDriver> 1b\n");
      return 1;
    }
  }
DBGprintf("Boot_AddDriver> a2: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
DBGprintf("Boot_AddDriver> 2\n");
  return 0;
}


/*
 Close the boot phase sending the "driver not found" message to all
 programs left in waiting for driver queue.
*/
void Boot_CloseBootPhase(void) {
  BootWaitQueue_T *elem=BootWaitQueue;
  int c=NumBootWaiting;

DBGprintf("Boot_CloseBootPhase> 1\n");
DBGprintf("Boot_CloseBootPhase> a1: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
  for (; c; elem++, c--) Send_AskPid_Reply(elem->pid,0);
  NumBootWaiting=0;
  BootRegisterFinished=1;
  CloseBootPhase();
DBGprintf("Boot_CloseBootPhase> a2: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
DBGprintf("Boot_CloseBootPhase> 2\n");
}


/*
 Set a driver in boot list as registered.
*/
void Boot_RegisterDriver(char *category, char *name, PID pid,
                         PID *waiting, int *num_free) {
  int c;

DBGprintf("Boot_RegisterDriver> 1: cat=<%s>, drv=<%s>, pid=%d\n",
          category,name,pid);
  if (BootRegisterFinished) return;

DBGprintf("Boot_RegisterDriver> a1: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
  /* Search for driver pid in boot list and, if present, remove it. */
  for (c=0; c<numBootDRV && BootDRV[c]!=pid; c++);
  if (c<numBootDRV) {
    BootDRV[c]=0;
    BootDRVremaining--;
  }
  Boot_UnlockWaitingProc(category,name,pid,waiting,num_free);

  if (BootInitFinished && !BootDRVremaining)
    Boot_CloseBootPhase();
DBGprintf("Boot_RegisterDriver> a2: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
DBGprintf("Boot_RegisterDriver> 2: waiting=%d, num_free=%d\n",
          *waiting,*num_free);
}


/*
 Function that closes boot phase.
*/
void Boot_Complete(void) {
DBGprintf("Boot_Complete> 1\n");
DBGprintf("Boot_Complete> a1: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
  BootInitFinished=1;
  if (!BootDRVremaining)
    Boot_CloseBootPhase();
DBGprintf("Boot_Complete> a2: %d, %d, %d, %d\n",numBootDRV,
          BootDRVremaining,BootInitFinished,BootRegisterFinished);
DBGprintf("Boot_Complete> 2\n");
}
