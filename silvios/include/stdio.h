/*
SilviOs

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
extern unsigned char *VIDEO_MEMORY;
extern unsigned char colore;

void printlong(unsigned long i);
void kprint(char *string);
unsigned char getch();
void getstring(char *string);
int get_number();

#endif
