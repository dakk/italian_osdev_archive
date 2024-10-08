/*
 * Dreamos
 * bitops.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

 /*
  * Autore Ivan Gualandri
  * Prima versione: 27/10/2003
  * Thanks to: Silvio Abruzzo ^_^ che come sempre senza di lui nn potevo fare nemmeno questa :)
  */

#include <mydef.h>
#include <video.h>
#include <bitops.h>

int find_first_bit(unsigned short int irq_mask)
{
   int i=0;
   for (i=0; i<8;i++) if ((1<<i)&irq_mask) break;
       return i;
}

void print_bin(unsigned short int number)
{
   int i=0;
   byte mask=1;
   mask<<=7;
   for(i=1; i<=8; i++)
   {
       (number & mask) ? _kputc('1') : _kputc('0');
       number <<=1;
   }
}
