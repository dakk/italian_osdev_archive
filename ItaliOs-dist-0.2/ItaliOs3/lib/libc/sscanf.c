#include <stdio.h>

int sscanf(char *buf, char *fmt, ...)
{
	va_list parms;
	int result;
	va_start(parms, fmt);
	result = vsscanf(buf, fmt, parms);
	va_end(parms);
	return (result);
}
