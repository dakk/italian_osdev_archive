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
 * file : $Source: /cvsroot/osso/OSSO/src/test/mpscom.c,v $
 * Description: MPS Commander: interface to command section.
 ***************************************************************************

 ***************************************************************************
 * $Id: mpscom.c,v 1.1 2001/07/30 17:07:29 steros Exp $
 ***************************************************************************
 *
 * $Log: mpscom.c,v $
 * Revision 1.1  2001/07/30 17:07:29  steros
 * MPS multiple copies drivers implemented.
 *
 *
 ***************************************************************************/



#include <printf.h>

#include <processes.h>
#include <memory.h>
#include <messages.h>

#include "../Config_Global.h"

#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>
#include <protocols/prg2mps.h>


extern PID MPS_Pid;



typedef struct {
  char *str;
  void *(*f_pointer)(char *);
  char *help;
} Comandi_t;


void *Com_help(char *s);
void *Com_exit(char *s);
void *Com_debug(char *s);

Comandi_t Comandi[] = {
  { "help",  Com_help,  "Print this help."                      },
  { "exit",  Com_exit,  "Exit from MPS Commander."              },
  { "debug", Com_debug, "Make MPS print it's structures state." },
  { 0, 0, 0 }   // End of list. Don't touch!
};


char get_char(void);


Comandi_t *Comando(char **s) {
  Comandi_t *tc=Comandi;
  char *p, *sp;

  for (; tc->str; tc++) {
    for (p=tc->str, sp=*s; *p && *p==*sp; p++, sp++);
    if (!*p && (*sp==' ' || *sp=='\t' || *sp=='\n' || *sp=='\0')) {
      while (*sp==' ' || *sp=='\t') sp++;
      *s=sp;
      return tc;
    }
  }
  return NULL;
}



void *Com_help(char *s) {
  Comandi_t *tc=Comandi;

  printf("\nAvailable commands:\n");
  for (; tc->str; tc++) {
    printf("%s -> %s\n",tc->str,tc->help);
  }
  printf("\n");
  return NULL;
}


void *Com_exit(char *s) {
  exit();
}


void *Com_debug(char *s) {
  mps_msg_t *msg;
  msginfo info;

  msg=getbmsg();
  msg->Class=MPS_CLASS;
  msg->type=MPS_DEBUG_REQUEST;
  msg->pid=0;
  send_msg(MPS_Pid,msg,sizeof(mps_msg_t));
  msg=recv_msg(&info);
  //freeosso(msg);
  return NULL;
}



int main(void) {
  char riga[500]="", *rigap;
  Comandi_t *com;
  void *p;
  int rigaidx;
  dword ret;
  char c;

#ifdef LINE_BUFFERED_OUTPUT
  LineBuffered=0;           // THIS DOESN'T WORK!!!
#endif

  /* Delay introduced for bochs */
  /*
    y=12356;
    for (x=0;x<912346;x++) {
    //for (y=0;y<123113;y++)
    rigaidx=y/(x+1);
    }
  */

  printf("MPS Commander .... mypid is %d\n",getpid());


  if ((ret=DriverInit(0))!=0) {
    printf("[SHELL] ***** Error in initialization phase: %s!!!\n",
	   MpsStrerror(ret));
    exit();
  }

  while (1) {
    printf("MPSCom> ");
    printc_flush();
    rigaidx=0;
    while ((c=get_char())!='\n') {
      if (c==127) {
    	  if(rigaidx>0) {
	        riga[--rigaidx]='\0';
	        printf("%c",c);
          printc_flush();
      	}	else /* Beep! */;
      } else {
	      riga[rigaidx++]=c;
	      printf("%c",c);
        printc_flush();
      }
    }
    riga[rigaidx]='\0';
    printf("%c",c);

    for (rigap=riga; *rigap==' ' || *rigap=='\t'; rigap++);
    if (*rigap=='\0') continue;

    if ((com=Comando(&rigap))==NULL)
      printf("Command unknown. Type 'help' for available command.\n");
    else
      p=(*com->f_pointer)(rigap);
  }
  return 0;
}
