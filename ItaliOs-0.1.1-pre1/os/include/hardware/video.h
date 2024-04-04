/*
ItalianOs

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

#define         NERO                    0
#define 	BLU			1
#define  	VERDE 			2
#define		CELESTE			3
#define		ROSSO			4
#define		VIOLA			5
#define		MARRONE 		6
#define		BIANCO_SPORCO		7
#define		GRIGIO			8
#define		BLU_CHIARO		9
#define		VERDE_CHIARO    	10
#define		CELESTINO	        11
#define		ARANCIONE       	12
#define		ROSA			13
#define		GIALLO			14
#define		BIANCO			15
#define     	MAX_COLOR		15
#define         LAMPEGGIO               128


void set_text_color( unsigned char color);
void set_back_color(unsigned char color);
unsigned char get_text_color(void);
unsigned char get_back_color(void);
void cls(void);
void set_y(unsigned int y);
void set_x(unsigned int x);


#endif

