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

typedef unsigned size_t;

#ifndef NULL
	#define NULL 0
#endif

char *strchr (const char *s, int c);
size_t strlen (const char *s);
char *strcpy (char *s1, const char *s2);
char *strncpy (char *dest, const char *src, size_t n);
char *strcat (char *s1, const char *s2);
void *memset (void *p, int c, size_t n);
void *memcpy (void *dest, const void *src, size_t n);
int memcmp (const void *s1, const void *s2, size_t n);
int strcmp(const char *str1, const char *str2);
void itoa(int n, unsigned char s[]);
unsigned  char* strstr(unsigned char* orig, unsigned char *pattern);

#endif

