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

#ifndef _STDIO_H_
#define _STDIO_H_


extern unsigned char keyboard_buffer[255];
extern unsigned int peppe;
extern int *buffer_size;


#define printf kprint
#define debug kprint


unsigned char get_tot_color(void);
unsigned char getch();
void kprint(char *format, ...);
void getstring(char *string);
int get_number();
void scroll_video_up(void);
void  scroll_video_down(void);
void print_float(float num);
void putchar(unsigned char car);

#include <filesystem.h>

long ftell(FILE * file);
int  fgetc(FILE * file);
int  fputc(unsigned char c, FILE * file);
int fputs(unsigned char *string, FILE * stream);
//non sono implementat stdin, stdout e stderr
void fflush(FILE *myfile);

#define SEEK_SET 1    /*inizio del file*/
#define SEEK_END 2 /*fine del file*/
#define SEEK_CUR 3 /*posizione corrente*/

int fseek(FILE *stream, long offset, int dove);

#include <mytypes.h>

BOOL get_key_control();
void put_key_control();
unsigned char nbgetch();


#endif
