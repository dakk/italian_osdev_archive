/*
 * DreamOS Shell
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *

 * Autore : sgurtz
 * Data   : 25/04/2004
 * Revisione by Lisa
 * Febbraio 2006
 */

/* Headers */
#include <stdlib.h>
#include <commands.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <video.h>
#include <keyboard.h>

/* Inizializza i valori iniziali delle variabili globali */
void init_shell_global_var(void)
{
  argc=0;
  prompt="dsh# ";
  mode=NORMAL;
}

/*
 * Inserisce gli argomenti di un comando in un array di stringhe
 * argc = numero degli argomenti
 * argv[0] = nome del comando
 * argv[n] = opzioni successive
 */
void options(char *com)
{
  int i=0;
  char *temp;
  argc=0;

  temp = (char *)malloc(sizeof(char *));

  for (; *com; com++) {
    if (*com != ' ') {
      *(temp+i) = *com;
      i++;
    }
    else {
      *(temp+i) = '\0';
      argv[argc] = temp;
      argc++;
      temp = (char *)malloc(sizeof(char *));
      i=0;
    }
  }
  *(temp+i) = '\0';
  argv[argc] = temp;  /* "scarica" per l'ultima volta il buffer temp
		       *  se omesso, l'ultimo argomento non sarà salvato
		       */
  argc++;
}

/* corpo della shell */
void dreamshell()
{
  int n=10;
  int d=8;

  /* lista dei comandi*/
  char *com_normal[11] = { "clear", "dreamos", "echo", "floppy", "shutdown", "signal", "math", "tlist", "debug", "help" };
  char *com_debug[9] = { "paging", "gdttest","pingpong", "sem", "syscall", "testchar", "exit", "help"};

  /* routine di servizio dei vari comandi */
  void (*n_serv[10])(void) = { _kclear, info, echo, floppy, shutdown, signal, math, tlist, debug, help };
  void (*d_serv[8])(void) = { paging, gdttest, pingpong, semaphore, syscall, testchar, exit_debug, help };


  char st[255] = {0}; // conterrà il comando scritto sul prompt
  int i=0;
  int p=0;

  _kputs("\nStarting " SHELLVERSION "...\n"
	 "Type 'help' for available commands.\n");

  init_shell_global_var();

  for(;;) {
  restart:
    _kputs(prompt);

    for(;;) {
      int c = _kgetch();
      if(c!=-1) {
	if (p < 253 && (c >= 32 && c <=126)) {
	  st[p++] = c;
	  _kputc(c);
	}
	if (c==8) { // backspace
	  if (p>0) {
	    p--;
	    st[p] = 0;
	    _kbackspace();
	  }
	}
	if (c==13) { // tasto invio

	  st[p] = 0;
	  p = 0;
	  _kputs("\n");
	  if (!strlen(st)) break;
	
	  options(st); // divide il comando dalle opzioni

	  /* ricerca del comando */
	  if (mode==NORMAL) {
	    for (i=n; i>=0; --i) {
	      if(_kstrncmp(argv[0], com_normal[i], strlen(com_normal[i])) == 0) {
		(*n_serv[i])();
	        goto restart;
	      }
	    }
	    printf( "Unknown command '%s'\n", argv[0] );
	    goto restart;
	  }
	  else {
	    for (i=d; i>=0; --i) {
	      if(_kstrncmp(argv[0],com_debug[i], strlen(com_debug[i])) == 0 ) {
		 (*d_serv[i])();
		 goto restart;
	      }
	    }
	    printf( "Unknown command '%s'\n", argv[0] );
	    goto restart;
	  }
	}
      }
    }
  }
}
