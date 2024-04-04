#include <stdio.h>

int sprintf(char *buf, char *fmt, ...){
	va_list parms;
	int result;
	va_start(parms, fmt);
	result = vsprintf(buf, fmt, parms);
	va_end(parms);
	return (result);
}
