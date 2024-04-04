#include <stdio.h>
#include <syscall.h>


static char stdout_buffer[1024];

void printf(char *format, ...)
{
	va_list pt;
	va_start(pt, format);
	vsprintf(stdout_buffer, format, pt);
	puts(stdout_buffer);
	va_end(pt);
}

//stampa una stringha
void puts(char *str)
{
      asm("int $0x55": :"a"(SYS_PUTS), "b"(str));
}

void putchar(char car)
{
	//putc(car);
	asm("pusha");
      asm("int $0x55": :"a"(SYS_PUTC), "b"(car));
	asm("popa");
	return;
}

int getchar()
{
	int car;
      asm("int $0x55": "=a"(car):"a"(SYS_GETC));
	return car;
}

char *gets(char *string)
{
      asm("int $0x55": :"a"(SYS_GETS), "b"(string));
	return string;
}


static char stdin_buffer[1024];

void scanf(char *string, ...)
{
	va_list pt;
	va_start(pt, string);
	gets(stdin_buffer);
	vsscanf(stdin_buffer, string, pt);
	va_end(pt);
}
