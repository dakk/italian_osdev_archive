#include <stdio.h>

static char stdout_buffer[1024];

void printf(char *format, ...){
	va_list pt;
	va_start(pt, format);
	vsprintf(stdout_buffer, format, pt);
	puts(stdout_buffer);
	va_end(pt);
}


dword get_scr_pos()
{
	dword pos;
      asm("int $0x55": "=a"(pos):"a"(11));
	return pos;
}
