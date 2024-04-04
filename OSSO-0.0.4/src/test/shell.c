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
 * file : $Source: /cvsroot/osso/OSSO/src/test/shell.c,v $
 * Description: First usable shell prototype
 ***************************************************************************

 ***************************************************************************
 * $Id: shell.c,v 1.27 2001/06/29 17:45:53 steros Exp $
 ***************************************************************************
 *
 * $Log: shell.c,v $
 * Revision 1.27  2001/06/29 17:45:53  steros
 * Added long file name for fat. Not complete!
 *
 * Revision 1.26  2001/06/26 21:00:41  iurlano
 * Added mem information program.
 * Changed shell mem command to meminfo
 *
 * Revision 1.25  2001/06/25 20:24:14  steros
 * Bugfixes.
 *
 * Revision 1.24  2001/06/17 20:52:44  iurlano
 * *** empty log message ***
 *
 * Revision 1.23  2001/06/16 11:35:21  steros
 * Added dir scanning functions. Fixed various bugs.
 *
 * Revision 1.22  2001/06/13 21:04:48  steros
 * Various fixes.
 *
 * Revision 1.21  2001/06/13 16:45:28  steros
 * FAT/SHELL Restructuration, bug fixes and flush function to avoid line
 * buffering.
 *
 * Revision 1.20  2001/06/12 11:02:26  iurlano
 * Added interface fucntion to fat.c fs.c and others (not working)
 *
 * Revision 1.19  2001/06/12 10:18:01  iurlano
 * Added limited directory capabilities to the shell (cd command) not yet working
 *
 * Revision 1.18  2001/06/10 17:17:48  steros
 * Working on fat basic functions.
 *
 * Revision 1.17  2001/06/03 09:53:17  steros
 * Bug fixes.
 *
 * Revision 1.16  2001/05/30 21:25:05  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.15  2001/05/29 17:41:17  steros
 * Modified printc for line buffered functionality.
 *
 * Revision 1.14  2001/05/02 21:53:53  iurlano
 * Added Backspace feature to the shell
 *
 *
 ***************************************************************************/



#include <printf.h>
#include <processes.h>
#include <signals.h>
#include <memory.h>
#include <string.h>

#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>

#include <fs.h>

#include "../Config_Global.h"


typedef enum {
  INFO, MEM, TYPE, LS, CD, REHASH
} TipoComando;

typedef struct {
  TipoComando tipo;
  char *str;
} Comandi_t;

Comandi_t Comandi[] = {
  { INFO,   "info"   },
  { MEM,    "meminfo"},
  { TYPE,   "type"   },
  { LS,     "ls"     },
  { CD,     "cd"     },
  { REHASH, "rehash" },
  { -1, 0 }   // End of list. Don't touch!
};


char get_char(void);

#define DRV_PRINT_PROC_INFO   129

unsigned int fat_pid=-1;

char pwd[MAX_ACCEPTABLE_PATH_LENGTH+1]="/";



TipoComando Comando(char **s) {
  Comandi_t *tc=Comandi;
  char *p, *sp;

  for (; tc->str; tc++) {
    for (p=tc->str, sp=*s; *p && *p==*sp; p++, sp++);
    if (!*p && (*sp==' ' || *sp=='\t' || *sp=='\n' || *sp=='\0')) {
      while (*sp==' ' || *sp=='\t') sp++;
      *s=sp;
      return tc->tipo;
    }
  }
  return -1;
}


void AbsolutePath(char *dest, char *path) {
  char *p=dest;

//printf("*** pwd='%s', path='%s'\n",pwd,path);
  if (*path!='/') {
    strcpy(dest,pwd);
    for (; *p; p++);
    if (*(p-1)!='/') *(p++)='/';
  }
  strcpy(p,path);
//printf("*** dest='%s'\n",dest);
}


void Com_info(void) {
  __asm__ ("int $0x40"::"a" (DRV_PRINT_PROC_INFO));
}


void Com_type(char *s) {
  char Nome[MAX_ACCEPTABLE_PATH_LENGTH+1];
  #define BLOCK_SIZE 1024
  char data[BLOCK_SIZE], *p;
  long numbytes;
  int fd, i;

  if (s=='\0')  {
    printf("Type requires an argument\n");
    return;
  }

  if (!*s)
    s=pwd;
  else {
    AbsolutePath(Nome,s);
    s=Nome;
  }

  if ((fd=Open_File(s))<1) {
    printf("*** TYPE: open error for '%s'\n",s);
    return;
  }
  for (; (numbytes=Read_File(fd,-1,BLOCK_SIZE,data))>0; ) {
    for (i=numbytes, p=data; i; i--, p++)
      printf("%c",*p);
  }
  Close_File(fd);
}


void Com_ls(char *s) {
  #define NUM_DIRENT 4
  char Nome[MAX_ACCEPTABLE_PATH_LENGTH+1];
  FS_PUBLIC_DIR_ENTRY pde[NUM_DIRENT];
  int direntries, i=0, fd, x;

  if (!*s)
    s=pwd;
  else {
    AbsolutePath(Nome,s);
    s=Nome;
  }

  if ((fd=Open_Dir(s))<1) {
    printf("*** LS: dir open error for '%s'\n",s);
    return;
  }
  while ((direntries=Read_Dir(fd,NUM_DIRENT,pde))!=0) {
    for (x=0; x<direntries; x++, i++) {
      if (pde[x].filename[0]!='\0')
        printf("%d : %s '%s' size = %d bytes\n",i,
               (pde[x].attribute&S_IFDIR)?"DIR ":"FILE",
                pde[x].filename,pde[x].file_size);
    }
  }
  Close_Dir(fd);
}


void Com_cd(char *s) {
  char Nome[MAX_ACCEPTABLE_PATH_LENGTH+1];
  FS_PUBLIC_DIR_ENTRY de;

  if (!*s)
    s=pwd;
  else
    if (*s=='/' && !*(s+1)) {
      strcpy(pwd,s);
      return;
    } else {
      AbsolutePath(Nome,s);
      s=Nome;
    }

  de=Get_Entry(s);
  if (!*de.filename) {
    printf("*** Error: directory not found!\n");
    return;
  }
  if (!(de.attribute & S_IFDIR)) {
    printf("*** Error: not a directory!\n");
    return;
  }
  strcpy(pwd,s);
}



void Exec(char *s) {
  char Nome[MAX_ACCEPTABLE_PATH_LENGTH+1];
  FS_PUBLIC_DIR_ENTRY de;
  byte *program_buffer;
  int pid, fd;
  word sz;

  if (!*s)
    s=pwd;
  else {
    AbsolutePath(Nome,s);
    s=Nome;
  }

//  printf("[* SHELL] cerco '%s'...\n",s);
  de=Get_Entry(s);
  if (!*de.filename) {
    printf("*** Error: file not found!\n");
    return;
  }

//  printf("'%s' found (size=%d)!\n",s,de.file_size);

  program_buffer=mallosso(de.file_size);
  if ((fd=Open_File(s))<1) {
    printf("*** EXEC: open error for '%s'\n",s);
    return;
  }
  if ((sz=Read_File(fd,-1,de.file_size,program_buffer))<de.file_size) {
    printf("*** EXEC: load error for '%s' (%d)\n",s,sz);
    return;
  }
  Close_File(fd);

  pid=Create_Process_From_ELF(program_buffer,SuperVisor_Level);
  freeosso(program_buffer);
  Signal(pid,SIGNAL_CONT);
}



int main(void) {
  char riga[500]="", *rigap;
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

  printf("First anticipation of a shell .... mypid is %d\n",getpid());


  if ((ret=DriverInit(0))!=0) {
    printf("[SHELL] ***** Error in initialization phase: %s!!!\n",
	   MpsStrerror(ret));
    exit();
  }

  while (1) {
    //printf("Waiting for a char ......\n");
    printf("OSSO pre-0.1.0 %s#> ",pwd);
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

    switch (Comando(&rigap)) {
      case INFO:
          Com_info();
          break;
      case MEM:
          printf("Free Mem pages = %d\n",get_free_pages());
          break;
      case TYPE:
          Com_type(rigap);
          break;
      case LS:
          Com_ls(rigap);
          break;
      case CD:
          Com_cd(rigap);
          break;
      case REHASH:
          FS_Clear_Cache();
          break;
      default:
          Exec(riga);
    }
  }
  return 0;
}
