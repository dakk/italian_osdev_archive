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

int main (void) {
  char * ppio;
  char text[]="Testo di prova per il test dei messaggi\n";
  int i;
  printf("Processo di prova: pid == %d\n",getpid());
  printf("Prendo un messaggio... :\n");
  ppio=getbmsg();
  printf("Preso a %x\n",ppio);
  for (i=0;text[i]!=0;i++) ppio[i]=text[i];
  ppio[i]=text[i];
  printf("Spedisco Messaggio al processo 8 ...\n");
  send_msg(8,ppio,100);
  printf("Fatto.\n");
}
