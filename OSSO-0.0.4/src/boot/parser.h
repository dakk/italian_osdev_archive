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
 * file : $Source: /cvsroot/osso/OSSO/src/boot/parser.h,v $
 * Description: Header for parser generic functions.
 ***************************************************************************

 ***************************************************************************
 * $Id: parser.h,v 1.4 2001/08/18 15:18:29 steros Exp $
 ***************************************************************************
 *
 * $Log: parser.h,v $
 * Revision 1.4  2001/08/18 15:18:29  steros
 * Serial driver: new version. It has already to be tested.
 *
 * Revision 1.3  2001/02/23 17:41:41  steros
 * Single pass-parser implemented. Inserted segment interval in INITCONF.
 * Inserted checking for modules overflow. Fixed some parser bugs.
 *
 * Revision 1.2  2001/01/11 10:04:29  steros
 * Fixed headers.
 *
 *
 ***************************************************************************/


#ifndef _PARSER_H_
#define _PARSER_H_


#define DELIMITERS         "\t ,:;-=(){}"
#define COMMENT_CHAR       '#'
#define INSTR_SEPARATOR    '='
#define SEGMENT_SEPARATOR  '-'

#define IS_SPACING(c)   ((c)==' ' || (c)=='\t')
#define SKIP_SPACES(s)   for (; IS_SPACING(*(s)); (s)++);


typedef struct {
  int type;
  char *name;
} ElementName;


/* Implemented sections. */

enum SectionTypes {
  NO_SECTION = -1,
  S_CONFIG,
  S_BOOT,
  S_INIT
};


/* Implemented instructions. */

enum InstructionTypes {
  NO_INSTRUCTION = -1,
  I_BOOT_SEGMENT,
  I_PRELOAD_SEGMENT,
  I_BOOT_DIR,
  I_MODULES_DIR,
  I_INSTALL,
  I_PRELOAD,
  I_START,
  I_LOAD,
  I_RUN
};



extern int parser_line;   /* Linea corrente: usare solo in lettura! */


/* Exported functions. */

void InitParser(char *initconf_address, int dim);
char *GetLine(void);
char *RegetLine(void);
char *GetToken(char **stringa, char *carattere, char senzaspazi,
               char *spec_delimiters);
int FindSection(char *istr);
int FindInstruction(char *istr);


#endif
