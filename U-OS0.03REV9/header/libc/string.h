#include <stdlib.h>

#ifndef __STRING_H
#define __STRING_H

int _kstrncmp(const char *, const char *, int);

void _kntos(char *, int, int);

size_t strlen(const char *string);
void strcat(char *strDestination, const char *strSource);
#endif

