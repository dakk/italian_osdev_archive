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


/******************************* O S S O ***********************************
 * file : kernel/printk.c                                                  *
 * Routine printk for printing to video                                    *
 ***************************************************************************

 ***************************************************************************
 * $Id: printk.c,v 1.13 2001/06/25 20:24:14 steros Exp $
 ***************************************************************************
 *
 * $Log: printk.c,v $
 * Revision 1.13  2001/06/25 20:24:14  steros
 * Bugfixes.
 *
 * Revision 1.12  2001/06/24 22:18:12  iurlano
 * Optimized process creation functions
 * Added programs to the test section in xconfig
 * Added some functions to kernel-loader.c
 * Page faults now terminate the running process
 * Bugfixed memory leak in program execution from the shell
 *
 * Revision 1.11  2001/06/24 17:04:39  steros
 * Bugfixes.
 *
 * Revision 1.10  2001/06/17 13:38:00  steros
 * Extended logging fuctionality to boot & kernel messages.
 *
 * Revision 1.9  2001/05/02 21:53:53  iurlano
 * Added Backspace feature to the shell
 *
 * Revision 1.8  2001/04/26 22:09:09  iurlano
 * fat work (again)
 * working snapshot ?
 *
 * Revision 1.7  2001/04/26 19:25:54  iurlano
 * fat work
 * bugfixes
 *
 * Revision 1.6  2001/04/23 14:43:05  iurlano
 * work on FAT module
 * work on FS protocol
 *
 * Revision 1.5  2001/04/10 23:10:26  iurlano
 * Little changes to the Makefiles
 * Check the inline asm instructions
 * check whether the kernel compiles with -O (strange behaviour, as expected)
 * Removed some warnings
 *
 * Revision 1.4  2000/12/12 22:29:00  iurlano
 * Threadified fork(). To be tested but seems to be working
 * Added support for coloured printk() in kernel.
 *
 * Revision 1.3  2000/10/25 19:59:39  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
 *
 * Revision 1.2  2000/07/27 12:13:07  iurlano
 * Translation in progress
 *
 * Revision 1.1.1.1  2000/05/30 08:11:12  iurlano
 * OSSO : First Public Release
 *
 ***************************************************************************/

#include <io.h>
#include <types.h>
#include <printk.h>

#include "../Config_Global.h"


#ifdef SERIAL_LOGGING
 #ifdef SERIAL_LOG_KERNEL
  #define OUTPUT2(c)                                                       \
    {                                                                      \
      unsigned char v;                                                     \
                                                                           \
      do {                                                                 \
        __asm__ volatile ("inb %%dx,%%al":"=a" (v):"d"                     \
                          (*(((unsigned int *)(0x400+(SERIAL_LOG_PORT*2)))+5)));\
      } while (!(v & 0x20));                                               \
        __asm__ volatile ("outb %%al,%%dx"::"a" (c),"d"                    \
                        (*((unsigned int *)(0x400+(SERIAL_LOG_PORT*2))))); \
    }
 #else
   #define OUTPUT2(c)
 #endif
#else
  #define OUTPUT2(c)
#endif


static int x=0,y=0; /* cursor coords */

int abs_y=0;

void scroll_up(void);
void update_HW_cursor(void);

void set_videomem_start_offset(word offset) {
  outb(0xD,0x3D4);
  outb((offset &0xFF),0x3D5);
  outb(0xC,0x3D4);
  outb((offset >> 8),0x3D5);
}

word get_videomem_start_offset(void) {
  word t;
  outb(0xD,0x3D4);
  t=inb(0x3D5);
  outb(0xC,0x3D4);
  t|=inb(0x3D5) << 8;
  return t;
}


void get_initial_cursor_pos(void) {
  char * punt;
  char data;
  punt=(char *)0x450;
  data=*punt++;
  x=data;
  data=*punt;
  y=data;
  abs_y=y;
}

void accapo(char **dest){ /* Carriage return */
  x=0;
  if (y>=24) {
    scroll_up();
  } else {
    y++;
    abs_y++;
  }
  *dest=(char *) 0xb8000+160*abs_y;
  update_HW_cursor();
}

void _PRINTS(const char attribute,const char * str) {
  int i;
  char * dest;
  dest=(char*) (0xb8000+2*x+160*abs_y); /* Video memory coords */
  for (i=0;str[i]!='\0';i++) {
    if (str[i]=='\n') accapo(&dest);
    else {
      if(x==80)	accapo(&dest);
      *dest++=str[i];
      *dest++=attribute;
      x++;
      OUTPUT2(str[i]);
    }
  }
  update_HW_cursor();
}

void _PRINTC(const char attribute,const char ch) {
  char * dest;
  dest=(char*) (0xb8000+2*x+160*abs_y); /* Video memory coords */
  OUTPUT2(ch);
  if (ch=='\n') accapo(&dest);
  else if (ch==127) {
    if (x>0) {
      x--;
      dest=(char*) (0xb8000+2*x+160*abs_y); /* Video memory coords */
      *dest=' ';
    } /* else do nothing */
  }
  else {
    if(x==80) accapo(&dest);
    *dest++=ch;
    *dest++=attribute;
    x++;
  }
  update_HW_cursor();
}


void _PRINTD(const char attribute,signed int d) {
  char tmp[20];
  int i;
  char * dest;
  dest=(char*) (0xb8000+2*x+160*abs_y); /* Video memory coords */
  i=0;
  if (d<0) {
    if(x==80) accapo(&dest);
    *dest++='-';
    *dest++=attribute;
    d=-d;
    x++;
  }
  do {
    tmp[i++]='0'+d%10;
    d/=10;
  } while (d>0);
  for (i--;i>=0;i--) {
    if(x==80) accapo(&dest);
    *dest++=tmp[i];
    *dest++=attribute;
    OUTPUT2(tmp[i]);
    x++;
  }

  update_HW_cursor();
}

void _PRINTU(const char attribute,unsigned int d) {
  char tmp[20];
  int i;
  char * dest;
  dest=(char*) (0xb8000+2*x+160*abs_y); /* Video memory coords */
  i=0;
  do {
    tmp[i++]='0'+d%10;
    d/=10;
  } while (d>0);
  for (i--;i>=0;i--) {
    if(x==80) accapo(&dest);
    *dest++=tmp[i];
    *dest++=attribute;
    OUTPUT2(tmp[i]);
    x++;
  }
  update_HW_cursor();
}

void _PRINTB(const char attribute,unsigned int d) {
  char tmp[35];
  int i;
  char * dest;
  dest=(char*) (0xb8000+2*x+160*abs_y); /* Video memory coords */
  i=0;
  do {
    tmp[i++]='0'+(d&1);
    d>>=1;
  } while (d>0);
  for (i--;i>=0;i--) {
    if(x==80) accapo(&dest);
    *dest++=tmp[i];
    *dest++=attribute;
    OUTPUT2(tmp[i]);
    x++;
  }
  update_HW_cursor();
}


void _PRINTX(const char attribute,unsigned int d) {
  char tmp[10];
  int i;
  char * dest;
  dest=(char*) (0xb8000+2*x+160*abs_y); /* Video memory coords */
  i=0;
  do {
    if ((d&0xF)>9) tmp[i++]='A'+(d&0xF)-10;
    else tmp[i++]='0'+(d&0xF);
    d>>=4;
  } while (d>0);
  for (i--;i>=0;i--) {
    if(x==80) accapo(&dest);
    *dest++=tmp[i];
    *dest++=attribute;
    OUTPUT2(tmp[i]);
    x++;
  }

  update_HW_cursor();
}

int _color_printk(const char attribute,char * stringa, ... )
{
  int i;
  char * argomenti; /* pointer to data to be printed */
  char * temp ;

  argomenti=(char *) &stringa + sizeof(stringa);

  /* Initialized to the first argument */

  for (i=0;stringa[i]!='\0';i++) {
    /* Scanning */
    switch (stringa[i]) {
    case '%': /* Variable */
      if (stringa[i+1]=='s') {  /* STRING */
        temp=*((char * *)argomenti);
        _PRINTS(attribute,temp);
        argomenti+=4; /* Updating pointer */
        i++;
      }
      else if (stringa[i+1]=='c') { /* Char */
        _PRINTC(attribute,*((char*)argomenti));
        argomenti+=4;
        i++;
      }
      else if (stringa[i+1]=='d') {
        _PRINTD(attribute,*((signed int*)argomenti));
        argomenti+=4;
        i++;
      }
      else if (stringa[i+1]=='u') { /* Unsigned 32 bit integer */
        _PRINTU(attribute,*((unsigned int*)argomenti));
        argomenti+=4;
        i++;
      }
      else if (stringa[i+1]=='x') { /* Hexadecimal 32 bit integer */
        _PRINTX(attribute,*((unsigned int*)argomenti));
        argomenti+=4;
        i++;
      }
      else if (stringa[i+1]=='b') { /* Binary 32 bit */
        _PRINTB(attribute,*((unsigned int*)argomenti));
        argomenti+=4;
        i++;
      }
      break;
    default: /* Normal char */
      _PRINTC(attribute,stringa[i]);
      break;
    }
  }
  while (x>=80) { /* Last column check */
    x-=80;
    if (y>=24) scroll_up(); else
      {
        y++;
        abs_y++;
      }
  }

  update_HW_cursor();

  return 0;
}

void update_HW_cursor(void) {
  unsigned int i;
  i=x+80*abs_y; /* Computing coords */

  if (abs_y>=24) set_videomem_start_offset(80*(abs_y-24));
  /* Updating HW Cursor*/
  outb(0xF,0x3D4);
  outb((i & 0xFF),0x3D5);
  outb(0xE,0x3D4);
  outb((i >> 8),0x3D5);
}


/* Scrolling */
void scroll_up(void)
{
  int i;
  dword * source, * dest,top;

    
  if (abs_y >= 150) {
    source=(dword *) (0xb8000+160); /* Second row ... */    
    dest=(dword *) 0xb8000; /* First row */
    top=(160*(abs_y))/4;

    for (i=0;i<top;i++) *dest++=*source++; /* COPIA */
    for (i=0;i<80;i++) { /* Empties last row */
      *((byte*)dest)++=' ';
      *((byte*)dest)++=Default_Color; /* Setting attribute */
    }
  } else
  abs_y++;
  
}
