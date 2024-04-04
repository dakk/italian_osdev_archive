/* AleOS input/output source            *
* Copyright (c) 2005 Alessandro Presta */

#include <io.h>
#include <math.h>

char *videomem = (char *) 0xb8000;
char textattr = WHITE_TXT;
int row = 0;
int column = 0;



/* ... */



void write_chr(char chr)
{
	if (chr == '\n') {
		row++;
		column = 0;
		update_cursor();
	}
	else {
		videomem[(row * 80 + column) * 2] = chr;
		videomem[(row * 80 + column) * 2 + 1] = textattr;
		if (column < 80)
			column++;
		else {
			column = 0;
			row++;
		}
		update_cursor();
	}
}

void write_str(char *str)
{
	char *chr = str;
	while (*chr != '\0') {
		write_chr(*chr);
		chr++;
	}
}



/* ... */



void clear_screen()
{
	unsigned int i = 0;
	while (i < (80 * 25 * 2)) {
		videomem[i] = ' ';
		i++;
		videomem[i] = textattr;
		i++;
	}
}

void write_newline()
{
	write_chr('\n');
}

void text_attr(char attr)
{
	textattr = attr;
}

inline void outportb(unsigned int port, unsigned char byte)
{
	__asm__ __volatile__ ("outb %%al,%%dx"::"d" (port), "a" (byte));
}

void update_cursor()
{
	unsigned short position = (row * 80) + column;
	outportb(0x3d4, 0x0f);
	outportb(0x3d5, (unsigned char)(position & 0xff));
	outportb(0x3d4, 0x0e);
	outportb(0x3d5, (unsigned char)((position >> 8) & 0xff));
}
