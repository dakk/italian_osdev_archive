#include <stdio.h>

static char stdin_buffer[1024];

void scanf(char *string, ...)
{
	va_list pt;
	va_start(pt, string);
	gets(stdin_buffer);
	vsscanf(stdin_buffer, string, pt);
	va_end(pt);
}
