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
 * file : lib/printc.c                                                     *
 * Print Routines                                                          *
 ***************************************************************************
 
 ***************************************************************************
 * $Id: printc.c,v 1.15 2001/06/24 17:04:39 steros Exp $
 ***************************************************************************
 *
 * $Log: printc.c,v $
 * Revision 1.15  2001/06/24 17:04:39  steros
 * Bugfixes.
 *
 * Revision 1.14  2001/06/17 13:38:00  steros
 * Extended logging fuctionality to boot & kernel messages.
 *
 * Revision 1.13  2001/06/14 12:34:46  iurlano
 * definition of flush_printc() has now a #else in the #ifdef block
 * to let shell compile even with line buffered feature turned off
 *
 * Revision 1.12  2001/06/10 17:15:35  steros
 * Fixed code for printing negative numbers.
 *
 * Revision 1.11  2001/06/02 08:04:36  steros
 * Serial access modifies.
 *
 * Revision 1.10  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.9  2001/05/29 17:41:17  steros
 * Modified printc for line buffered functionality.
 *
 * Revision 1.8  2001/05/27 17:33:36  steros
 * Added other log & serial features.
 * Logging and serial still have strange behaviour and possible bugs...
 *
 * Revision 1.7  2001/05/26 14:56:15  steros
 * Added serial logging facility.
 *
 * Revision 1.6  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.5  2000/10/25 19:59:39  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
 *
 * Revision 1.4  2000/10/23 23:33:44  iurlano
 * Remove ebx references from the libOSSO
 *
 * Revision 1.3  2000/10/17 22:57:30  iurlano
 * io changes (added outd & ind)
 * printc expansion (added %ld & %lx in format)
 * work on rtl8139
 *
 * Revision 1.2  2000/08/03 15:57:44  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:18  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.1.1.1  2000/03/06 15:53:45  iurlano
 *
 *
 * Revision 1.1.1.1  2000/01/19 10:42:42  pecciant
 *
 *
 * Revision 1.5  1999/12/20 17:34:08  iurlano
 * Corretto bug per cui non stampava lo 0
 *
 * Revision 1.4  1999/12/02 15:02:43  iurlano
 *
 * Aggiustato files nella directory lib
 *
 *
 ***************************************************************************/

#include <syscalls.h>
#include <string.h>
#include <types.h>

#include <../Config_Global.h>



#define LINE_BUFFER_LENGTH 256


#ifdef LINE_BUFFERED_OUTPUT
  char LineBuffer[LINE_BUFFER_LENGTH];
  int numcar=0;
  int LineBuffered=1;
#endif


void linebuffer_writechar(unsigned char c) {
    __asm__ ("int $0x40"::"a" (SYS_PRINT),"c" (SYS_PRINT_CHAR),"d" (c));
#ifdef SERIAL_LOGGING
#ifndef SERIAL_LOG_KERNEL
{
  unsigned char v;

  do {
    __asm__ volatile ("inb %%dx,%%al":"=a" (v):"d" (*(((word *)(0x400+(SERIAL_LOG_PORT*2)))+5)));
  } while (!(v & 0x20));
  __asm__ volatile ("outb %%al,%%dx"::"a" (c),"d" (*((word *)(0x400+(SERIAL_LOG_PORT*2)))));
}
#endif
#endif
}


#ifdef LINE_BUFFERED_OUTPUT
void printc_flush(void) {
  char *p=LineBuffer;
  int x;

  __asm__ volatile ("cli");
  for (x=numcar; x; p++, x--) {
    linebuffer_writechar(*p);
  }
  numcar=0;
  __asm__ volatile ("sti");
}
#else 
void printc_flush(void) {
  return ;
}
#endif


void printc(unsigned char c) {
#ifdef LINE_BUFFERED_OUTPUT
  if (LineBuffered) {
    LineBuffer[numcar]=c;
    numcar++;
    if (c=='\n' || numcar>=LINE_BUFFER_LENGTH)
      printc_flush();
  } else {
    if (numcar) printc_flush();
#endif
    linebuffer_writechar(c);
#ifdef LINE_BUFFERED_OUTPUT
  }
#endif
}


void prints(char * s) {
  int i;
  for (i=0;s[i]!='\0';i++)
    printc(s[i]);
}



void printd(int d) {
  char tmp[20], s=(d<0);
  int i;

  if (d==0) {
    printc('0');
  } else {
    if (s) {
      d=0-d;
      printc('-');
    }
    i=0;
    tmp[0]='0';
    while (d>0) {
      tmp[i++]='0'+d%10;
      d/=10;
    }
    for (i--;i>=0;i--)
      printc(tmp[i]);
  }
}


void printld(long int d) {
  char tmp[20], s=(d<0);
  int i;
  if (d==0) {
    printc('0');
  } else {
    if (s) {
      d=0-d;
      printc('-');
    }
    i=0;
    tmp[0]='0';
    while (d>0) {
      tmp[i++]='0'+d%10;
      d/=10;
    }
    for (i--;i>=0;i--)
      printc(tmp[i]);
  }
}

void printu(unsigned int d) {
  char tmp[20];
  int i;

  if (d==0) {
    printc('0');
  } else {
    i=0;
    tmp[0]='0';
    while (d>0) {
      tmp[i++]='0'+d%10;
      d/=10;
    }
    for (i--;i>=0;i--)
      printc(tmp[i]);
  }
}

void printb(unsigned int d) {
  char tmp[35];
  int i;

  if (d==0) {
    printc('0');
  } else {
    i=0;
    tmp[0]='0';
    while (d>0) {
      tmp[i++]='0'+(d&1);
      d>>=1;
    }
    for (i--;i>=0;i--)
      printc(tmp[i]);
  }
}

void printx(int d) {
  char tmp[20];
  int i;

  if (d==0) {
    printc('0');
  } else {
    i=0;
    tmp[i]='0';
    while (d>0) {
      if ((d&0xF)<0xA)
	tmp[i++]='0'+(d&0xF);
      else tmp[i++]='A'+(d&0xF)-10;
      d>>=4;
    }
    for (i--;i>=0;i--)
      printc(tmp[i]);
  }
}

void printlx(long int d) {
  char tmp[40];
  int i;

  if (d==0) {
    printc('0');
  } else {
    i=0;
    tmp[i]='0';
    while (d>0) {
      if ((d&0xF)<0xA)
	tmp[i++]='0'+(d&0xF);
      else tmp[i++]='A'+(d&0xF)-10;
      d>>=4;
    }
    for (i--;i>=0;i--)
      printc(tmp[i]);
  }
}

int new_unworking_printf(char * stringa, ... ) {
  char Stringone[2048];
  void * args;
  char * i;

  args = (void *) __builtin_next_arg (stringa);
  sprintf(Stringone, stringa,args);
  i=Stringone;
  while (*i) printc(*i++);
  return 0;
}


int _printf(char * stringa, ...)
{
  int i;
  char * argomenti; /* pointer to arguments to be printed */
  char * temp ;

  argomenti=(char *) &stringa + sizeof(stringa);

  /* Initialized to the first element */

  for (i=0;stringa[i]!='\0';i++) {
    /* Scan */
    switch (stringa[i]) {
    case '%':
      if (stringa[i+1]=='s') {  /* STRING */
	temp=*((char * *)argomenti);
	prints(temp);
	argomenti+=4; /* Pointer update */
	i++;
      }
      else if (stringa[i+1]=='c') { /* Char */

	printc(*((char*)argomenti));
	argomenti+=4;
	i++;
      }
      else if (stringa[i+1]=='d') {
	printd(*((signed int*)argomenti));
	argomenti+=4;
	i++;
      }
      else if (stringa[i+1]=='l') {
	if (stringa[i+2]=='d') {
	  printld(*((long signed int*)argomenti));
	  argomenti+=4;
	  i+=2;
	} else if (stringa[i+2]=='x') {
	  printlx(*((long unsigned int*)argomenti));
	  argomenti+=4;
	  i+=2;
	}
      }
      else if (stringa[i+1]=='u') { /* unsigned 32 bit integer */
	printu(*((unsigned int*)argomenti));
	argomenti+=4;
	i++;
      }
      else if (stringa[i+1]=='x') { /* Hexdecimal 32 bit integer */
	printx(*((unsigned int*)argomenti));
	argomenti+=4;
	i++;
      }
      else if (stringa[i+1]=='b') { /* 32 bit binary*/
	printb(*((unsigned int*)argomenti));
	argomenti+=4;
	i++;
      }
      break;
    default: /* Normal char */ 
      printc(stringa[i]);
      break;
    } 
  }

  return 0;
}
