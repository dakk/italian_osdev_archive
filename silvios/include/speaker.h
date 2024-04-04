/*SilviOs
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef SPEAKER_H
#define SPEAKER_H

/*
//note
#define REM 400
#define DOM 500
#define SI 550
#define LA 650
#define SOL 750
#define FA 850
#define MI  900
#define RE 1000
#define DO 1100
#define DIESIS (-50)
#define BEMOLLE 50
#define PAUSA 1

//tempi
#define INTERO 8000	//4/4
#define META INTERO/2	//2/4
#define QUARTO META/2	//1/4
#define CROMA   QUARTO/2	//1/8
#define SEMICROMA CROMA/2 //1/16
#define BISCROMA BISCROMA/2	//1/32
#define PUNTO SEMICROMA/2

*/

//quando si vuole suonare una pausa nella frequenza mettere 0
// il tempo è espresso in millisecondi

/*struct tono{
	unsigned short int frequenza;
	unsigned short int durata;
};

struct note{
	struct tono nota[1000];
	unsigned short int num_note;
}note ;*/

void sound (unsigned short int freq);
void nosound ();
void beep ();
void suona(unsigned short int freq, unsigned short int tempo);
/*void musica(struct note *canzone);*/


#endif /* SPEAKER_H */


