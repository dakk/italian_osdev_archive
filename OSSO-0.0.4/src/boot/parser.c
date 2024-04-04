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
 * file : $Source: /cvsroot/osso/OSSO/src/boot/parser.c,v $
 * Description: Parser generic functions.
 ***************************************************************************

 ***************************************************************************
 * $Id: parser.c,v 1.6 2001/08/18 15:18:29 steros Exp $
 ***************************************************************************
 *
 * $Log: parser.c,v $
 * Revision 1.6  2001/08/18 15:18:29  steros
 * Serial driver: new version. It has already to be tested.
 *
 * Revision 1.5  2001/02/24 14:11:51  steros
 * Removed limitations.
 *
 * Revision 1.4  2001/02/23 17:41:41  steros
 * Single pass-parser implemented. Inserted segment interval in INITCONF.
 * Inserted checking for modules overflow. Fixed some parser bugs.
 *
 * Revision 1.3  2001/02/12 17:57:59  steros
 * Booting process: the parser is now not destructive!
 *
 * Revision 1.2  2001/01/11 10:04:29  steros
 * Fixed headers.
 *
 *
 ***************************************************************************/


#include "parser.h"

#ifndef NULL
  #define NULL 0
#endif



ElementName Sections[] = {
  { S_CONFIG, "config" },
  { S_BOOT,   "boot"   },
  { S_INIT,   "init"   },
  { 0, 0 }     /* Warning: do not remove! */
};


ElementName Instructions[] = {
  { I_BOOT_SEGMENT,    "boot_segment"    },
  { I_PRELOAD_SEGMENT, "preload_segment" },
  { I_BOOT_DIR,        "boot_dir"        },
  { I_MODULES_DIR,     "modules_dir"     },
  { I_INSTALL,         "install"         },
  { I_PRELOAD,         "preload"         },
  { I_START,           "start"           },
  { I_LOAD,            "load"            },
  { I_RUN,             "run"             },
  { 0, 0 }     /* Warning: do not remove! */
};



/* Parser global variables. */
char *parser_buffer=0;     /* Buffer containing init file.               */
char *parser_finebuffer;   /* Pointer to the next element in the buffer. */
int parser_line;           /* Current line number.                       */
static char *parser_p=NULL;        /* Pointer to current line in buffer. */
static char *parser_pp=NULL;       /* Pointer to last line in buffer.    */

/* Data for non-destructive use. */
static char *parser_lineP=0, *parser_tokenP=0;   /* Character pointers. */
static char parser_lineVal, parser_tokenVal;     /* Character values.   */





/*
 Init the parser assigning initconf address.
 This must be the first function to call!
 If used by kernel-loader, last parameter indicate if line number must be
 resetted (if 1) or mantained (if 0).
*/
#ifdef _KERNEL_LOADER_
void InitParser(char *initconf_address, int dim, int resetline) {
#else
void InitParser(char *initconf_address, int dim) {
#endif
  parser_buffer=initconf_address;
  parser_finebuffer=parser_buffer+dim;
  parser_p=parser_buffer;
#ifdef _KERNEL_LOADER_
  if (resetline)
    parser_line=0;
  else
    parser_line--;
#else
  parser_line=0;
#endif
  /* Restore changed characters. */
  if (parser_tokenP) {
    *parser_tokenP=parser_tokenVal;
    parser_tokenP=0;
  }
  if (parser_lineP) {
    *parser_lineP=parser_lineVal;
    parser_lineP=0;
  }
}


/*
 Copy n bytes from a memory area to another.
*/
void Strncpy(char *dest, char *source, int num) {
  for (; num; num--, dest++, source++) *dest=*source;
}


/*
 Compares 2 strings with case insensitive matching.
 Returns 0 if different, 1 if equal.
*/
int StringEqual(char *str1, char *str2) {
  char diff='A'-'a', c1, c2;

  for (; *str1 && *str2; str1++, str2++) {
    c1=(*str1>='A' && *str1<='Z') ? (*str1)-diff : *str1;
    c2=(*str2>='A' && *str2<='Z') ? (*str2)-diff : *str2;
    if (c1!=c2) return 0;
  }
  if (*str1=='\0' && *str2=='\0') return 1;
  return 0;
}


/*
  Check if a char is in a string.
  Returns 0 if not found, otherwise returns the position starting from 1
*/
int is_in(char carattere, char *stringa) {
  int cont;

  for (cont=1; *stringa; stringa++, cont++)
    if (carattere==*stringa) return cont;
  return 0;
}


/*
  Returns a valid line or NULL if end reached.
  If it is used by kernel-loader, and 'finebuffer' returns a non zero value
  the buffer is finished and 'finebuffer' report the start address of the
  current line.
*/
#ifdef _KERNEL_LOADER_
char *_GetLine(char **finebuffer) {
#else
char *GetLine(void) {
#endif
  char *pi;   /* Pointer to start of line.  */
  char *s;    /* Pointer to end of line.    */
  char *s2;   /* Pointer to a comment char. */
  int ok=0;

  /* Restore changed characters. */
  if (parser_tokenP) {
    *parser_tokenP=parser_tokenVal;
    parser_tokenP=0;
  }
  if (parser_lineP) {
    *parser_lineP=parser_lineVal;
    parser_lineP=0;
  }
  pi=parser_p;
  /* Loop until it finds an invalid line or the buffer ends. */
  do {
    parser_line++;
    /* Finding start of line. */
    for (; IS_SPACING(*pi) && pi<parser_finebuffer; pi++);
    if (pi>=parser_finebuffer || !pi) {
#ifdef _KERNEL_LOADER_
      *finebuffer=parser_finebuffer;
#endif
      return NULL;
    }
    /* Finding end of line. */
    for (s=pi, s2=NULL; *s!='\n' && s<parser_finebuffer; s++)
      if (*s==COMMENT_CHAR && s2==NULL) s2=s;
    if (s>=parser_finebuffer) {
#ifdef _KERNEL_LOADER_
      *finebuffer=pi;
#endif
      return NULL;
    }
    if (*pi!='\0' && *pi!='\n' && *pi!='\r' && *pi!=COMMENT_CHAR) {
      /* Valid line. */
      ok=1;
    } else {
      /* Comment or empty line. */
      if (s<parser_finebuffer)
        pi=s+1;
      else {
#ifdef _KERNEL_LOADER_
      if (*pi==COMMENT_CHAR)
        *finebuffer=pi;
      else
        *finebuffer=parser_finebuffer;
#endif
        return NULL;
      }
    }
  } while (!ok);
  /* Updating static pointer to next line. */
  parser_p=s+1;
  /* Finds logic end of line. */
  if (s2) s=s2;
  for (s--; *s=='\r' || IS_SPACING(*s); s--);
  s++;
  /* Save character to change. */
  parser_lineP=s;
  parser_lineVal=*s;
  /* Terminate the line. */
  *s='\0';
  parser_pp=pi;
#ifdef _KERNEL_LOADER_
  *finebuffer=0;
#endif
  return pi;
}


/*
 Returns again last line read.
*/
char *RegetLine(void) {
  if (parser_tokenP) {
    *parser_tokenP=parser_tokenVal;
    parser_tokenP=0;
  }
  return parser_pp;
}


/*
  Mark the end of token with a \0 and returns in 'carattere' the char that
  was there. Updates 'stringa' for the next token and returs the beginning
  of the actual token. 'withoutspaces' ==1 means that 'carattere' must not be
  a space but what is next.
  If spec_delimiters is not 0, will be used these instead of standard ones.
*/
char *GetToken(char **stringa, char *carattere, char withoutspaces,
               char *spec_delimiters)
{
  char *start=*stringa;   /* Contains the beginning of the string. */
  char *d;   /* Keep track of the first delimiter met. */
  char *Delim=(spec_delimiters==NULL || *spec_delimiters=='\0') ?
              DELIMITERS : spec_delimiters;

  /* Restore changed character. */
  if (parser_tokenP) {
    *parser_tokenP=parser_tokenVal;
    parser_tokenP=0;
  }
  /* Find the first delimiter. */
  for (; **stringa!='\0' && !is_in(**stringa,Delim); (*stringa)++);
  d=*stringa;
  /* If it is a space, look next. */
  SKIP_SPACES(*stringa);

  *carattere=withoutspaces ?
    (((!IS_SPACING(**stringa) && is_in(**stringa,Delim)) || **stringa=='\0') ?
      **stringa : *d) : *d;
  /* If necessary, look for the next token. */
  if (**stringa && is_in(**stringa,Delim)) {
    (*stringa)++;
    SKIP_SPACES(*stringa);
  }
  /* Save character to change. */
  parser_tokenP=d;
  parser_tokenVal=*d;
  /* Terminate the string. */
  *d='\0';
  return start;
}


/*
  Returns the type of the instruction or NO_INSTRUCTION in case of error.
*/
int FindInstruction(char *istr) {
  ElementName *e;

  for (e=Instructions; e->name; e++)
    if (StringEqual(istr,e->name)) return e->type;
  return NO_INSTRUCTION;
}


/*
  Returns the type of the section or NO_SECTION in case of error.
*/
int FindSection(char *s) {
  ElementName *e;
  char *end, ch;

  /* Removing signs of section. */
  if (*s=='[') s++;
  for (end=s; *end; end++);
  end--;
  if (*end==']') {
    ch=*end;
    *end='\0';
  }
  /* Search for the section name. */
  for (e=Sections; e->name; e++) {
    if (StringEqual(s,e->name)) {
      *end=ch;
      return e->type;
    }
  }
  *end=ch;
  return NO_SECTION;
}
