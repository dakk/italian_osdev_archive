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

#include <video.h>
#include <io.h>

unsigned char *VIDEO_MEMORY = (char *)0xB8000;
unsigned char colore = 0x07;

unsigned char text_color = 0x07;
unsigned char back_color = 0;

void set_text_color( unsigned char color)
{
	text_color = color;
	return;
}

void set_back_color(unsigned char color)
{
	back_color =  (color * MAX_COLOR) + color;
}


void cls(void)				/* Clear the screen		*/
{
   unsigned int i;

   for(i = 0; i < (80*25); i++)         /* Fill the screen with         */
   {					/* background color		*/
      VIDEO_MEMORY[i*2] = 0x20;
      VIDEO_MEMORY[i*2+1] = back_color;
   }

   outportb(0x3d4, 0x0f);		/* Set the cursor to the	*/
   outportb(0x3d5, 0);			/* upper-left corner of the	*/
   outportw(0x3d4, 0x0e);		/* screen			*/
   outportb(0x3d5, 0);
}

