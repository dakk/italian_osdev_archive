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
#include <messages.h>

/* Message passing test program */

int main (void) {
  char * ppio, * res;
  msginfo mi;
  char text[]="Testo di prova per il test dei messaggi";
  int i,x;
  int figlio;

  recv_msg(&mi); /* To make mps happy */

  printf("Processo di prova: pid == %d\n",getpid());
  if ((figlio=fork())==0) {
    char myanswer[]="La mia risposta !!";
    x=0;
    printf("Figlio : Sono il figlio %d\n",getpid());
    //    for (i=0;i<1012314;i++) {
    //      for (x=0;x<234578;x++) figlio=x/2+i;
    //    }
    while (1) {
      x++;
      ppio=recv_msg(&mi);
      printf("Figlio : Messaggio ricevuto da %d\n",mi.mittente);
      printf("Figlio : Testo del messaggio n. %d '%s'\n",x,ppio);
      res=getbmsg();
      for (i=0;myanswer[i];i++) res[i]=myanswer[i];
      myanswer[i]='\0';
      if ((send_msg(mi.mittente,res,100))!=0) {
	printf("Figlio : Errore in send\n");
	while (1) ;
      }
    }
    return 0;
  }
  printf("Padre : Prendo un messaggio... :\n");
  for (x=0;x<5;x++) {
    ppio=getbmsg();
    //    printf("Padre : Preso a %x\n",ppio);
    for (i=0;text[i]!=0;i++) ppio[i]=text[i];
    ppio[i]=text[i];
    printf("Padre : Spedisco Messaggio al processo figlio (%d) ...\n",figlio);
    if (i=send_msg(figlio,ppio,100)) {
      printf("***** SendMSG !=0 (%d)\n",i);
      while (1) ;
    }
    ppio=recv_msg(&mi);
    printf("Padre : Ricevuto Messaggio dal processo (%d) '%s'\n",mi.mittente,ppio);
    
  }
  printf("Padre : Fatto.\n");
}


