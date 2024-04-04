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
#include <signals.h>
#include <memory.h>

/* This program tests the amount of free memory before and after some forks */

int main(void) {
  //  unsigned long int pid;
  //  int fd,rit1,rit2;
  //  char stringa[30]="Hello World!";
  //  char stringa2[30];
  int conta,conta2;

  printf("FrEe PaGeS :%d\n",get_free_pages());

  printf("Forking ...\n");
  
  for (conta=0;conta<165;conta++) {
    if (fork()==0) {
      if (fork()==0) {
	if (fork()==0)
	  exit();
	else
	  exit();
      } else
	//for (conta2=0;conta2<10;conta2++)  if (fork()==0) exit();
	exit();
    }
    if (conta%10==0) printf("Reached %d\n",conta);
  }
  
  //  if (fork()==0) exit();
  //  if (fork()==0) exit();
  //fork();
  //fork();
  for (conta=0;conta<5000000;conta++) __asm__("nop\n\t nop\n\t nop\n\t");
  printf("Free Pages after fork():%d\n",get_free_pages());


  printf("Fine fork test (pid=%d)\n",getpid());
  printf("FRee PAgeS :%d\n",get_free_pages());
  //releasecharbuffer(fd);
  return 0;
}
