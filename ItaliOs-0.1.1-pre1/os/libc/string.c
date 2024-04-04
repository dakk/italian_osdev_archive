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
#include <string.h>
#include <stdio.h>
#include <mem.h>


char *strchr(const char *s, int c)
{
	int p = 0;
	while ((s[p] != c) && (s[p] != '\0'))
		p++;
	if (s[p] == '\0')
		return NULL;
	return (char *) &(s[p]);	/* cast because of CONST char * stuff */
}


size_t strlen(const char *s)
{
	size_t n;
	for (n = 0; *s != '\0'; s++)
		n++;
	return n;
}


char *strcpy(char *s1, const char *s2)
{
	char *p = s1;
	while ((*p++ = *s2++))	/* gcc likes ((...)) :-) */
		;
	return s1;
}


char *strncpy(char *dest, const char *src, size_t n)
{
	char *p = dest;
	int i = 0;
	while ((*p++ = *src++) && (i < n))
		i++;
	for (; i < n; i++)
		*p++ = '\0';
	return dest;
}


char *strcat(char *s1, const char *s2)
{
	char *p = s1;
	while (*p)
		p++;		/* go to the end */
	while ((*p++ = *s2++));	/* copy */
	return s1;
}


void *memset(void *p, int c, size_t n)
{
	int i;
	for (i = 0; i < n; i++)
		*((unsigned char *) p + i) = c;
	return p;
}


void *memcpy(void *dest, const void *src, size_t n)
{
	int i;
	for (i = 0; i < n; i++)
		*((char *) dest + i) = *((char *) src + i);
	return dest;
}


int memcmp(const void *s1, const void *s2, size_t n)
{
	int i;
	for (i = 0; i < n; i++)
		if (*((char *) s1 + i) != *((char *) s2 + i))
			return *((char *) s1 + i) - *((char *) s2 + i);
	return 0;
}

int strcmp(const char *str1, const char *str2)
{
	while (*str1 == *str2 && *str1 != 0 && *str2 != 0) {
		str1++;
		str2++;
	}
	if (*str1 == *str2)
		return 0;
	if (*str1 > *str2)
		return 1;
	if (*str1 < *str2)
		return -1;
	return -1;
}

void reverse(unsigned char s[])
{
	int c, i, j;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

   /* itoa:  convert n to characters in s */
void itoa(int n, unsigned char s[])
{
	int i, sign;

	if ((sign = n) < 0)	/* record sign */
		n = -n;		/* make n positive */
	i = 0;
	do {			/* generate digits in reverse order */
		s[i++] = n % 10 + '0';	/* get next digit */
	} while ((n /= 10) > 0);	/* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

unsigned char *strstr(unsigned char *orig, unsigned char *pattern)
{
	int hlen;
	int nlen;

	hlen = strlen(orig);
	nlen = strlen(pattern);
	while (hlen >= nlen) {
		if (!memcmp(orig, pattern, nlen))
			return orig;

		orig++;
		hlen--;
	}
	return 0;
}
