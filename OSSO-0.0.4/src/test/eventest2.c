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
//#include <signals.h>
//#include <memory.h>
#include <events.h>

int main(void) {
  int ritorno;
  int pid,babbo;
  char evento[14]="Ciao Babbo!";

  printf("[EVENTEST2] Ciao mondo, sono babbo!\n");

  printf("[EVENTEST2] Creo l'evento\n");
  ritorno=getevent(evento);
  if(ritorno!=0){
    printf("[EVENTEST2] Errore numero %d!\n",ritorno);
    exit;
  }

  printf("[EVENTEST2] Sto per figliare...\n");
  if(fork()!=0){
    pid=getpid();
    printf("[EVENTEST2] Salve Bimbo!, sono babbo %d\n",pid);

    printf("[EVENTEST2] Aspetto che 'r bimbo mi saluti\n");
    ritorno=wait(evento,&pid);
    if(ritorno!=0){
      printf("[EVENTEST] Errore numero %d!\n",ritorno);
      exit;
    }

    printf("[EVENTEST2] Che bello: m'ha salutato!\n");   
    printf("[EVENTEST2] Tronco l'evento\n");
    ritorno=clearevent(evento);
    if(ritorno!=0){
      printf("[EVENTEST2] Errore numero %d!\n",ritorno);
    exit;
    }
    printf("[EVENTEST2] Festa finita!\n");
    
  } else {
    /* Figlio: 'spettamme nu poco...*/
    for(pid=0;pid<6000000;pid++);
    
    babbo=getppid();
    printf("[FIGLIO] Saluto Babbo...\n");
    ritorno=signal(babbo,evento);
    if(ritorno!=0){
      printf("[FIGLIO] Errore numero %d!\n",ritorno);
      exit;
    }
    printf("[FIGLIO] Festa finita!\n");
  }

  return 0;
}









