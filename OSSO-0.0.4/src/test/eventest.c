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
  int pid;

  pid=getpid();
  printf("[PROVONE] AO BBello! So er pid %d!\n",pid);

  printf("[PROVONE] creo l'evento\n");
  ritorno=getevent("PROVA");
  if(ritorno!=0){
    printf("[PROVONE] Errore numero %d!\n",ritorno);
    exit;
  }
  
  printf("[PROVONE] Mi segnalo l'evento\n");
  ritorno=signal(pid,"PROVA");
  if(ritorno!=0){
    printf("[PROVONE] Errore numero %d!\n",ritorno);
    exit;
  }

  printf("[PROVONE] Aspetto r'mi evento\n");
  ritorno=wait("PROVA",&pid);
  if(ritorno!=0){
    printf("[PROVONE] Errore numero %d!\n",ritorno);
    exit;
  }

  printf("[PROVONE] Tronco l'evento\n");
  ritorno=clearevent("PROVA");
  if(ritorno!=0){
    printf("[PROVONE] Errore numero %d!\n",ritorno);
    exit;
  }
  printf("[PROVONE] Festa finita!\n");

  return 0;
}









