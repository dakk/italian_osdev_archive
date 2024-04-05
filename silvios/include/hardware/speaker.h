/*ItalianOs
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


//note
#define LA5  880
#define SOL5 784
#define FA5  698
#define MI5  659
#define RE5  587
#define DO5  523
#define SI4  494
#define LA4  440
#define SOL4 392
#define FA4  349
#define MI4  329
#define RE4  293
#define DO4  261
#define SI3  247
#define LA3  220
#define SOL3 196
#define FA3  175
#define MI3  165
#define RE3  147
#define DO3  131
//#define DIESIS 50
//#define BEMOLLE (-50)
//#define PAUSA 1

//tempi
#define SEMIBREVE 	1000000		//4/4
#define MINIMA 		SEMIBREVE/2	//2/4  500000
#define SEMIMINIMA 	MINIMA/2	//1/4  250000
#define CROMA 		SEMIMINIMA/2	//1/8  125000
#define SEMICROMA	CROMA/2 	//1/16 062500
#define BISCROMA	SEMICROMA/2	//1/32 031250

#define PUNTO(val) (val/2)

//quando si vuole suonare una pausa nella frequenza mettere 0
// il tempo è espresso in bpm (sul 1/4) *250000

struct tono{
	unsigned short int frequenza;
	unsigned int durata;
};

struct note{
	struct tono nota[1000];
	unsigned short int tempo;
	unsigned short int num_note;
}note ;

void sound (unsigned short int freq);
void nosound ();
void beep ();
void suona(unsigned short int nota, unsigned short int tempo, unsigned int durata);
void musica(struct note *canzone);
void suona2(unsigned short int nota,   unsigned int durata);


#endif /* SPEAKER_H */


