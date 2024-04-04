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
 * file : boot/init.c                                                      *
 * Guess what ? INIT !                                                     *
 ***************************************************************************

 ***************************************************************************
 * $Id: init.c,v 1.11 2001/08/18 15:18:29 steros Exp $
 ***************************************************************************
 *
 * $Log: init.c,v $
 * Revision 1.11  2001/08/18 15:18:29  steros
 * Serial driver: new version. It has already to be tested.
 *
 * Revision 1.10  2001/05/01 13:47:38  steros
 * Improved MPS.
 *
 * Revision 1.9  2001/05/01 01:04:19  iurlano
 * Work on Dynamic Linker problem
 * Seems like that the shared MPS_Pid is working now.
 * I thing that the condivision of a variable from the lib and the program works if the variable is declared in the program.
 * for the other case, I've to work on it and I don't think I'm going to do it very soon.
 *
 * Revision 1.8  2001/04/30 17:32:22  steros
 * Now INIT wait for MPS pid global setting.
 *
 * Revision 1.7  2001/04/20 20:45:35  steros
 * Added GetDriverPid function for driver pid retrival from MPS.
 *
 * Revision 1.6  2001/04/05 21:06:13  iurlano
 * Completed xconfig modifies. Now requires the tix package (Sorry but tcl/tk doesn't provide a NoteBook widget :-(( )
 *
 * Revision 1.5  2001/03/25 15:27:25  steros
 * Divided MPS interfaces in more structured files.
 *
 * Revision 1.4  2001/03/10 17:08:48  steros
 * Added header and lib functions to interface driver to MPS.
 *
 * Revision 1.3  2001/02/23 17:41:41  steros
 * Single pass-parser implemented. Inserted segment interval in INITCONF.
 * Inserted checking for modules overflow. Fixed some parser bugs.
 *
 * Revision 1.2  2001/01/11 10:04:29  steros
 * Fixed headers.
 *
 * Revision 1.1  2001/01/08 09:52:54  steros
 * New boot procedure implemented.
 *
 * Revision 1.6  2000/12/08 16:45:27  iurlano
 * Thread Work in progress.
 * First working version reached. Needs Debug.
 *
 * Revision 1.5  2000/10/25 19:59:38  iurlano
 * Added automatic module name printing in printf and printk
 * Added little ELF check in process.c
 *
 * Revision 1.4  2000/10/16 18:41:54  iurlano
 * Added prototype of rtl8139 network card driver
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
 * Revision 1.1.1.1  2000/05/30 08:11:01  iurlano
 * OSSO : First Public Release
 *
 * Revision 1.12  2000/03/26 13:20:23  iurlano
 * Completamento protocollo di INIT
 * Da completarne il test
 *
 * Revision 1.11  2000/03/23 08:40:35  rosell
 * *** empty log message ***
 *
 * Revision 1.10  2000/03/22 23:16:02  iurlano
 * Correzioni drivers
 *
 * Revision 1.9  2000/03/22 16:44:34  iurlano
 * Sviluppo drivers
 *
 * Revision 1.8  2000/03/22 08:37:02  rosell
 * Modificato l'inserimento dei parametri del modulo.
 *
 * Revision 1.7  2000/03/21 16:44:26  iurlano
 * *** empty log message ***
 *
 * Revision 1.6  2000/03/21 10:30:04  iurlano
 * Completato protocollo di init e di ide. Necessita di molto debug
 *
 * Revision 1.5  2000/03/20 12:01:40  iurlano
 * lavoro su drivers
 *
 * Revision 1.4  2000/03/13 08:37:40  rosell
 * Definite funzioni di base per il parsing e struttura grammatica.
 * Mancano da implementare le varie funzioni.
 *
 * Revision 1.3  2000/03/08 09:55:33  iurlano
 * Modularizzazione di init in corso
 *
 * Revision 1.2  2000/03/07 21:37:52  iurlano
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2000/01/19 10:42:40  pecciant
 *
 *
 * Revision 1.1  2000/01/17 23:44:41  iurlano
 * Aggiunto INIT
 *
 *
 ***************************************************************************/

#define __PRINT_MODULE_NAME__ "INIT"

#include <io.h>
#include <printf.h>
#include <processes.h>
#include <signals.h>
#include <messages.h>
#include <memory.h>
#include <string.h>
#include "parser.h"
#include <protocols/mpsint.h>
#include <protocols/drv2mps.h>
#include "../Config_Boot.h"


#define MPS_NAME "mps"


extern long int MPS_Pid;

char *ModulesDir=0;



/*
 Send a message to mps.
*/
void SendToMPS(mps_request_type type, PID pid) {
  mps_msg_t *msg;

  msg=getbmsg();
  msg->Class=MPS_CLASS;
  msg->type=type;
  msg->pid=pid;
  send_msg(MPS_Pid,msg,sizeof(mps_msg_t));
}


/*
 Send start message to drivers.
*/
char Initialize(PID pid, char *param, int param_size) {
  drvmps_start_type *messaggio;

  messaggio=getbmsg();
  messaggio->intest.Type=DRVMPS_START;
  messaggio->pid=MPS_Pid;
  memcpy(messaggio->data,param,param_size);
  send_msg(pid,messaggio,param_size+sizeof(PID)+sizeof(drvmps_request_type));
  return 0;
}


/*
 The strcasecmp() of library raise a segmentation fault!!!
*/
int mystrcasecmp(const char *s1, const char *s2) {
  const char *p1=s1, *p2=s2;
  char diff='A'-'a', c1, c2;
  int ret;

  c1=(*p1>='A' && *p1<='Z') ? (*p1)-diff : *p1;
  c2=(*p2>='A' && *p2<='Z') ? (*p2)-diff : *p2;
  ret=c1-c2;
  if (!*p1 || !*p2) return ret;
  for (p1++, p2++; !ret && *p1 && *p2; p1++, p2++) {
    c1=(*p1>='A' && *p1<='Z') ? (*p1)-diff : *p1;
    c2=(*p2>='A' && *p2<='Z') ? (*p2)-diff : *p2;
    ret=c1-c2;
  }
  return ret;
}


/*
 Return 1 if name correspond to MPS module.
 Else return 0.
*/
int isMPS(char *name) {
  char *p;

  /* Find the end of the string. */
  for (p=name; *p; p++);
  if (p==name) return 0;
  /* Find the first char, excluding path. */
  for (p--; p>name && *p!='/'; p--);
  if (*p=='/') p++;
  /* Check the name. */
  if (!mystrcasecmp(p,MPS_NAME)) return 1;
  return 0;
}


/*
 This function run the modules preloaded by boot process.
*/
void RunModules(void *address) {
  char *modpointer, *name, *param;
  int dim, ciclomoduli, nmoduli, namelen, attrib, param_len;
  PID pid;

  modpointer=address;
  nmoduli=*((int*)modpointer);
  printf("Number of modules: %d\n",nmoduli);
  modpointer+=4;
  printf("Modules start: %x\n",modpointer);
  printf("Searching for modules to run...\n");
  for (ciclomoduli=0; ciclomoduli<nmoduli; ciclomoduli++) {
    namelen=*modpointer;
    modpointer+=4;
    name=modpointer;
    attrib=*((word*)(modpointer+namelen));
    printf("Module \"%s\"... ",name);
    param_len=*((word*)(modpointer+namelen+4));
    modpointer+=namelen+8;
    param=modpointer;
    modpointer+=param_len;
    dim=*((word*)modpointer);
    modpointer+=4;
    if (attrib & 1) {
      if (!isMPS(name) && MPS_Pid==-1) {
        printf("I can't execute module without MPS!\n");
        while (1);
      }
      printf("I execute it in %s level mode!\n",
        (attrib & 2) ? "Supervisor" : "User");
      pid=Create_Process_From_ELF(modpointer,
        (attrib & 2) ? SuperVisor_Level : User_Level);
      Signal(pid,SIGNAL_CONT);
      if (isMPS(name)) {
        while (MPS_Pid==-1) MPS_Pid=Get_MPS_Pid();
      }
      Initialize(pid,param,param_len);
      if (attrib & 4)
        SendToMPS(MPS_WAITFORDRIVER,pid);
    } else
      printf("not executable!\n");
    modpointer+=dim;
  }
  SendToMPS(MPS_INIT_COMPLETE,0);
}


/*
 Error management function.
*/
void ParserError(char *errore) {
  printf("*** Parser error: %s in line %d.\n",errore,parser_line);
  while (1);   /* BREAK!!! */
}


/*
  Prepare parameters and returns a structure
*
INIT_COMMAND *GetParam(char *linea, char sep) {
  INIT_COMMAND *messaggio;
  char *p;
  char *token;

  messaggio=getbmsg();
  p=messaggio->data;
  while (IS_SPACING(sep)) {
    token=GetToken(&linea,&sep,1,NULL);
    // COPY OF TOKEN IN THE STRUCTURE AS A PARAMETER.
    for (; *token; token++, p++) *p=*token;
    *p='\0';
    p++;
    if (sep=='=') {
      token=GetToken(&linea,&sep,1," \t\r\n\0");
      // COPY OF TOKEN IN THE STRUCTURE AS A VALUE.
      for (; *token; token++, p++) *p=*token;
      *p='\0';
      p++;
    } else {
      // COPY OF A DEFAULT VALUE IN THE STRUCTURE.
      *p='\0';
      p++;
    }
  }
  if (sep!='\0') ParserError("syntax error");
  *p='\0';
  p++;
  *p='\0';
  return messaggio;
}
*/

/*
 Load Implementation.
 'linea' and 'sep' are data of the token to be used.
 'tipo' is the type of the load:
   0 - Load;
   1 - Load with Root;
   2 - Run.
*
void Load(char *linea, char sep, int tipo) {
  char *token;
  char * Module;
  int dim;
  unsigned long int pid;
  char ModuleName[50];
  int level;


  level=User_Level;
  while (IS_SPACING(sep)) {
    token=GetToken(&linea,&sep,1,NULL);
    // Here must be processed a parameter of load or run
//...    if (!strcasecmp("root",token)) level=SuperVisor_Level;
  }
  if (sep!=INSTR_SEPARATOR) ParserError("module to be loaded not specified");
  if (*(token=GetToken(&linea,&sep,1,NULL))=='\0') 
    ParserError("module to be loaded not specified");
  printf("module to be loaded <%s>\n   ",token);
  strcpy(ModuleName,token);
  switch (tipo) {
  case 0:
    printf("instruction <LOAD>, ");
	
    if (!Find_Mem_Module(ModuleName,&dim,&Module)) {
      printf("\nWarning!! Module %s not found\n",ModuleName);
    } else {
      printf("Launching %s ...\n",ModuleName);
      pid=Create_Process_From_ELF(Module,level);
      Signal(pid,SIGNAL_CONT);
      Initialize(pid,GetParam(linea,sep));
      printf("End initialization %s.\n",ModuleName);
    }
    break;
  case 1:
    printf("instruction <LOAD 2>, ");
	
    if (!Find_Mem_Module(ModuleName,&dim,&Module)) {
      printf("\nWarning!! Module %s not found\n",ModuleName);
    } else {
      printf("ROOT-Launching %s ...\n",ModuleName);
      pid=Create_Process_From_ELF(Module,SuperVisor_Level);
      Signal(pid,SIGNAL_CONT);
      Initialize(pid,GetParam(linea,sep));
      printf("End INIT %s.\n",ModuleName);
    }
    break;
  case 2:
    printf("instruction <RUN>, ");
	
    if (!Find_Mem_Module(ModuleName,&dim,&Module)) {
      printf("\nWarning!! Module %s not found\n",ModuleName);
    } else {
      pid=Create_Process_From_ELF(Module,SuperVisor_Level);
      Signal(pid,SIGNAL_CONT);
    }
    break;
  default:
    ParserError("?!? This Case should not exist: HELP!!!!!");
    break;
  }
}
*/


/*
 Parser Entry point.
*
void Processa(int dim, char *initconf) {
  char *stringa, *token, sep;
  char *Root=NULL; // , *Modules_dir=NULL;

  InitParser(initconf,dim);
//printf("Ricerca sezione di INIT...\n");
  // Searching for init section.
  while ((stringa=GetLine())!=NULL) {
//printf("Linea> \"%s\"\n",stringa);
    token=GetToken(&stringa,&sep,1,NULL);
    if (token && *token=='[')
      if (FindSection(token)==S_INIT) break;
  }
  if (stringa) {
//printf("Parsing...\n");
    while ((stringa=GetLine())!=NULL) {
      printf("PARSER [line %d] > ",parser_line);
      token=GetToken(&stringa,&sep,1,NULL);
      if (token) {
        if (*token=='[') break;
        switch (FindInstruction(token)) {
          case I_LOAD:
              Load(stringa,sep,Root!=NULL);
              break;
//
          case 1:   // ROOT
              printf("instruction <ROOT>, ");
              if (Root) ParserError("Root already declared");
              if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
              if (*(token=GetToken(&stringa,&sep,1,NULL))=='\0') ParserError("'root' expected");
              if (sep!='\0') ParserError("nothing else was expected");
              printf("parameter <%s>\n",token);
              Root=token;
              break;
          case 2:   // MODULES_DIR
              printf("instruction <MODULES_DIR>, ");
              if (Modules_dir) ParserError("Modules_dir already declared");
              if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
              if (*(token=GetToken(&stringa,&sep,1,NULL))=='\0') ParserError("Directory expected");
              if (sep!='\0') ParserError("nothing else was expected");
              printf("parameter <%s>\n",token);
              Modules_dir=token;
              break;
          case 3:   // RUN
              if (!Root) ParserError("RUN can be only after ROOT");
              Load(stringa,sep,2);
              break;
//
          default:
              ParserError("wrong instruction");
        }
      }
    }
  } else {
    printf("\nINIT section not found! Fatal!!!\n");
    __asm__ ("hlt");
  }
}
*/


/*
 This must be moved in a ctype library file!!!
*/
int toupper(int c) {
  if (c>='a' && c<='z') c+='A'-'a';
  return c;
}


/*
 Read a number from C-format string. The base can be 8, 10 or 16.
 This have to be substituted by strtol() in the future.
*/
unsigned int StringToInt(const char *s) {
  int base=10, molt=1;
  unsigned int num=0;
  char const *p;
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
 Read configuration section of INITCONF.
*/
void *ReadConfig(void) {
  unsigned int dim=*((unsigned int *)(CONF_FILE_SEGMENT*16));
  char *initconf=((char *)(CONF_FILE_SEGMENT*16))+4;
  char *stringa, *token, sep;
  void *PreloadStart=0, *PreloadEnd=0;

  InitParser(initconf,dim);
  /* Searching for configuration section. */
  while ((stringa=GetLine())!=0) {
    token=GetToken(&stringa,&sep,1,NULL);
    if (token && *token=='[')
      if (FindSection(token)==S_CONFIG) break;
  }
  if (stringa) {
    /* Parsing configuration section. */
    while ((stringa=GetLine())!=0) {
      token=GetToken(&stringa,&sep,1,NULL);
      if (token) {
        if (*token=='[') break;
        switch (FindInstruction(token)) {
            case I_PRELOAD_SEGMENT:
                if (PreloadStart)
                  ParserError("preload segment already declared");
                if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
                if (*(token=GetToken(&stringa,&sep,1,NULL))=='\0')
                  ParserError("segment start value expected");
                PreloadStart=(void *)(StringToInt(token)*16);
                if (*(token=GetToken(&stringa,&sep,1,NULL))=='\0')
                  ParserError("segment end value expected");
                PreloadEnd=(void *)(StringToInt(token)*16);
                if (sep!='\0') ParserError("too many parameters");
              break;
          case I_MODULES_DIR:
              if (ModulesDir)
                ParserError("modules directory already declared");
              if (sep!=INSTR_SEPARATOR) ParserError("'=' expected");
              if (*(token=GetToken(&stringa,&sep,1,NULL))=='\0')
                ParserError("directory name expected");
              if (sep!='\0') ParserError("too many parameters");
              if ((ModulesDir=(char *)mallosso(strlen(token+1)))==NULL) {
                printf("ERROR: no more memory avalaible!");
                while (1);
              }
              strcpy(ModulesDir,token);
              break;
        }
      }
    }
  }
  if (!PreloadStart)
    ParserError("Preload segment not declared");
  if (!ModulesDir)
    ParserError("Modules directory not declared");
  return PreloadStart;
}

int ShareInt;

extern int libShareInt;


int main(void) {
  void *PreloadAddress;
  /*  
  ShareInt=1234;
  printf("Value of ShareInt is %d ; ",ShareInt);
  //printf("Stampo il mio pid: %d\n",getpid());
  printf("Calling printShareInt()\n");
  printShareInt();
  printf("printShareInt() called\n");

  printf("The value of libShareInt is %d ; ",libShareInt);

  printf("Address of libShareInt is %x\nAddress of ShareInt is %x\n",&libShareInt,&ShareInt);
  return 0;
  */
  printf("INIT starting !!!\n");

  /* Read the configuration section of configuration file. */
  PreloadAddress=ReadConfig();
  /* Runs preloaded modules. */
  RunModules((void *)PreloadAddress);
//  ...Parsing section [INIT] of INITCONF... (Read it from disk?!?)

  printf("INIT finished.\n");
  //  while (1) ;
  return 0;
}
