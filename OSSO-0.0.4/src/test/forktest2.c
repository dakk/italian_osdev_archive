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

/*
  This program tests the variables both global and on the stack for correct
  value maintaining between forks
*/

int globaldata1;
int riglobdata2=321899;

int main(void) {
  char stringa[30]="Hello World!";

  globaldata1=55667788;
  
  printf("Fork() correctness test mypid is %d, myfather is %d\n",getpid(),getppid());
  
  printf("before fork(): globdata1 = %d\n",globaldata1);
  printf("before fork(): globdata2 = %d\n",riglobdata2);
  printf("before fork(): Hello World string = '%s'\n",stringa);

  {
    int onstackdata;
    int newpid;
    onstackdata=77;
    printf("before fork(): onstackdata = %d\n",onstackdata);
    printf("Forking ...\n");
    if ((newpid=fork())==0) {
      printf("I am the child with pid = %d, myfather is %d\n",getpid(),getppid());

      printf("Child after fork(): globdata1 = %d\n",globaldata1);
      printf("Child after fork(): globdata2 = %d\n",riglobdata2);
      printf("Child after fork(): Hello World string = '%s'\n",stringa);
      printf("Child after fork(): onstackdata = %d\n",onstackdata);
      
    } else {
      printf("I am the father with pid = %d, result of fork is %d\n",getpid(),newpid);

      printf("Father after fork(): globdata1 = %d\n",globaldata1);
      printf("Father after fork(): globdata2 = %d\n",riglobdata2);
      printf("Father after fork(): Hello World string = '%s'\n",stringa);
      printf("Father after fork(): onstackdata = %d\n",onstackdata);      
    }
    
  }
  

  return 0;
}
