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
#include <malloc.h>

char robetta[5000]="";

int main (void) {
  void * ppio;
  msginfo m;
  ppio=mallosso(90909);
  printf("Processo di prova: pid == %d\n",getpid());
  printf("Attendo un messaggio... :\n");
  while ((ppio=recv_msg(&m))==0) ;
  printf("Messaggio ricevuto : attaccato a %x\n",ppio);
  printf("contiene : %s\n",ppio);
}
