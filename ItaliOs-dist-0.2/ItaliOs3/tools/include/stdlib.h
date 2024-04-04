	/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _STDLIB_H_
#define  _STDLIB_H_

#include <stddef.h>

#define RAND_MAX        2147483647


int rand(void);

void srand(unsigned int seed);



unsigned abs(int x);

unsigned ecvt(double v,char *buffer,int width,int prec,int flag);
unsigned fcvt(double v,char *buffer,int width,int prec,int flag);
unsigned gcvt(double v,char *buffer,int width,int prec,int flag);
unsigned dcvt(long v,char *buffer,int base,int width,int flag);
unsigned ucvt(unsigned long v,char *buffer,int base,int width,int flag);


long strtoi(char *s,int base,char **scan_end);
unsigned long strtou(char *s,int base,char **scan_end);
double strtod(char *s,char **scan_end);
long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);



#define atof(s)	strtod(s, NULL)
#define atoi(s)	strtoi(s, 10, NULL)
#define atou(s)	strtou(s, 10, NULL)
#define atol(s) strtol(s, 10, NULL)



int max(int a, int b);
int min(int a, int b);



void *malloc(size_t dim);

void free(void * addr);

//Feature oscura realloc(NULL, size) == malloc(size)
//                                 realloc(pointer, 0) == free(pointer)
void *realloc(void *pointer, size_t size);

void *calloc(size_t nmemb, size_t size);

char *getenv(const char *name);
int setenv(const char *name, const char *value, int overwrite);

#endif
