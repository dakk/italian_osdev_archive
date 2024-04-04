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


#include <printf.h>
#include <processes.h>
#include <events.h>

char evento[16]="CATENA";
int numero=10; /* TO CHANGE AT PLAISIR */

void figlia(int num){
  int figlio,babbo;
  int io,pid,ultimo,ritorno;
  
  /* Codice comune ... */
  io=getpid();
  babbo=getppid();
  
  printf("[ >:-{ ] Sono %d e creo l'evento\n",io);
  ritorno=getevent(evento);
  if(ritorno!=0){
    printf("[ >:-{ ] %d: Errore numero %d getevent!\n",io,ritorno);
    exit();
  }
  /* Sono l'origine */
  if(num==numero){
    ultimo=io+numero; /* Guess the pid of the last ... */

    figlio=fork();
    if(figlio!=0){
      while(1){
	//printf("[ >:-{ ] Sono %d e aspetto...\n",io);      
	ritorno=wait(evento,&pid);
	if(ritorno!=0){
	  printf("[ >:-{ ] %d: Errore numero %d su WAIT!\n",io,ritorno);
	  exit();
	}
	printf("[ >:-{ ] Sono %d (l'origine!) e saluto %d...\n",io,ultimo);
	ritorno=signal(ultimo,evento);
	if(ritorno!=0){
	  printf("[ >-:{ ] %d: Errore numero %d su signal!\n",io,ritorno);
	  exit();
	}
      }
    } else {
      figlia(num -1);
    }
  }
  /* Sono l'ultimo */
  if(num==0){
    /* CON QUESTO SI FINISCE */
    printf("[ >:-{ ] %d: Ultimo del treno!\n",io); 
    /* 'spettamme nu poco...*/
    for(pid=0;pid<6000000;pid++);
    while(1){
      printf("[ >:-{ ] Sono %d e saluto Babbo %d...\n",io,babbo);
      ritorno=signal(babbo,evento);
      if(ritorno!=0){
	printf("[ >-:{ ] %d: Errore numero %d su signal!\n",io,ritorno);
	exit();
      }
      //printf("[ >:-{ ] Sono %d e aspetto...\n",io);      
      ritorno=wait(evento,&pid);
      if(ritorno!=0){
	printf("[ >:-{ ] d: Errore numero %d su WAIT!\n",io,ritorno);
	exit();
      }
    }
    
  }else {
    /* RICORRIAMO... */

    figlio=fork();
    if(figlio!=0){
      /* Point of non-return */
      while(1){
	//printf("[ >:-{ ] Sono %d e aspetto...\n",io);      
	ritorno=wait(evento,&pid);
	if(ritorno!=0){
	  printf("[ >:-{ ] %d: Errore numero %d su WAIT!\n",io,ritorno);
	  exit();
	}
	printf("[ >:-{ ] Sono %d e saluto Babbo %d...\n",io,babbo);
	ritorno=signal(babbo,evento);
	if(ritorno!=0){
	  printf("[ >-:{ ] %d: Errore numero %d su signal!\n",io,ritorno);
	  exit();
	}
      } /* END WHILE */
      
    } else {
      figlia(num -1);
    }    
  }
}



/* MAIN body */
int main(void) {

  printf("[ >:-{ ] A very evil test is going to begin!!!!\n");
  /* Start the cicle */
  figlia(numero);
  return 0;
}









