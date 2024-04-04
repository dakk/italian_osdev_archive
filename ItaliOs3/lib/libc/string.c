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
