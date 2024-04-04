 /*ItaliOs
  * 
  * Copyright  (c) <2003> Abruzzo Silvio
  * 
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version 2
  * of the License, or (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 7
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  */

#include <video.h>
#include <cstring>
#include <cstdio>
#include <debug.h>
#include <idt.h>
#include <io.h>
#include <vt100.h>

namespace {
	const char ESC = 27;
	const byte len_escape = 10;
	byte escape = 0;
	char vtseq[10];
	dword offs, poss;
}


Video mystdout;
Video & kout = mystdout;

Video & Video::operator <<(const int numero)
{
	char num_str[30];
	itoa(numero, num_str);
	put(num_str);
	return *this;
}


Video & Video::operator <<(const unsigned int numero)
{
	char num_str[30];
	utoa(numero, num_str, 10);
	put(num_str);
	return *this;
}


Video & Video::operator <<(const char *str)
{
	put(str);
	return *this;
}


Video& Video::operator << (const long double num){
	char num_str[30];
	sprintf(num_str, "%lf", num);
	put(num_str);
	return *this;
};

Video & Video::operator <<(const char ch)
{
	put(ch);
	return *this;
}


Video::Video()
{
	pos = 0;
	off = 0;
	videomem = (unsigned short *) 0xB8000;
	set_text_color(GIALLO);
	set_back_color(BLU);
	clear();
}

void Video::init()
{
	pos = 0;
	off = 0;
	videomem = (unsigned short *) 0xB8000;
	set_text_color(BIANCO);
	set_back_color(NERO);
	clear();
}
/*
void Video::set_x(byte x){
	outportb(0x3d4, 0x0f);
	outportb(0x3d5, x);
}

void Video::set_y(byte y){
	outportb(0x3d4, 0x0e);
	outportb(0x3d5, y);
}*/

void Video::gotoxy(dword offset /*byte x, byte y */ )
{
	outportb(0x3D4, 14);
	outportb(0x3D5, offset >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, offset);
}


void Video::clear()
{
	unsigned int i;

	for (i = 0; i < screen_size; i++) {
		videomem[i] = (unsigned char) ' ' | get_formed_color();
	}
	pos = 0;
	off = 0;
	gotoxy(off + pos);
}

void Video::accapo()
{
	while (pos < max_x) {
		videomem[off + pos] =
			(unsigned char) ' ' | get_formed_color();
		pos++;
	}
	pos = 0;
	off += max_x;
	off /= max_x;
	off *= max_x;
	scroll();
}

void Video::put(const char *c)
{
	int i;
	for (i = 0; c[i] != '\0'; i++) {
		put(c[i]);
	}
	return;
}

size_t Video::get_pos()
{
	return off + pos;
}

void Video::scroll()
{
	if (off >= screen_size) {
		for (size_t i = 0; i < screen_size - max_x; i++)
			videomem[i] = videomem[i + max_x];
		for (size_t i = screen_size - max_x; i < screen_size; i++)
			videomem[i] = videomem[off + pos] =
				(unsigned char) ' ' | get_formed_color();;

		pos = 0;
		off -= max_x;
	}
	gotoxy(off + pos);
}

void Video::put(const char c)
{
	asm("cli");
	debug << c /* << "      " << pos << "      " << off << endl */ ;

	if (c == '\n') {
		accapo();

		idt::sti();
		return;
	}

	if (c == '\t') {
		for (size_t i = 0; i < tab_size; i++)
			put(' ');
		idt::sti();
		return;
	}

	if (c == BACKSPACE) {
		if (pos) {
			pos--;
		} else {
			pos = max_x - 1;
			off -= max_x;;
		}
		//debug << "pos = " << pos  << " off = " << off << endl;
		videomem[off + pos] =
			(unsigned char) ' ' | get_formed_color();
		gotoxy(off + pos);
		idt::sti();
		return;
	}

	if (pos >= max_x) {
		accapo();
	}

	char car = c;
	//gestisco i casi particolari
	switch (car) {
	case 'è':
		car = 138;
		break;
	case 'ì':
		car = 141;
		break;
	case 'à':
		car = 133;
		break;
	case 'ò':
		car = 149;
		break;
	case 'ù':
		car = 141;
		break;
	};

	if (car == ESC) {	//wow Ã¨ un carattere di escape
		//debug << "wow Ã¨ un carattere di escape\n";
		escape = 1;
		for (byte i = 0; i < len_escape; i++)
			vtseq[i] = 0;
		idt::sti();
		return;
	}

	if (escape) {
		if (escape == 1) {
			//debug << "vado nella gestione del primo escape\n";
			switch (car) {
			case '[':
				idt::sti();
				return;
			case 'A':	//freccia in alto
				if (off > max_x) {
					off -= max_x;
					gotoxy(off + pos);
				}
				escape = 0;
				idt::sti();
				return;
			case 'B':	//freccia in basso
				if ((off + max_x) < (max_x * max_y * 2)) {
					off += max_x;
					gotoxy(off + pos);
				}
				escape = 0;
				idt::sti();
				return;
			case 'C':	//freccia a destra
				if ((off + pos + 1) < (max_x * max_y * 2)) {
					pos++;
					gotoxy(off + pos);
				}
				escape = 0;
				idt::sti();
				return;
			case 'D':
				if (off + pos - 1) {
					pos--;
					gotoxy(off + pos);
				}
				escape = 0;
				idt::sti();
				return;
			case 'K':	/*cancella tutta la righa e si posiziona all'inizio??????? */
				debug << "Devo cancellare tutta la righa\n";
				break;
			case 'S':	//salvo la posizione del cursore
				offs = off;
				poss = pos;
				escape = 0;
				idt::sti();
				return;
			case 'u':	//ripristino la posizione del cursore
				off = offs;
				pos = poss;
				escape = 0;
				idt::sti();
				return;
			default:
				//      debug << "non Ã¨ nessuno dei precedenti\n";
				vtseq[0] = car;
				escape = 2;
				idt::sti();
				return;
			};
		}
		if (escape == 2) {
			switch (car) {
			case 'J':
				if (vtseq[0] == '2') {	//cls
					clear();
					escape = 0;
					idt::sti();
					return;
				}
				escape = 0;
				idt::sti();
				return;
			case ';':
				escape = 3;
				idt::sti();
				return;
			default:
				escape = 0;
				idt::sti();
				return;
			}
		}
		if (escape == 3) {
			switch (car) {
			case 'A':	//devo portare il cursore su di n righe
				for (int i = 0; i < (vtseq[0] - '0'); i++) {
					if (off > max_x) {
						off -= max_x;
						gotoxy(off + pos);
					}
				}
				escape = 0;
				idt::sti();
				return;
			case 'B':	//freccia in basso
				for (int i = 0; i < (vtseq[0] - '0'); i++) {
					if ((off + max_x) <
					    (max_x * max_y * 2)) {
						off += max_x;
						gotoxy(off + pos);
					}
				}
				escape = 0;
				idt::sti();
				return;
			case 'C':	//freccia a destra
				for (int i = 0; i < (vtseq[0] - '0'); i++) {
					if ((off + pos + 1) <
					    (max_x * max_y * 2)) {
						pos++;
						gotoxy(off + pos);
					}
				}
				escape = 0;
				idt::sti();
				return;
			case 'D':
				for (int i = 0; i < (vtseq[0] - '0'); i++) {
					if (off + pos - 1) {
						pos--;
						gotoxy(off + pos);
					}
				}
				escape = 0;
				idt::sti();
				return;
			default:
				vtseq[1] = car;
				escape = 4;
				idt::sti();
				return;
			}
		}
/*		if(escape == 4){
			switch(car){
				case 'H':
				case 'f':
					
			}*/
		//      }
	}

	videomem[off + pos] = (unsigned char) car | get_formed_color();
	pos++;
	gotoxy(off + pos);


	idt::sti();
}

unsigned short int Video::get_formed_color()
{
	return color << 8;
}

void Video::set_text_color(colore mycolor)
{
	color = color & 0x70;
	color |= (mycolor & 0xF);
}


void Video::set_back_color(colore mycolor)
{
	//azzero i bit deillo sfondo
	color &= 0x8f;		//ottengo x000 xxxx

	//preparo i bit dello sfondo
	mycolor &= 0x7;		//cancello tutti i i bbit tranne i primi tre quindi alla fine ottengo 0000 0xxx
	mycolor <<= 4;		//shifto tutto di 4 in questo modo li allineo per bene e ottengo 0xxx 0000

	//setto il colore per bene
	color |= mycolor;
}


colore Video::get_text_color(void)
{
	return color & 0xF;
}


colore Video::get_back_color(void)
{
	return (color & 0x7f) >> 4;
}
