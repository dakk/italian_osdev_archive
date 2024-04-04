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

char *strcpy(char *dst, char *src)
{
	char *retval = dst;
	while (*src != 0)
		*dst++ = *src++;
	*dst = 0;
	return (retval);
}

char *strncpy(char *dst, const char *src, int n)
{
	char *retval = dst;
	while (*src != 0 && n-- > 0)
		*dst++ = *src++;
	*dst = 0;
	return (retval);
}

int strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2) {
		if (*s1 == 0)
			return 0;
		s1++;
		s2++;
	}
	return *(unsigned const char *) s1 - *(unsigned const char *) (s2);
}

int strncmp(const char *s1, const char *s2, int n)
{
	if (n == 0)
		return 0;
	do {
		if (*s1 != *s2++)
			return *(unsigned const char *) s1 -
				*(unsigned const char *) --s2;
		if (*s1++ == 0)
			break;
	} while (--n != 0);
	return 0;
}

char *strupr(char *s)
{
	char *base = s;
	while (*s != 0) {
		if (*s >= 'a' && *s <= 'z')
			*s = *s + 'A' - 'a';
		s++;
	}
	return (base);
}

char *strlwr(char *s)
{
	char *base = s;
	while (*s != 0) {
		if (*s >= 'A' && *s <= 'Z')
			*s = *s + 'a' - 'A';
		s++;
	}
	return (base);
}

int strlen(const char *s)
{
	register int result = 0;
	while (*s != 0)
		s++, result++;
	return (result);
}

char *strcat(char *dst, char *src)
{
	char *base = dst;
	while (*dst != 0)
		dst++;
	while (*src != 0)
		*dst++ = *src++;
	*dst = 0;
	return (base);
}

char *strscn(char *s, char *pattern)
{
	char *scan;
	while (*s != 0) {
		scan = pattern;
		while (*scan != 0) {
			if (*s == *scan)
				return (s);
			else
				scan++;
		}
		s++;
	}
	return (NULL);
}

char *strchr(char *s, int c)
{
	while (*s != 0) {
		if (*s == (char) (c))
			return (s);
		else
			s++;
	}
	return (NULL);
}


/*
 * Concatenate src on the end of dst.  At most strlen(dst)+n+1 bytes
 * are written at dst (at most n+1 bytes being appended).  Return dst.
 */
char *strncat(char *dest, const char *src, int n)
{
	if (n != 0) {
		char *d = dest;
		const char *s = src;

		while (*d != 0)
			d++;
		do {
			if ((*d = *s++) == 0)
				break;
			d++;
		} while (--n != 0);
		*d = 0;
	}
	return (dest);
}

char *strrchr(const char *s, int c)
{
	char *save;

	for (save = NULL; *s != NULL; s++)
		if (*s == c)
			save = (char *) s;

	return save;
}

char *strstr(const char *haystack, const char *needle)
{
	int hlen;
	int nlen;

	hlen = strlen((char *) haystack);
	nlen = strlen((char *) needle);
	while (hlen >= nlen) {
		if (!memcmp(haystack, needle, nlen))
			return (char *) haystack;

		haystack++;
		hlen--;
	}
	return 0;
}



int memcmp(const void *s1, const void *s2, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		if (*((char *) s1 + i) != *((char *) s2 + i))
			return *((char *) s1 + i) - *((char *) s2 + i);
	return 0;
}



void *memset(void *p, int c, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		*((unsigned char *) p + i) = c;
	return p;
}


void *memcpy(void *dest, const void *src, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		*((char *) dest + i) = *((char *) src + i);
	return dest;
}

void reverse(char s[])
{
	int c, i, j;

	for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

   /* itoa:  convert n to characters in s */
void itoa(int n, char s[])
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

char *utoa(unsigned value, char *digits, int base)
{
	char *s, *p;

	s = "0123456789abcdefghijklmnopqrstuvwxyz";
	if (base == 0)
		base = 10;
	if (digits == NULL || base < 2 || base > 36)
		return NULL;
	if (value < (unsigned) base) {
		digits[0] = s[value];
		digits[1] = '\0';
	} else {
		for (p = utoa(value / ((unsigned) base), digits, base);
		     *p; p++);
		utoa(value % ((unsigned) base), p, base);
	}
	return digits;
}
