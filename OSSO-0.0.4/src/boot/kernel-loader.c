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
 * file : $Source: /cvsroot/osso/OSSO/src/boot/kernel-loader.c,v $
 * Description: 16-bit kernel loader (second stage boot-loader).
 ***************************************************************************

 ***************************************************************************
 * $Id: kernel-loader.c,v 1.24 2001/08/18 15:18:29 steros Exp $
 ***************************************************************************
 *
 * $Log: kernel-loader.c,v $
 * Revision 1.24  2001/08/18 15:18:29  steros
 * Serial driver: new version. It has already to be tested.
 *
 * Revision 1.23  2001/06/24 22:18:12  iurlano
 * Optimized process creation functions
 * Added programs to the test section in xconfig
 * Added some functions to kernel-loader.c
 * Page faults now terminate the running process
 * Bugfixed memory leak in program execution from the shell
 *
 * Revision 1.22  2001/06/17 13:38:00  steros
 * Extended logging fuctionality to boot & kernel messages.
 *
 * Revision 1.21  2001/06/01 20:47:40  iurlano
 * Added prototype and schema of an implementation of a ELF loader
 * function in kernel-loader.c, the bcc code.
 * Changed TO_DO file to new contents
 *
 * Revision 1.20  2001/05/30 21:25:04  steros
 * Corrected serial input as substitute of keyboard: now it work!
 * And other little adjustment...
 *
 * Revision 1.19  2001/05/14 05:47:39  steros
 * Kernel loader now should be fs indipendent.
 *
 * Revision 1.18  2001/05/02 18:50:27  steros
 * Driver dependancies automation implemented.
 *
 * Revision 1.17  2001/05/01 13:47:39  steros
 * Improved MPS.
 *
 * Revision 1.16  2001/04/05 21:06:13  iurlano
 * Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
 *
 * Revision 1.15  2001/04/05 14:10:54  iurlano
 * xconfig changes
 *
 * Revision 1.14  2001/02/24 14:11:51  steros
 * Removed limitations.
 *
 * Revision 1.13  2001/02/23 17:41:41  steros
 * Single pass-parser implemented. Inserted segment interval in INITCONF.
 * Inserted checking for modules overflow. Fixed some parser bugs.
 *
 *
 ***************************************************************************/

#define _KERNEL_LOADER_


#include "../Config_Global.h"
#include "../Config_Boot.h"
#include "parser.h"
#include "parser.c"


/*------------------------------------------------------------*/
/*                        Definitions.                        */
/*   (Add or delete the '/' char at the end of comment line)  */
/*------------------------------------------------------------*/

/* Enable visualization of configuration file data. */
  #define VERBOSE
/**/

/* Enable visualizzation of disk structure data. *
  #define XVERBOSE
/**/

/* Enable debug code. *
  #define DEBUG
/**/


#define MAX_FILE_NAME        1024
#define INITCONF_BUFFER_SIZE 1024


typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned int uint;
typedef unsigned long int ulong;
typedef unsigned long int dword;


/* Values from configuration section. */
char BootDir[128]="", ModulesDir[128]="";
uint BootSegmentStart=-1, BootSegmentEnd=-1;
uint PreloadSegmentStart=-1, PreloadSegmentEnd=-1;

/* Start program values. */
uint StartSegment=-1, StartOffset=0;
uint EndSegment=-1;

/* Pointer for first free byte of reserved areas. */
uint BootSeg=-1, BootOff=0;
uint PreloadSeg=-1, PreloadOff=0;


/*
 Colors definition.
*/
#define Black          0
#define Blue           1
#define Green          2
#define Cyan           3
#define Red            4
#define Magenta        5
#define Brown          6
#define LightGray      7

#define DarkGray       8
#define LightBlue      9
#define LightGreen    10
#define LightCyan     11
#define LightRed      12
#define LightMagenta  13
#define Yellow        14
#define White         15


/*
 Definitions for uniform printing.
*/
char STRING_OK[]    = "[OK]\n\r";
char STRING_FAIL[]  = "[FAILED]\n\r";
char STRING_PANIC[] = "  Press Ctrl-Alt-Canc for reboot. ";
#define OPERATION(s)      color_prints(LightBlue,s)
#define OPERATION_OK      color_prints(Yellow,STRING_OK)
#define OPERATION_FAIL    color_prints(LightRed,STRING_FAIL)
#define ERROR(s)          color_prints(Red,s)
#define ERRORP(s,p)       {                                 \
                            char _stringa[1024];            \
                            sprintf(_stringa,s,p);          \
                            color_prints(Red,_stringa);     \
                          }
#define ERRORP2(s,p1,p2)  {                                 \
                            char _stringa[1024];            \
                            sprintf(_stringa,s,p1,p2);      \
                            color_prints(Red,_stringa);     \
                          }
#define COMMENT(s)        color_prints(Green,s)
#define PANIC             {color_prints(Yellow,STRING_PANIC); while (1);}


/* Variables used by INITCONF window. */
char initconf_buffer[INITCONF_BUFFER_SIZE];   /* INITCONF buffer. */
long conf_size,       /* Size of entire INITCONF file.            */
     conf_winstart;   /* Start of window relative to entire file. */
word conf_winsize;    /* INITCONF window actual size.             */





/*---------------------------------------------------------------------*/
/*                     Far memory access functions.                    */
/*---------------------------------------------------------------------*/


char readbyte_val;

void readbyte(int segment, int offset) {
#asm
#if !__FIRST_ARG_IN_AX__
  mov   bx,sp
  mov   ax,[bx+2]
  mov   bx,[bx+4]
#else
  mov   bx,sp
  mov   bx,[bx+2]
#endif
  push  ds
  mov   ds,ax
  mov   cl,byte[bx]
  pop   ds
  mov   [_readbyte_val],cl
#endasm
}


void readbytex(ulong address) {
  int seg,off;
  seg=address>>4;
  off=address&0xF;
  readbyte(seg,off);
}



void readbytes(uint segment, uint offset, void *dest_address, int numbytes)
{
  int n;
  char *dest=(char *)dest_address;

  for (n=numbytes; n; n--, dest++, offset++) {
    readbyte(segment,offset);
    *dest=readbyte_val;
  }
}


void writebyte(uint segment, uint offset, byte value) {
#asm
#if !__FIRST_ARG_IN_AX__
  mov   bx,sp
  mov   ax,[bx+2]
  mov   cx,[bx+6]
  mov   bx,[bx+4]
#else
  mov   bx,sp
  mov   cx,[bx+4]
  mov   bx,[bx+2]
#endif
  push  ds
  mov   ds,ax
  mov   byte[bx],cl
  pop   ds
#endasm
}


void writebytex(ulong address,byte value) {
  int seg,off;
  seg=address>>4;
  off=address&0xF;
  writebyte(seg,off,value);
}


void writebytes(int segment, int offset, void *source_address, int numbytes) {
  int n;
  char *src=(char *)source_address;

  for (n=numbytes; n; n--, offset++, src++)
    writebyte(segment,offset,*src);
}


/*
 Increment a couple segment:offset of a certain value.
*/
void AddressIncrement(uint *segment, uint *offset,
                      ulong increment)
{
  ulong address=((ulong)*segment)*16l+((ulong)*offset);

  address+=increment;
  *segment=(int)(address/16l);
  *offset=(int)(address%16l);
}



/*---------------------------------------------------------------------*/
/*                         Printing functions.                         */
/*---------------------------------------------------------------------*/

#ifdef SERIAL_LOGGING
 #ifdef SERIAL_LOG_BOOT
  int Porta_Pronti;
  int Porta_OUT;
 #endif
#endif

void printc(char a) {
#asm
#if !__FIRST_ARG_IN_AX__
  mov   bx,sp
  mov   ax,[bx+2]
#endif

#ifdef SERIAL_LOGGING
 #ifdef SERIAL_LOG_BOOT
  push ax

  mov  dx,[_Porta_Pronti]
sbusy:
  in   al,dx
  and  al,#$20
  jz   sbusy
  pop  ax
  mov  dx,[_Porta_OUT]
  out  dx,al
 #endif
#endif

  mov   ah,#$0E
  mov   bx,#7
  int   $10
#endasm
}


void printc_color_asm(char a, char attrib) {
#asm
#if !__FIRST_ARG_IN_AX__
  mov   bx,sp
  mov   ax,[bx+2]
  mov   bx,[bx+4]
  jmp go
#endif
  mov   bx,sp
  mov   bx,[bx+2]
go:
  mov   ah,#$09
  mov   cx,#1
  int   $10
#endasm
}


void printc_color(char a, char attrib) {
  if (a!='\n' && a!='\r')
    printc_color_asm(a,attrib);
  printc(a);
}


void prints(char *s) {
  int i;

  for (i=0; s[i]!='\0'; i++)
    printc(s[i]);
}


void color_prints(char attrib, char *s) {
  int i;

  for (i=0; s[i]!='\0'; i++)
    printc_color(s[i],attrib);
}


void printd(int d) {
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
    for (i--; i>=0; i--)
      printc(tmp[i]);
  }
}


void printld(long int d) {
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
    for (i--; i>=0; i--)
      printc(tmp[i]);
  }
}


void printu(uint d) {
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
    for (i--; i>=0; i--)
      printc(tmp[i]);
  }
}


void printb(uint d) {
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
    for (i--; i>=0; i--)
      printc(tmp[i]);
  }
}


void printx(uint d) {
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
    for (i--; i>=0; i--)
      printc(tmp[i]);
  }
}


void printlx(ulong d) {
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
    for (i--; i>=0; i--)
      printc(tmp[i]);
  }
}

int _printf(char *stringa, ...)
{
  int i;
  char *argomenti; /* pointer to arguments to be printed */
  char *temp ;

  argomenti=(char *) &stringa + sizeof(stringa);
  /* Initialized to the first element */
  for (i=0; stringa[i]!='\0'; i++) {
    /* Scan */
    switch (stringa[i]) {
    case '%':
      if (stringa[i+1]=='s') {  /* STRING */
	temp=*((char **)argomenti);
	prints(temp);
	argomenti+=2; /* Pointer update */
	i++;
      }
      else if (stringa[i+1]=='c') { /* Char */
	printc(*((char *)argomenti));
	argomenti+=2;
	i++;
      }
      else if (stringa[i+1]=='d') {
	printd(*((int *)argomenti));
	argomenti+=2;
	i++;
      }
      else if (stringa[i+1]=='l') {
	if (stringa[i+2]=='d') {
	  printld(*((long *)argomenti));
	  argomenti+=4;
	  i+=2;
	} else if (stringa[i+2]=='x') {
	  printlx(*((ulong *)argomenti));
	  argomenti+=4;
	  i+=2;
	}
      }
      else if (stringa[i+1]=='u') {   /* Unsigned integer */
	printu(*((uint*)argomenti));
	argomenti+=2;
	i++;
      }
      else if (stringa[i+1]=='x') {   /* Hexdecimal integer */
	printx(*((uint *)argomenti));
	argomenti+=2;
	i++;
      }
      else if (stringa[i+1]=='b') {   /* 16 bit binary */
	printb(*((uint *)argomenti));
	argomenti+=2;
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



/*---------------------------------------------------------------------*/
/*                        Disk access functions.                       */
/*---------------------------------------------------------------------*/


/*
 Number of tries to make on defective reading.
*/
#define NUM_READ_TRIES 5


/*
 If segment=0, it will be ignored.
 After the call of this function we must read the value of
 'read_error' to know if all is done without error.
*/

int tries=NUM_READ_TRIES;
int read_error=0;

int read_sector_CHS(int cyl, int head, int sect, int drive,
                    int numsect, int segment, void *buffer)
{
#asm

#if __FIRST_ARG_IN_AX__
  push  ax
#endif

  #define CYL     [bx+2]
  #define HEAD    [bx+4]
  #define SECT    [bx+6]
  #define DRIVE   [bx+8]
  #define NUMSECT [bx+10]
  #define SEGMENT [bx+12]
  #define BUFFER  [bx+14]

chs_loop:
  mov   bx,sp
  push  es
  mov   cx,CYL
  shl   ch,6
  xchg  ch,cl
  mov   dx,HEAD
  xchg  dl,dh
  mov   ax,DRIVE
  mov   dl,al
  mov   ax,SECT
  and   ax,#$3F
  or    cl,al

  mov   ax,SEGMENT
  cmp   ax,0
  je    chs_cont
  mov   es,ax

chs_cont:
  mov   ax,NUMSECT
  mov   bx,BUFFER
  mov   ah,#2
  int   $13

  pop   es
  jnc   ok
  mov   cx,[_tries]
  cmp   cx,0
  je    chs_error
  dec   cx
  mov   [_tries],cx
  jmp   chs_loop

chs_error:
  mov   cx,1
  mov   [_read_error],cx

ok:
#if __FIRST_ARG_IN_AX__
  pop   ax
#endif

#endasm


#undef CYL
#undef HEAD
#undef SECT
#undef DRIVE
#undef NUMSECT
#undef SEGMENT
#undef BUFFER
}


int check386(void) {
#asm
  pushf                   ; save the flags original value

        xor ah,ah               ; ah = 0
        push ax                 ; copy ax into the flags
        popf                    ; with bits 12-15 clear

        pushf                   ; Read flags back into ax
        pop ax
        and ah,#$f0             ; check if bits 12-15 are set
        cmp ah,#$f0
        je no386                ; no 386 detected (8088/8086 present)
        ; check for a 286 (bits 12-15 are clear)
        mov ah,#$f0             ; set bits 12-15
        push ax                 ; copy ax onto the flags
        popf

        pushf                   ; copy the flags into ax
        pop ax
        and ah,#$f0             ; check if bits 12-15 are clear
        jz no386                ; no 386 detected (80286 present)
        popf                    ; pop the original flags back
        mov ax,#1			     
                
        jmp OK                  ; no 8088/8086 or 286, so atleast 386
no386:
	popf                  ;
        mov ax,#0
OK:
#endasm
}


/*
 Prints the message of no more space avalaible and stop the execution.
*/
void NoSpaceError(char *s) {
  OPERATION_FAIL;
  if (s) {
    ERRORP("\n\r  *** No more space avalaible in this segment for %s!\n\r",s);
  } else {
    ERROR("\n\r  *** No more space avalaible in this segment!\n\r");
  }
  COMMENT("\n\r  Resize segment interval or load less modules!!!\n\r\n\r");
  PANIC;
}


/*
 Allocation with error management.
*/
void *Alloca(ulong bytes) {
  void *p;

  if ((p=malloc(bytes))==0) {
    OPERATION_FAIL;
    COMMENT("\r\n  Sorry, it seems there's not enough available memory!!!\n\r\n\r");
    PANIC;
  }
}





/*---------------------------------------------------------------------*/
/*                   File system dependent functions.                  */
/*---------------------------------------------------------------------*/

#include "fs.h"





/*---------------------------------------------------------------------*/
/*                          Parsing functions.                         */
/*---------------------------------------------------------------------*/


/*
 Print an error string with the relative line number.
*/
void ParserError(char *ErrorStr) {
  OPERATION_FAIL;
  ERRORP2("\n\r  *** Parser error: %s in line %d!\n\r",ErrorStr,parser_line);
  COMMENT("\n\r  Check configuration file for errors!!!\n\r\n\r");
  PANIC;
}


/*
 Read a number from C-format string. The base can be 8, 10 or 16.
 This have to be substituted by strtol() in the future.
*/
uint StringToInt(char *s) {
  int base=10, molt=1;
  uint num=0;
  char *p;
  char c;

  if (*s=='0') {
    s++;
    if (*s=='x') {
      s++;
      base=16;
    } else
      base=8;
  }
  p=s;
  /* Searching for end of number. */
  switch (base) {
    case 8:
      for (; *p>='0' && *p<='7'; p++);
      break;
    case 10:
      for (; *p>='0' && *p<='9'; p++);
      break;
    case 16:
      for (; ((*p>='0' && *p<='9') || (toupper(*p)>='A' && toupper(*p)<='F'));
        p++);
      break;
    default:
      printf("Error in StringToLong(): base not implemented!!!\n");
      return 0;
  }
  p--;
  /* Scanning number. */
  for (; p>=s; p--, molt*=base) {
    switch (base) {
      case 8:
      case 10:
        num+=(*p-'0')*molt;
        break;
      case 16:
        c=toupper(*p);
        if (c>='A' && c<='F')
          num+=(c-'A'+10)*molt;
        else
          num+=(*p-'0')*molt;
        break;
      default:
        printf("Error in StringToLong(): base not implemented!!!\n");
    }
  }
  return num;
}


/*
 Read a segment interval value and return the start and end values in
 'startseg' e 'endseg'. The first 3 parameters are the same of GetToken().
 If the segment is not present an error will be visualized stopping the
 execution.
*/
void GetSegment(char **stringa, char *sep, char withoutspaces,
               uint *startseg, uint *endseg)
{
  char *token;

  if (*(token=GetToken(stringa,sep,1,NULL))=='\0')
    ParserError("segment start value expected");
  if (*sep!=SEGMENT_SEPARATOR) ParserError("segment end value expected");
  *startseg=StringToInt(token);
  if (*(token=GetToken(stringa,sep,withoutspaces,NULL))=='\0')
    ParserError("segment end value expected");
  *endseg=StringToInt(token);
  if (*startseg>*endseg)
    ParserError("segment start is greater than end");
}


/*
 Make a wrapping of the corrispondent parser function in order to let
 automate the copy of parser file portion in the little buffer that
 is imposed by 16-bit segmented adrressing.
*/
char *GetLine(void) {
  char *l, *b;
  uint c_seg, c_off, off;
  ulong address;

  l=_GetLine(&b);
  while (b) {
/*_printf("\n\r[1]\n\r");*/
    off=b-parser_buffer;
/*_printf("[2, off=%u]\n\r",off);*/
    if (!off) {
      if ((conf_size-conf_winstart)>INITCONF_BUFFER_SIZE)
        ParserError("line too long");
      else
        return NULL;
    }
    conf_winstart+=off;
    if ((conf_size-conf_winstart)>INITCONF_BUFFER_SIZE)
      conf_winsize=INITCONF_BUFFER_SIZE;
    else
      conf_winsize=(int)(conf_size-conf_winstart);
    address=(ulong)CONF_FILE_SEGMENT*(ulong)16+(ulong)conf_winstart+(ulong)4;
/**
_printf("[a, conf_in=0x%x:%x]\n\r",CONF_FILE_SEGMENT,4);
_printf("[b, address=0x%x:%x, increment=0x%x]\n\r",address/16,address%16,conf_winstart);
_printf("[c, conf_in=%ld]\n\r",CONF_FILE_SEGMENT*16+4);
_printf("[d, address=%ld, increment=%ld]\n\r",address,conf_winstart);
**/
    c_seg=address/16;
    c_off=address%16;
    readbytes(c_seg,c_off,initconf_buffer,conf_winsize);
    InitParser(initconf_buffer,conf_winsize,0);
/*_printf("[3, winsize=%u, winstart=%u]\n\r",conf_winsize,conf_winstart);*/
    l=_GetLine(&b);
/**
_printf("[4, b=%u]\n\r",b);
{
  int c=0;
  _printf("[5, '");
  for (; c<40; c++)
    _printf("%c",initconf_buffer[c]);
  _printf("']\n\r");
}
**/
  }
  return l;
}



/*
 Read the configuration section of the file.
*/
void ReadConfigSection(void) {
  char *stringa, *token, sep;
  int c=-1;

  /* Searching for configuration section. */
  while ((stringa=GetLine())!=0) {
    token=GetToken(&stringa,&sep,1,NULL);
    if (token && *token=='[')
      if (FindSection(token)==S_CONFIG) break;
  }
#ifdef VERBOSE
    _printf("\n\r");
#endif
  if (stringa) {
    c=0;
    /* Parsing configuration section. */
    while ((stringa=GetLine())!=0) {
      token=GetToken(&stringa,&sep,1,NULL);
      if (token) {
        if (*token=='[') break;
        switch (FindInstruction(token)) {
          case I_BOOT_SEGMENT:
              {
                ulong addr;
                uint a_seg, a_off;

                if (BootSegmentStart!=-1)
                  ParserError("boot segment already declared");
                if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
                GetSegment(&stringa,&sep,1,&BootSegmentStart,&BootSegmentEnd);
                if (sep!='\0') ParserError("too many parameters");
                BootSeg=BootSegmentStart;
                BootOff=4;
                addr=(ulong)BootSegmentStart*16;
                a_seg=GENERAL_PARAMETERS_ADDRESS/16;
                a_off=GENERAL_PARAMETERS_ADDRESS%16;
                writebytes(a_seg,a_off,&addr,4);
                c++;
#ifdef VERBOSE
                _printf("  Boot Segment: 0x%x - 0x%x\n\r",BootSegmentStart,BootSegmentEnd);
#endif
              }
              break;
          case I_PRELOAD_SEGMENT:
              if (PreloadSegmentStart!=-1)
                ParserError("preload segment already declared");
              if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
              GetSegment(&stringa,&sep,1,&PreloadSegmentStart,&PreloadSegmentEnd);
              if (sep!='\0') ParserError("too many parameters");
              PreloadSeg=PreloadSegmentStart;
              PreloadOff=4;
              c++;
#ifdef VERBOSE
              _printf("  Preload Segment: 0x%x - 0x%x\n\r",PreloadSegmentStart,PreloadSegmentEnd);
#endif
              break;
          case I_BOOT_DIR:
              if (*BootDir)
                ParserError("boot directory already declared");
              if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
              if (*(token=GetToken(&stringa,&sep,1,NULL))=='\0')
                ParserError("directory name expected");
              if (sep!='\0') ParserError("too many parameters");
              strcpy(BootDir,token);
              c++;
#ifdef VERBOSE
              _printf("  Boot Dir: '%s'\n\r",BootDir);
#endif
              break;
          case I_MODULES_DIR:
              if (*ModulesDir)
                ParserError("modules directory already declared");
              if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
              if (*(token=GetToken(&stringa,&sep,1,NULL))=='\0')
                ParserError("directory name expected");
              if (sep!='\0') ParserError("too many parameters");
              strcpy(ModulesDir,token);
              c++;
#ifdef VERBOSE
              _printf("  Modules Dir: '%s'\n\r",ModulesDir);
#endif
              break;
        }
      }
    }
  }
  if (c<4) {
    OPERATION_FAIL;
    prints("\n\r");
    if (c==-1) {
      ERRORP("  %s: Configuration section not found!\n\r",CONF_FILE_NAME);
    } else {
      if (BootSegmentStart==-1)
        ERRORP("  %s: Boot segment not declared!\n\r",CONF_FILE_NAME);
      if (PreloadSegmentStart==-1)
        ERRORP("  %s: Preload segment not declared!\n\r",CONF_FILE_NAME);
      if (!*BootDir)
        ERRORP("  %s: Boot directory not declared!\n\r",CONF_FILE_NAME);
      if (!*ModulesDir)
        ERRORP("  %s: Modules directory not declared!\n\r",CONF_FILE_NAME);
    }
    COMMENT("\n\r  Check configuration file for errors!!!\n\r\n\r");
    PANIC;
  }
#ifndef VERBOSE
  OPERATION_OK;
#endif
}


/*
 Prepare parameters and returns the length of the used area.
*/
long GetParam(int segment, int offset, char *linea, char sep) {
  char *token, ep='\0';
  long size=0;
  int l;

  while (sep==' ' || sep=='\t') {
    token=GetToken(&linea,&sep,1,NULL);
    /* Copy the token in the structure as a parameter name. */
    l=strlen(token)+1;
    writebytes(segment,offset,token,l);
    AddressIncrement(&segment,&offset,(ulong)l);
    size+=l;
    if (sep=='=') {
      token=GetToken(&linea,&sep,1," \t\r\n\0");
      /* Copy the token in the structure as a value. */
      l=strlen(token)+1;
      writebytes(segment,offset,token,l);
      AddressIncrement(&segment,&offset,(ulong)l);
      size+=l;
    } else {
      /* Copy a default value in the structure. */
      writebytes(segment,offset,&ep,1);
      AddressIncrement(&segment,&offset,1l);
      size++;
    }
  }
  if (sep!='\0') ParserError("syntax error");
  writebytes(segment,offset,&ep,1);
  AddressIncrement(&segment,&offset,1l);
  size++;
  return size;
}


/*
 Process start (if type=0) or install (if type!=0) directive.
*/
void GetStartInstall(int type, int *BootSpaceFile, char **stringa, char sep)
{
  char module_name[13];
  char *filename;
  uint startsegment, endsegment, offset, seg, off;
  int cluster, n=0, len;
  long size;
  char *token;

  if (!type && StartSegment!=-1)
    ParserError("start executable already declared");
  if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
  if (*(token=GetToken(stringa,&sep,1,NULL))=='\0')
    ParserError("file name expected");
  memcpy(module_name,token,13);
  filename=AbsolutePath(token,BootDir);
  if (sep==',') {
    if (type) {
      GetSegment(stringa,&sep,1,&startsegment,&endsegment);
      offset=0;
    } else
      GetSegment(stringa,&sep,1,&StartSegment,&EndSegment);
    if (sep!='\0') ParserError("too many parameters");
  } else {
    if (sep!='\0') ParserError("bad instruction format");
    (*BootSpaceFile)++;
    len=strlen(module_name)+1;
    endsegment=BootSegmentEnd;
    seg=BootSeg; off=BootOff;
    AddressIncrement(&BootSeg,&BootOff,(ulong)(len+4));
    if ((type && BootSeg>endsegment) || (!type && BootSeg>EndSegment))
      NoSpaceError(module_name);
    writebytes(seg,off,&len,4);        /* Name size. */
    writebytes(seg,off+4,module_name,len);  /* Name. */
    if (type) {
      startsegment=BootSeg;
      offset=BootOff+4;
    } else {
      StartSegment=BootSeg;
      StartOffset=BootOff+4;
    }
    n=1;
  }
#ifdef VERBOSE
  if (type)
    _printf("    Loading file '%s' at 0x%x:%x... ",filename,startsegment,offset+n*4);
  else
    _printf("    Loading start file '%s' at 0x%x:%x... ",filename,StartSegment,StartOffset+n*4);
#endif
  cluster=GetStartCluster(filename,0,&size);
  if (n) {
    seg=BootSeg; off=BootOff;
    AddressIncrement(&BootSeg,&BootOff,(ulong)size+4);
    if ((type && BootSeg>endsegment) || (!type && BootSeg>EndSegment))
      NoSpaceError(module_name);
    writebytes(seg,off,&size,4);   /* Module size. */
  }
  if (type)
    Load(cluster,startsegment,offset,endsegment);
  else
    Load(cluster,StartSegment,StartOffset,EndSegment);
#ifdef VERBOSE
  _printf("ok!\n\r");
#endif
}


/*
 Read the configuration section of the file.
*/
void ReadBootSection(void) {
  long BootSpaceFile=0, PreloadSpaceFile=0;
  char *stringa, *token, sep, c=-1;

  /* Searching for boot section. */
  stringa=RegetLine();
  while (stringa) {
    token=GetToken(&stringa,&sep,1,NULL);
    if (token && *token=='[')
      if (FindSection(token)==S_BOOT) break;
    stringa=GetLine();
  }
#ifdef VERBOSE
    _printf("\n\r");
#endif
  if (stringa) {
    c=0;
    /* Parsing configuration section. */
    while ((stringa=GetLine())!=0) {
      token=GetToken(&stringa,&sep,1,NULL);
      if (token) {
        if (*token=='[') break;
        switch (FindInstruction(token)) {
          case I_START:
              GetStartInstall(0,&BootSpaceFile,&stringa,sep);
              break;
          case I_INSTALL:
              GetStartInstall(1,&BootSpaceFile,&stringa,sep);
              break;
          case I_PRELOAD:
              {
                char module_name[13];
                char *filename;
                int cluster, len;
                uint seg, off;
                long size, attrib=0;

/*                if (IS_SPACING(sep)) {*/
                if (sep==' ' || sep=='\t') {
                  token=GetToken(&stringa,&sep,1,NULL);
                  if (!strcasecmp(token,"run")) {
                    attrib=attrib | 1;
                  } else {
                    if (!strcasecmp(token,"driver")) {
                      attrib=attrib | 5;
                    } else ParserError("bad instruction format");
                  }
                  if (sep==' ' || sep=='\t') {
                    token=GetToken(&stringa,&sep,1,NULL);
                    if (!strcasecmp(token,"root")) {
                      attrib=attrib | 2;
                    } else ParserError("bad instruction format");
                  }
                }
                if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
                if (*(token=GetToken(&stringa,&sep,1,NULL))=='\0')
                  ParserError("file name expected");
                memcpy(module_name,token,13);
                filename=AbsolutePath(token,ModulesDir);
                PreloadSpaceFile++;
                len=strlen(module_name)+1;
                seg=PreloadSeg; off=PreloadOff;
                AddressIncrement(&PreloadSeg,&PreloadOff,(ulong)(len+8));
                if (PreloadSeg>PreloadSegmentEnd) NoSpaceError(module_name);
                writebytes(seg,off,&len,4);              /* Name size. */
                writebytes(seg,off+4,module_name,len);   /* Name. */
                writebytes(seg,off+4+len,&attrib,4);     /* Attributes. */
                size=GetParam(PreloadSeg,PreloadOff+4,stringa,sep);
                seg=PreloadSeg; off=PreloadOff;
                AddressIncrement(&PreloadSeg,&PreloadOff,(ulong)(size+4));
                if (PreloadSeg>PreloadSegmentEnd) NoSpaceError(module_name);
                writebytes(seg,off,&size,4);    /* Param size. */
#ifdef VERBOSE
                _printf("    Loading file '%s' at 0x%x:%x... ",filename,PreloadSeg,PreloadOff+4);
#endif
                cluster=GetStartCluster(filename,0,&size);
                seg=PreloadSeg; off=PreloadOff;
                AddressIncrement(&PreloadSeg,&PreloadOff,(ulong)size+4);
                if (PreloadSeg>PreloadSegmentEnd) NoSpaceError(module_name);
                writebytes(seg,off,&size,4);   /* Module size. */
                AddressIncrement(&seg,&off,4l);
                Load(cluster,seg,off,PreloadSegmentEnd);
#ifdef VERBOSE
                _printf("ok!\n\r");
#endif
              }
              break;
          default:
              ParserError("wrong instruction");
        }
      }
    }
  }
  writebytes(BootSegmentStart,0,&BootSpaceFile,4);
  writebytes(PreloadSegmentStart,0,&PreloadSpaceFile,4);
#ifdef VERBOSE
  _printf("  Modules loaded in boot space:    %d.\n\r",BootSpaceFile);
  _printf("  Modules loaded in preload space: %d.\n\r",PreloadSpaceFile);
#endif
  if (c==-1 || StartSegment==-1) {
    OPERATION_FAIL;
    prints("\n\r");
    if (c==-1) {
      ERRORP("  %s: Boot section not found!\n\r",CONF_FILE_NAME);
    } else {
      if (StartSegment==-1)
        ERRORP("  %s: Start program not declared!\n\r",CONF_FILE_NAME);
    }
    COMMENT("\n\r  Check configuration file for errors!!!\n\r\n\r");
    PANIC;
  }
#ifndef VERBOSE
  OPERATION_OK;
#endif
}



/*---------------------------------------------------------------------*/
/*                          Loader functions.                          */
/*---------------------------------------------------------------------*/


/*
 Read configuration file and get configuration and boot section.
*/
void ReadConfig(void) {
  word cluster;

  OPERATION("Getting configuration file...     ");
  cluster=GetStartCluster(AbsolutePath(CONF_FILE_NAME,"/"),0,&conf_size);
  writebytes(CONF_FILE_SEGMENT,0,&conf_size,4);
  Load(cluster,CONF_FILE_SEGMENT,4,CONF_FILE_ENDSEG);
  OPERATION_OK;
  if (conf_size>INITCONF_BUFFER_SIZE)
    conf_winsize=INITCONF_BUFFER_SIZE;
  else
    conf_winsize=(int)conf_size;
  conf_winstart=0;
  OPERATION("Reading configuration section...  ");
  readbytes(CONF_FILE_SEGMENT,(uint)4,initconf_buffer,conf_winsize);
  InitParser(initconf_buffer,conf_winsize,1);
  ReadConfigSection();
  OPERATION("Reading boot section...           ");
  ReadBootSection();
}


/*
 Run the kernel.
*/
void StartKernel(void) {
  OPERATION("Starting kernel... \n\r");
#asm
  mov  ax,[_StartSegment]
  mov  bx,[_StartOffset]
  push ax
  push bx
  retf
#endasm
  OPERATION_FAIL;
}


#define EI_NIDENT 16

typedef 
struct __ELF_Header {
  byte e_ident[EI_NIDENT];
  word e_type;
  word e_machine;
  dword e_version;
  dword e_entry; /* entry point */
  dword e_phoff; /* Program Header Offset */
  dword e_shoff; /* Section Header Offset */
  dword e_flags;
  word e_ehsize; /* Header size */
  word e_phentsize; /* Program Header Entry Size */
  word e_phnum; /* Program Header Entry Number */
  word e_shentsize; /* Section Header Entry Size */
  word e_shnum;/* Section Header Entry Number */
  word e_shstindex; /* Section name string index */
} ELF_Header; 

typedef 
struct __ELF_Program_Header {
  dword p_type;        /* Kind of the segment */
  dword p_offset;      /* Offset from start of file */
  dword p_vaddr;       /* Virtual Address of start */
  dword p_vpaddr;      /* Physical Address of start */
  dword p_filesz;      /* File Image size of segment */
  dword p_memsz;       /* Memory Image size of segment */
  dword p_flags;       /* Attributes */
  dword p_align;       /* Alignment */
} ELF_Program_Header;


#define PT_NULL         0          /* UNUSED */
#define PT_LOAD         1          /* Loadable Segment */
#define PT_DYNAMIC      2          /* Dynamic linking */
#define PT_INTERP       3          /* Interpreter Name */
#define PT_NOTE         4          /* Note Section */
#define PT_SHLIB        5          /* Reserved */
#define PT_PHDR         6          /* Location And Size of Program Header */

/*
dword ReadELF(byte * filename) {
  ELF_Header Header;

  //Should be at most 2, so 10 is to be sure
  ELF_Program_Header PHeader[10];

  int i;

  // if (strncmp(((char *) &Header)+1,"ELF",3)!=0) {
  //   printf("BAD ELF Header!\n");
  //   return 0;
  // }
  // Prototype: ReadFile ( filename, starting offset,
  //       destination address, number of bytes to be read )
  
  ReadFile(filename,0,&Header,sizeof(Header));
  ReadFile(filename,Header.e_phoff,PHeader,Header.e_phnum*Header.e_phentsize);
  for (i=0;i<Header.e_phnum;i++) {
    if (PHeader[i].p_type==PT_LOAD) {
      ReadFile(filename,PHeader[i].p_offset,PHeader[i].p_vaddr,PHeader[i].p_filesz);
      if (PHeader[i].p_filesz<PHeader[i].p_memsz) {
	//Prototype: BZero(Start address, number of bytes)
	//sets memory region to zero
	BZero(PHeader[i].p_vaddr+PHeader[i].p_filesz,PHeader[i].p_memsz-PHeader[i].p_filesz);
      }
    }
  }

  return Header.e_entry; 
  // returns entry point of the Kernel
  // We'll have to use this to boot.
}
*/

void main() {
  int starter_cluster, kernel_cluster, modulesdir_cluster;
  ulong starter_size, kernel_size;
  void *Kernel;

#ifdef SERIAL_LOGGING
 #ifdef SERIAL_LOG_BOOT
  readbytes(0x40,(SERIAL_LOG_PORT*2)+5,&Porta_Pronti,2);
  readbytes(0x40,(SERIAL_LOG_PORT*2),&Porta_OUT,2);
 #endif
#endif

  color_prints(Yellow|(Blue<<4)," ----------------------------- OSSO Kernel Loader ----------------------------- \n\r");

  OPERATION("Checking for i386...              ");
  if (!check386()) {
    OPERATION_FAIL;
    COMMENT("\n\r  Sorry, OSSO requires i386+\n\r");
    PANIC;
  }
  OPERATION_OK;

  OPERATION("Getting disk data...              ");
  GetBootDiskData();
  OPERATION_OK;

  ReadConfig();

  StartKernel();

  while (1);
}
