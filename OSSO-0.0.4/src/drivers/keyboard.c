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
 * file : drivers/keyboard.c
 * Keyboard driver
 ***************************************************************************

 ***************************************************************************
 * $Id: keyboard.c,v 1.19 2001/08/09 10:37:53 iurlano Exp $
 ***************************************************************************
 *
 * $Log: keyboard.c,v $
 * Revision 1.19  2001/08/09 10:37:53  iurlano
 * Change Creat_thread to accept an integer parameter to be passed to the thread.
 * corrected exit_thread behaviour.
 * Added format routines to the ide (WARNING VERY DANGEROUS and NOT working)
 *
 * Revision 1.18  2001/08/04 18:22:47  steros
 * Added DriverExit. Various MPS fixes.
 *
 * Revision 1.17  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 * Revision 1.16  2001/05/29 22:36:18  iurlano
 * Worked on xconfig tools. Now loads and saves and has new instructions
 * Changed keyboard driver to work with events subsystem
 *
 * Revision 1.15  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.14  2001/04/29 17:40:48  steros
 * Added dynamic mps pid request on drv2mps also.
 * Improved mps and his protocols.
 * There's still problems in taking MPS pid from process MPS_Pid variable.
 *
 * Revision 1.13  2001/04/19 11:19:03  iurlano
 * fixed bugs
 * fixed bochs problem (very strange)
 *
 * Revision 1.12  2001/04/18 21:39:43  iurlano
 * Fixed a very insidious memory allocation bug in (getbmsg())
 * little change in xconfig
 * fixed stability problems
 * added two programs for testing message passing
 * Added freemsg()
 * no memory leak now on message passing
 * the shell seems to work!!
 * A strange problem: bochs reports freepage page already freed where the real pc and vmware do not!
 *
 * Revision 1.11  2001/04/17 18:04:12  iurlano
 * Fixed compilation code
 * Warning: This is a very unstable snapshot
 * There are a lot of strange problems
 *
 * Revision 1.10  2001/04/15 14:15:43  iurlano
 * Fix in drivers/Makefile
 * Added two syscall for mps pid (experimental)
 * Added first attempt of a shell
 * changed and multithreaded keyboard driver
 * changed startup code
 * added libOSSO-special for special processes like mps & INIT
 * added get_char() in libOSSO
 *
 * Revision 1.9  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 * Revision 1.8  2001/03/10 17:08:48  steros
 * Added header and lib functions to interface driver to MPS.
 *
 * Revision 1.7  2001/02/15 21:04:31  iurlano
 * Restore keyboard functionality (expecting MarcoPex to finish his work)
 * Some Bug fixes.
 *
 * Revision 1.6  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.5  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.4  2000/10/25 19:59:38  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
 *
 * Revision 1.3  2000/08/20 16:46:06  iurlano
 * Fixed bug in init in modules loading
 * IDE driver test. Doesn't work with my Quantum Fireball 4.3 Gb but does
 *   work with my Conner 42MB
 * More translation
 * Fixed some bad comments
 * Changed paging.c for a test (see the comments there)
 *
 * Revision 1.2  2000/08/04 17:47:58  iurlano
 * Translation in progress
 *
 *
 ****************************************************************************/

#define __PRINTF_MODULE_NAME__ "KEYBOARD"

#include <printf.h>
#include <semaphores.h>
#include <drivers.h>
#include <types.h>
#include <io.h>
#include <processes.h>
#include <messages.h>
#include <memory.h>
#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>
#include "keyscancodes.h"

#include <semaphores.h>

int ShareInt;

//extern int abs_y; /* printk.c */
//extern Last_Pid_Used;
/* Vector with char states */
unsigned char key_states[256]="";

/* State of modifiers keys */
static int shift=0;
static int control=0;
static int alt=0;

/* Led state */
static int NumLock=1;
static int ScrollLock=0;
static int CapsLock=0;


void UpdateKbdLocks(void) {
  unsigned char data;
  data=(ScrollLock)|(NumLock<<1)|(CapsLock<<2);

  outb(0xed,0x60);
  while (inb(0x64) & 02) ;
  outb(data,0x60);
  while (inb(0x64) & 02) ;
}

#define MAX_CHARS 50

char charqueue[MAX_CHARS];
int charstart,charend;

Semaphore CharBuffer;
Bool_Semaphore CharMutex;

static int KeyPause=0;
static int KeyStampa=0;
static int extended_key=0;

void CharSender(void) {
  msginfo info;
  char * msg;
  char * res_msg;

  while (1) {
    msg=recv_msg(&info);
    //    printf("Received msg from pid %d\n",info.mittente);
    if (*msg=='K') {
      res_msg=getbmsg();
      P(&CharBuffer);
      Bool_P(&CharMutex);
      if (charstart==charend) {
	printf("CODA VUOTA!!! NON PUO' ESSERE|\n");
	Bool_V(&CharMutex);
      } else {
	*res_msg=charqueue[charstart];
	charstart=(charstart+1)%MAX_CHARS;
	Bool_V(&CharMutex);
      }
      send_msg(info.mittente,res_msg,1);
    }
    freemsg(msg);
  }
}


int main(void) {
  //  SemaphoreAddress sem;
  int sem;
  unsigned int pidsem;
  unsigned char a,kstat;
  unsigned char keypressed;
  MpsRetVal ret;


  int CharSender_Tid;

  printf("[KEYBOARD] My pid is %d.\n",getpid());

  /* Start initialization. */
  if ((ret=DriverInit(0))!=0) {
    printf("[KEYBOARD] ***** Error in initialization phase: %s!!!\n",
      MpsStrerror(ret));
    exit();
  }
  //  sem=get_HW_Sem(1);
  sem=getevent("INTERRUPT1");
  if (sem!=0) {
    printf("Errore requesting handling of INTERRUPT1 event!\n");
    exit();
  }
  // Now I should process the parameters... I should...

  charend=0;
  charstart=0;
  Init_Semaphore(&CharBuffer,0);
  Init_Bool_Semaphore(&CharMutex,1);

  /* This ends initialization phase. */
  if ((ret=DriverStart("io_device","keyboard",DRV_I_MONO,0,1,
    "This driver support standard keyboards."))!=0)
  {
    printf("[KEYBOARD] ***** ERROR: MPS don't allow me to register: %s!!!\n",
      MpsStrerror(ret));
    exit();
  }

  printf("[KEYBOARD] Ok. I'm registered to MPS.\n");

  // ... free(msg);

  /* Driver body. */

  CharSender_Tid=Create_Thread(CharSender,0);
  while (1) {
    //printf("[KEYBOARD] waiting for P\n");
    //    P(sem);
    sem=wait("INTERRUPT1",&pidsem);
    if (sem!=0) {
      printf("Error number %d while waiting INTERRUPT1 event\n",sem);
    }
    keypressed=0;
    kstat=inb(0x64); /* Data has to be read */
    if (kstat &1) {
      a=inb(0x60); /* Reading scancode */
      if (a==0xE0) { /* extended Scancode */
	extended_key=1;
      }
      else if (a!=0xFA) { /* 0xFA == Command Acknowledge */
	if (a & 128) key_states[(a & 127)+128*extended_key]=0;
	else key_states[(a & 127)+128*extended_key]=1;

	shift=key_states[kscn_Left_Shift] | key_states[kscn_Right_Shift];
	control=key_states[kscn_Left_Control] | key_states[kscn_Right_Control];
	alt=key_states[kscn_Left_Alt] | key_states[kscn_Right_Alt];

	/* keypressed: */
	switch (a) {
	case 69:
	  NumLock=1-NumLock;
	  UpdateKbdLocks();
	  a=0;
	  break;
	case 70:
	  ScrollLock=1-ScrollLock;
	  UpdateKbdLocks();
	  a=0;
	  break;
	case 58:
	  CapsLock=1-CapsLock;
	  UpdateKbdLocks();
	  a=0;
	  break;
	default:
	  if (extended_key==0) {
	    if (a==69 && KeyPause==2) {
	      KeyPause=0;
	      keypressed=0;
	      printf("PAUSA\n");
	    } else if (a==29 && KeyPause==1) {
	      keypressed=0;
	      KeyPause=2;
	    } else if (a==0xE1) {
	      KeyPause=1;
	      keypressed=0;
	    } else KeyPause=0;
	    if ((a & 128)==0) {
	      if (shift==1)
		keypressed=char_table[a & 127][1];
	      else if (control==1)
		keypressed=char_table[a & 127][2];
	      else if (alt==1)
		keypressed=char_table[a & 127][3];
	      else
		keypressed=char_table[a & 127][0];

	    }
	  } else {
	    keypressed=0;
	    if ((a & 127)==0x37) {
	      if (KeyStampa==1) {
		printf("STAMPA!\n");
		keypressed=0;
	      }
	    }

	    if ((a & 127)==0x2A) {
	      KeyStampa=1;
	      keypressed=0;
	    } else KeyStampa=0;
	    switch (a) {
	    case 0x1C:
	      keypressed=13;
	      break;
	    case 0x2A:
	      KeyStampa=1;
	      keypressed=0;
	      break;
	    case 0x36:
	      keypressed='/';
	      break;
	    case 0x40:
	      printf("Right\n");
	      break;
	    case 0x47: {
	      //int i;
	      Print_proc_info();
	    }
	    break;
	    
	    case 0x48:
	      printf("Free pages : %d\n",get_free_pages());
	      break;
	    case 0x49:
	      if (shift) {
		Video_Scroll_UP();
	      } else
		printf("Page Up\n");
	      break;
	    case 0x4B:
	      printf("Left\n");
	      break;
	    case 0x4F:
	      printf("End\n");
	      break;
	    case 0x50:
	      printf("Down\n");
	      break;
	    case 0x51:	    
	      if (shift) {
		Video_Scroll_DOWN();
	      } else	    
		printf("Page Down\n");
	      break;
	    case 0x52:
	      printf("Insert\n");
	      break;
	    case 0x53:
	      if (shift==0 && control==1 && alt==1) {
		printf("***********REBOOT**********\n\n");
		outb(0xFE,0x64);
		outb(0x1,0x60);
		// __asm__("hlt");
		while (1) ;
	      }
	      else printf("Canc\n");
	      break;

	    }
	  }
	  break;
	}
	if (keypressed!=0) {
	  Bool_P(&CharMutex);
	  if (charstart==(charend+1)%MAX_CHARS) {
	    printf("KEYBOARD Buffer FULL\n");
	    Bool_V(&CharMutex);
	  } else {
	    charqueue[charend]=keypressed;
	    charend=(charend+1)%MAX_CHARS;
	    Bool_V(&CharMutex);	    
	    V(&CharBuffer);
	  }	  
	  // printf("%c",keypressed);
	}
	extended_key=0;
      }
    }
    else printf("No DATA from keyboard\n");

  }
  DriverExit();
  return 0;
}
