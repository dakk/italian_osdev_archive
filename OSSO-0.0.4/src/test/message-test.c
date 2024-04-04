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
  char * ppio;
  msginfo mi;
  char text[]="Testo di prova per il test dei messaggi";
  int i,x;
  int figlio;
  printf("Processo di prova: pid == %d\n",getpid());
  if ((figlio=fork())==0) {
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
    }
    return 0;
  }
  printf("Padre : Prendo un messaggio... :\n");
  for (x=0;x<50;x++) {
    ppio=getbmsg();
    //    printf("Padre : Preso a %x\n",ppio);
    sprintf(ppio,"Test msg number %d",x);
    //    for (i=0;text[i]!=0;i++) ppio[i]=text[i];
    //    ppio[i]=text[i];
    printf("Padre : Spedisco Messaggio al processo figlio (%d) ...\n",figlio);
    if (i=send_msg(figlio,ppio,100)) {
      printf("***** SendMSG !=0 (%d)\n",i);
    }
  }
  printf("Padre : Fatto.\n");
}


