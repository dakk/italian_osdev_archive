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

/* This program tests the behaviour of threads */

Thread_Function StampaInt(unsigned int val) {
  printf("Thread Created with val = %d\n",val);
}

int main(void) {
  unsigned int conta,conta2;
  int t;
  printf("Main thread ...\n");
  
  printf("Creating first thread with 9 ...\n");
  t=Create_Thread(StampaInt,9);
  printf("First thread done %d\n",t);
  printf("Creating second thread with 1278 ...\n");
  t=Create_Thread(StampaInt,1278);
  printf("Second thread done %d\n",t);
  printf("Creating third thread with 34569 ...\n");
  t=Create_Thread(StampaInt,34569);
  printf("Third thread done %d\n",t);

  for (conta2=0;conta2<-1;conta2++)
    conta=conta*conta2/conta;
  return 0;
}
