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
 * file : $Source: /cvsroot/osso/OSSO/src/lib/tty.c,v $
 * First attempt of a VERY basic tty approach
 ***************************************************************************

 ***************************************************************************
 * $Id: tty.c,v 1.13 2001/07/08 11:12:51 steros Exp $
 ***************************************************************************
 *
 * $Log: tty.c,v $
 * Revision 1.13  2001/07/08 11:12:51  steros
 * Converted MPS to dynamic allocation and other little fix.
 *
 * Revision 1.12  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.11  2001/05/30 10:56:33  iurlano
 * bugfixes
 *
 * Revision 1.10  2001/05/27 20:22:53  iurlano
 * Added experimental driver config feature
 *
 * Revision 1.9  2001/05/27 17:33:36  steros
 * Added other log & serial features.
 * Logging and serial still have strange behaviour and possible bugs...
 *
 * Revision 1.8  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.7  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.6  2001/04/28 12:15:53  steros
 * OOPS: corrected stupid commit description!!!
 *
 * Revision 1.5  2001/04/28 11:30:40  steros
 * Rearranging tree: committing new directory include files.
 *
 * Revision 1.4  2001/04/22 13:17:38  iurlano
 * Changed mpstest
 * mps now working
 * integrated some code for pid asking in the libosso
 *
 * Revision 1.3  2001/04/19 15:17:58  iurlano
 * Fixed some bug
 * changed ide driver
 * changed fat driver (cabled the pid of the ide driver)
 * changed shell( now accept 3 commands mem, info, ls) TRY IT
 * VERY STABLE SNAPSHOT
 *
 * Revision 1.2  2001/04/18 21:39:43  iurlano
 * Fixed a very insidious memory allocation bug in (getbmsg())
 * little change in xconfig
 * fixed stability problems
 * added two programs for testing message passing
 * Added freemsg()
 * no memory leak now on message passing
 * the shell seems to work!!
 * A strange problem: bochs reports freepage page already freed where the real pc and vmware do not!
 *
 * Revision 1.1  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 *
 ***************************************************************************/

#include <messages.h>
#include <printf.h>

#include <drivers/protocols/mpsint.h>
#include <drivers/protocols/prg2mps.h>

#include "../Config_Global.h"


//typedef unsigned long int dword;
/*
extern int ShareInt;

int libShareInt;

void printShareInt(void) {
  printf("LibOSSO: ShareInt val is %d\n",ShareInt);
  libShareInt=9876;
  printf("LibOSSO: Setting libShareInt val to %d\n",libShareInt);
  printf("LibOSSO: Address of libShareInt is %x",&libShareInt);
  printf("LibOSSO: Address of ShareInt is %x\n",&ShareInt);
}
*/

#ifdef DYNAMIC_MPS_PID
  extern unsigned long MPS_Pid;
#endif

static dword keyboard_pid=-1;


void Find_Keyboard_Pid(void) {
  MpsRetVal errcode;
#ifdef DYNAMIC_MPS_PID
//  printf("<***** tty: MPS pid = %d *****>\n",MPS_Pid);
#endif

#ifdef SERIAL_INPUT
  errcode=GetDriverPid("io_device","serial",&keyboard_pid,MPS_WAITRUN);
#else
  errcode=GetDriverPid("io_device","keyboard",&keyboard_pid,MPS_WAITRUN);
#endif
  if (errcode) {
    printf("[tty.c] ***** Error in getting input device pid: %s!!!\n",
	   MpsStrerror(errcode));
    exit();
  }
}

char get_char(void) {
  char * msg;
  char * ans_msg;
  char c;
  msginfo mi;

  if (keyboard_pid==-1) Find_Keyboard_Pid();
  msg=getbmsg();
  *msg='K';
  send_msg(keyboard_pid,msg,1);
  ans_msg=recv_msg(&mi);
  c=*ans_msg;
  freemsg(ans_msg);
  return c;
}

