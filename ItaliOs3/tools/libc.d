#include "config_func.h"
#include <stdio.h>
#include <stddef.h>

#ifdef __IT_SPRINTF__
#undef  __IT_VSPRINTF__
#define __IT_VSPRINTF__
int sprintf(char *buf, char *fmt, ...){
	va_list parms;
	int result;
	va_start(parms, fmt);
	result = vsprintf(buf, fmt, parms);
	va_end(parms);
	return (result);
}
#endif

#ifdef __IT_PRINTF__
#undef  __IT_VSPRINTF__
#define __IT_VSPRINTF__
#undef  __IT_PUTS__
#define __IT_PUTS__
static char stdout_buffer[1024];

void printf(char *format, ...){
	va_list pt;
	va_start(pt, format);
	vsprintf(stdout_buffer, format, pt);
	puts(stdout_buffer);
	va_end(pt);
}
#endif

#ifdef __IT_STRCPY__
char *strcpy(char *dst, char *src)
{
	char *retval = dst;
	while (*src != 0)
		*dst++ = *src++;
	*dst = 0;
	return (retval);
}
#endif

#ifdef __IT_STRNCPY__
char *strncpy(char *dst, const char *src, int n)
{
	char *retval = dst;
	while (*src != 0 && n-- > 0)
		*dst++ = *src++;
	*dst = 0;
	return (retval);
}
#endif


#ifdef __IT_STRCMP__
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
#endif

#ifdef __IT_STRNCMP__
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
#endif

#ifdef __IT_STRUPR__
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
#endif


#ifdef __IT_STRLWR__
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
#endif

#ifdef __IT_STRCAT__
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
#endif

#ifdef __IT_STRSCN__
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
#endif

#ifdef __IT_STRCHR__
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
#endif

#ifdef __IT_STRNCAT__
/*
 * Concatenate src on the end of dst.  At most strlen(dst)+n+1 bytes
 * are written at dst (at most n+1 bytes being appended).  Return dst.
 */\
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
#endif

#ifdef __IT_STRCHR__
char *strrchr(const char *s, int c)
{
	char *save;

	for (save = NULL; *s != NULL; s++)
		if (*s == c)
			save = (char *) s;

	return save;
}
#endif

#ifdef __IT_STRSTR__
#undef  __IT_STRLEN__
#define __IT_STRLEN__
#undef   __IT_MEMCMP__
#define  __IT_MEMCMP__
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
#endif

#ifdef  __IT_MEMCMP__
int memcmp(const void *s1, const void *s2, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		if (*((char *) s1 + i) != *((char *) s2 + i))
			return *((char *) s1 + i) - *((char *) s2 + i);
	return 0;
}
#endif


#ifdef __IT_MEMSET__
void *memset(void *p, int c, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		*((unsigned char *) p + i) = c;
	return p;
}
#endif

#ifdef  __IT_MEMCPY__
void *memcpy(void *dest, const void *src, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		*((char *) dest + i) = *((char *) src + i);
	return dest;
}
#endif

#ifdef  __IT_ITOA__
#undef   __IT_STRLEN__
#define __IT_STRLEN__
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

#endif

#ifdef __IT_UTOA__
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
#endif

#ifdef __IT_STRLEN__
int strlen(const char *s)
{
	register int result = 0;
	while (*s != 0)
		s++, result++;
	return (result);
}
#endif

#ifdef __IT_PUTS__
void puts(char *str)
{
      asm("int $0x55": :"a"(6), "b"(str));
}
#endif

#ifdef __IT_PUTCHAR__
void putchar(char car){
	  asm("int $0x55": :"a"(5), "b"(car));
}
#endif

#ifdef __IT_VSPRINTF__
int vsprintf(char *buf,char *fmt,va_list parms){
			int result;
		  asm("int $0x55":"=a"(result) :"a"(100), "b"(buf), "c"(fmt), "d"(parms));
		return result;
}
#endif

#ifdef __IT_GET_SCR_POS__
dword get_scr_pos()
{
	dword pos;
      asm("int $0x55": "=a"(pos):"a"(11));
	return pos;
}
#endif


#ifdef __IT_GETENV__
#define __IT_GETENV__
#undef __IT_MALLOC__
#define __IT_MALLOC__
	char *getenv(const char *name){
		char* value = (char*) malloc(1000);
		asm("int $0x55": :"a"(18), "b"(name),"c"(value));
		return value;
	}
#endif

#ifdef __IT_MALLOC__
void *malloc(size_t dim){
	int pos;
      asm("int $0x55": "=a"(pos):"a"(1), "b"(dim));
	return (void*)pos;
}
#endif

#ifdef __IT_FREE__
void free(void *addr)
{
	asm("int $0x55"::"a"(2), "b"(addr));
}
#endif

#ifdef __IT_REALLOC__
//Feature oscura realloc(NULL, size) == malloc(size)
//                                 realloc(pointer, 0) == free(pointer)
void *realloc(void *pointer, size_t size)
{
	void *punt;
	asm("int $0x55":"=a"(punt):"a"(3), "b"(pointer), "c"(size));
	return punt;
}
#endif

#ifdef __IT_CALLOC__
void *calloc(size_t nmemb, size_t size)
{
	
	void *punt;
	asm("int $0x55":"=a"(punt):"a"(4), "b"(nmemb), "c"(size));
	return punt;
}
#endif


#ifdef __IT_SSCANF__
#undef __IT_VSSCANF__
#define __IT_VSSCANF__
int sscanf(char *buf, char *fmt, ...)
{
	va_list parms;
	int result;
	va_start(parms, fmt);
	result = vsscanf(buf, fmt, parms);
	va_end(parms);
	return (result);
}
#endif


#ifdef __IT_SCANF__
#undef __IT_GETS__
#define __IT_GETS__
#undef __IT_VSSCANF__
#define __IT_VSSCANF__
static char stdin_buffer[1024];

void scanf(char *string, ...)
{
	va_list pt;
	va_start(pt, string);
	gets(stdin_buffer);
	vsscanf(stdin_buffer, string, pt);
	va_end(pt);
}
#endif

#ifdef __IT_VSSCANF__
int vsscanf(char *buf,char *fmt,va_list parms){
			int result;
		  asm("int $0x55":"=a"(result) :"a"(101), "b"(buf), "c"(fmt), "d"(parms));
		return result;
}
#endif



#ifdef __IT_GETCHAR__
int getchar()
{
	int car;
      asm("int $0x55": "=a"(car):"a"(12));
	return car;
}
#endif

#ifdef __IT_GETS__
char *gets(char *string)
{
      asm("int $0x55": :"a"(13), "b"(string));
	return string;
}
#endif

#ifdef __IT_SETENV__
#define __IT_SETENV__
	int setenv(const char *name, const char *value, int overwrite){
		asm("int $0x55": :"a"(19), "b"(name),"c"(value));
		return 0;
	}
#endif
