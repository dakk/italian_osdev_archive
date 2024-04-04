/*
ItalianOs

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

#ifndef  _STRING_H_
#define _STRING_H_

#include <stddef.h>

char *strcpy(char *dst, char *src);
char *strncpy(char *dst,const char *src,int n);
int strcmp(const char *s1,const char *s2);
int strncmp(const char *s1,const char *s2,int n);
int strlen(const char *s);
char *strscn(char *s,char *pattern);
char *strchr(char *s,int c);
char *strupr(char *s);
char *strlwr(char *s);
char *strcat(char *dst,char *src);
int memcmp(const void *s1, const void *s2, size_t n);
void *memset(void *p, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
void itoa(int n, char s[]);
char *utoa(unsigned value, char *digits, int base);


#endif
