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

#ifndef INCLUDE_VIDEO
#define INCLUDE_VIDEO

#include <cstddef>

enum  {NERO,
	    BLU,
	    VERDE,
	    CELESTE,
	    ROSSO,
	    VIOLA,
	    MARRONE,
	    BIANCO_SPORCO,
	    GRIGIO,
	    BLU_CHIARO,
	    VERDE_CHIARO,
	    CELESTINO,
	    ARANCIONE,
	    ROSA,
	    GIALLO,
	    BIANCO,
	    MAX_COLOR = 15,
	    LAMPEGGIO = 128
};

typedef unsigned char colore;


#define  endl  '\n'

class Video
{
public:
        Video();
	void init();
        void clear() ;
        void put(const char c) ;
	void put(const char* c) ;
	void set_text_color(  colore  mycolor);
	void set_back_color( colore mycolor);
	colore get_text_color(void)  ;
	colore get_back_color(void) ;

	Video& operator << (const int numero);
	Video& operator << (const unsigned int numero);
	Video& operator << (const char* str);
	Video& operator << (const char ch);
	Video& Video::operator << (const long double num);
/*	void set_x(byte x);
	void set_y(byte y);*/
	void Video::gotoxy(dword offset/*byte x, byte y*/);
	size_t get_pos();
private:
        unsigned short *videomem ;   //puntatore alla memoria
        size_t off ;                    //offset usato per le coordinate della y
        size_t pos ;                  //indica la posizione della x
	void accapo();
	//restituisce il colore nella forma color<<2
	unsigned short int get_formed_color();
	colore color;
	static const size_t tab_size = 8;
	//questi 2 volari non posso essere cambiati per problemi interni alle funzioni
	//li metto come costati per dare un significato ai numeri
	static const size_t max_x = 80;
	static const size_t max_y = 25;
	static const size_t screen_size  = max_x * max_y;

	void scroll();
};

extern Video& kout;

#endif
