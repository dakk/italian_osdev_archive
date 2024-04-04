 /*ItaliOs
 
 Copyright  (c) <2003> Abruzzo Silvio
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
7
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <video.h>
#include <string.h>


Video mystdout;
Video& kout = mystdout;

Video& Video::operator << (const int numero){
	char num_str[30];
	itoa(numero, num_str);
	put(num_str);
	return *this;
}


Video& Video::operator << (const unsigned int numero){
	char num_str[30];
	utoa(numero, num_str, 10);
	put(num_str);
	return *this;
}


Video& Video::operator << (const char* str){
	put(str);
	return *this;
}


Video& Video::operator << (const char ch){
	put(ch);
	return *this;
}


Video::Video(){
      pos=0 ;        off=0 ;
        videomem = (unsigned short*) 0xB8000 ;
	set_text_color(GIALLO);
	set_back_color(BLU);
	clear();
}

void Video::init(){
      pos=0 ;        off=0 ;
        videomem = (unsigned short*) 0xB8000 ;
	set_text_color( BIANCO);
	set_back_color(NERO);
	clear();
}

void Video::clear()
{
        unsigned int i;

        for(i=0; i<screen_size; i++)
        {
                videomem[i] = (unsigned char) ' ' | get_formed_color() ;
        }
        pos=0 ; off=0 ;
}

void Video::accapo(){
	while(pos<max_x){
		videomem[off + pos] = (unsigned char) ' ' | get_formed_color() ;
		pos++;
	}
	pos = 1;
	off += max_x;
}

void Video::put(const char* c){
	int i;
	for (i = 0; c[i] != '\0'; i++) {
		put(c[i]);
	}
	return;
}

void Video::put(const char c)
{

	if(c == '\n'){
		accapo();
		return;
	}

	if(c == '\t'){
		for(size_t i = 0; i < tab_size; i++)
			put(' ');
		return;
	}

        if(pos>=max_x){
        	accapo();
        }

        if(off>=screen_size)
        {
		for(size_t i=0; i<screen_size-80; i++)
			videomem[i] = videomem[i+80];
		for(size_t  i = screen_size-80; i < screen_size; i++)
			videomem[i] = videomem[off + pos] = (unsigned char) ' '  | get_formed_color() ;;

		pos = 0; off-=80;
        }

        videomem[off + pos] = (unsigned char) c | get_formed_color() ;
        pos++ ;
}

unsigned short int Video::get_formed_color(){
	return color<<8;
}

void Video::set_text_color( colore  mycolor){
	color = color & 0x70;
	color |= (mycolor & 0xF);
}


void Video::set_back_color(colore mycolor){
	//azzero i bit deillo sfondo
	color &=0x8f; //ottengo x000 xxxx

	//preparo i bit dello sfondo
	mycolor &= 0x7;  //cancello tutti i i bbit tranne i primi tre quindi alla fine ottengo 0000 0xxx
	mycolor <<=4;  //shifto tutto di 4 in questo modo li allineo per bene e ottengo 0xxx 0000

	//setto il colore per bene
	color |= mycolor;
}


colore Video::get_text_color(void){
	return color & 0xF;
}


colore Video::get_back_color(void){
	return (color & 0x7f) >>4;
}
