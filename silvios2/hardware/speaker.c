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

#include <speaker.h>
#include <delay.h>
#include <io.h>
#include <stdio.h>


void sound(unsigned short int freq)
{
	unsigned short int freqdiv;
	unsigned char tmp;
	freqdiv = 1193180 / freq;
	outportb_p(0x43, 0xB6);	/* counter 2 select, binary 16-bit counter,
				 * first bits 0-7 */
	outportb_p(0x42, (unsigned char) freqdiv);	/* first bits 0-7 */
	outportb_p(0x42, (unsigned char) (freqdiv >> 8));	/* then bits 8-15 */
	tmp = inportb_p(0x61);
	if (tmp != (tmp | 3))	/* only output if bits are not correctly set */
		outportb_p(0x61, tmp | 3);
}


void nosound()
{
	unsigned char tmp;
	tmp = inportb_p(0x61);	/* KB controller port B */
	tmp &= 0xFC;		/* disable speaker + clock 2 gate */
	outportb_p(0x61, tmp);	/* output */
}

void suona(unsigned short int nota, unsigned short int tempo,
	   unsigned int durata)
{
	if (nota != 0)
		sound(nota);
	delay(durata * 60 / (250 * tempo));
	nosound();
}

void suona2(unsigned short int nota, unsigned int durata)
{
	if (nota != 0)
		sound(nota);
	delay(durata);
	nosound();
}

void beep()
{
	sound(1000);
	delay(250);
	nosound();
}

void musica(struct note *canzone)
{
	int i;
	for (i = 1; i <= canzone->num_note; i++) {
		/*printf("\nnota = %dfrequenza = %d, tempo = %d, durata = %d", i, canzone->nota[i].frequenza, canzone->tempo, canzone->nota[i].durata); */
		suona(canzone->nota[i].frequenza, canzone->tempo,
		      canzone->nota[i].durata);
		delay(10);
	}
	return;
}
