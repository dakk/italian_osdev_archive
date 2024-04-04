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

/*
0x8A00: Usato per abilitare il debug
0x8A01: Selects register 0: Indirizzo dell'inizio del monitoraggio della memoria (inclusivo)
0x8A02: Selects register 1: Indirizzo della fine del monitoraggio della memoria (esclusivo)
0x8A80: Abilita il monitoraggio della memoria come indicato dai registri 0 e 1 e cancella entrambi i registri
0x8AE0: Ritorna il Prompt di debug
0x8AE2: Disabilita il trace delle istruzioni
0x8AE3: Abilita il trace delle istruzioni
0x8AE4: Disabilita il trace dei registri
0x8AE5: Abilita il trace dei registri (funziona solo se il trace delle istruzioni e abilitato)
0x8AFF: Disabilita l'i/o debug e le funzioni di monitoraggio della memoria.
*/


#include <debug.h>
#include <cstring>
#include <io.h>

namespace {
	static const word BOCHS_IODEBUG_PORT = 0x8A00;
}

Debug debug;

void Debug::istruction_trace(word status)
{
	if (status == ON) {
		outportw(BOCHS_IODEBUG_PORT, 0x8AE3);
		return;
	}
	if (status == OFF) {
		outportw(BOCHS_IODEBUG_PORT, 0x8AE2);
		return;
	}
	return;
}


void Debug::register_trace(word status)
{
	if (status == ON) {
		istruction_trace(ON);
		outportw(BOCHS_IODEBUG_PORT, 0x8AE5);
		return;
	}
	if (status == OFF) {
		istruction_trace(OFF);
		outportw(BOCHS_IODEBUG_PORT, 0x8AE4);
		return;
	}
	return;
}

void Debug::stop()
{
	outportw(BOCHS_IODEBUG_PORT, 0x8AE0);
}

Debug & Debug::operator <<(const int numero)
{
	char num_str[30];
	itoa(numero, num_str);
	put(num_str);
	return *this;
}


Debug & Debug::operator <<(const unsigned int numero)
{
	char num_str[30];
	utoa(numero, num_str, 10);
	put(num_str);
	return *this;
}


Debug & Debug::operator <<(const char *str)
{
	put(str);
	return *this;
}


Debug & Debug::operator <<(const char ch)
{
	put(ch);
	return *this;
}


Debug::Debug()
{
	outportw(BOCHS_IODEBUG_PORT, 0x8A00);
	put("\n\nDebugger inizializzato\n\n");
}

void Debug::put(const char *c)
{
	int i;
	for (i = 0; c[i] != '\0'; i++) {
		put(c[i]);
	}
	return;
}

void Debug::put(const char c)
{
	outportb(0xe9, c);
}
