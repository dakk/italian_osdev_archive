#include <stdlib.h>

#ifndef __STRING_H
#define __STRING_H

/**
 *   _kstrncmp()
 *
 *   Compares the first (at most) n characters of s1 and s2
 *   Return: 0(equals), 1(s1>s2), -1(s1<s2)
 */
int _kstrncmp(const char *, const char *, int);

/**
 *   Converte un numero in una stringa
 */
void _kntos(char *, int, int);

size_t strlen(const char *string);
void strcat(char *strDestination, const char *strSource);
void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *dest, const int c, size_t n);
#endif
