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

#ifndef _MOUSE_H_
#define _MOUSE_H_

#define TASTO_SINISTRO  0x20
#define TASTO_DESTRO 0x10

#define MAX_MOUSE_X  127
#define MIN_MOUSE_X -128

#define MIN_MOUSE_Y -128
#define MAX_MOUSE_Y  127

//contiene le coordinate del mouse normalizzate
struct mouse{
	char dx;
	char sx;
	unsigned char button;
};

void mouse_init(unsigned char port);
struct mouse get_mouse_pos(void);

#endif
