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

#include <serial.h>
#include <mouse.h>
#include <stdio.h>
#include <delay.h>
#include <keyboard.h>
#include <task.h>

/*formato pakketto microsoft mouse
  1 byte:    1  LB  DB  Y7  Y6  X7  X6
  2 byte:   0 X5  X4  X3  X2  X1  X0
  2 byte:   0 Y5  Y4  Y3  Y2  Y1  Y0
  
  LB = tasto sinitro
  DB = tasto destro
  
  Se tutti e tre i byte sono a zero allora è stato premuyto il tasto centrale.
  Dopo che è stato premuto un tasto viene restituito 0x40 0x0 0x0  
  */

  //commentare queste 2 righe per togliere i messaggi di debug
#undef debug
#define debug(str, ...)

#define BUTTON_MASK 0x30

#define SINCRONIZE 0x40		//bit di sincronizzazione del pakketto

#define X_HIGH  0x3
#define Y_HIGH 0xC
#define MIN_REAL_X -2048
#define MAX_REAL_X 2048

#define MIN_REAL_Y -2048
#define MAX_REAL_Y 2048

//stuttura che contiene le coordinate del mouse assolute
struct internal_mouse {
	short int dx;
	short int sx;
	unsigned char button;
};

struct internal_mouse imouse;
SERIALE *ser;

void mouse_task(void)
{
	unsigned char i;
	int byte[3];		//pakketto grosso 24byte
	unsigned char button;
	char dx, sx;
	while (1) {
		for (i = 0; i < 3; i++) {
			//aspetto che arrivano i dati
			while ((byte[i] =
				uart_getchar_interrupt(ser)) == NO_DATA);

			//se è necessario sincronizzo il pakketto
			if (byte[i] & SINCRONIZE) {
				byte[0] = byte[i];
				i = 0;
			}
		}
		button = byte[0] & BUTTON_MASK;
		if (button) {	//se è stato premuto qualche tasto lo imposto
			imouse.button = button;
		}
		dx = ((byte[0] & X_HIGH) << 6) + byte[1];
		if (dx) {
			imouse.dx += dx;
			if (imouse.dx < MIN_REAL_X)
				imouse.dx = MIN_REAL_X;
			else if (imouse.dx > MAX_REAL_X)
				imouse.dx = MAX_REAL_X;
		}
		sx = ((byte[0] & Y_HIGH) << 4) + byte[2];
		if (sx) {
			imouse.sx += sx;
			if (imouse.sx < MIN_REAL_Y)
				imouse.sx = MIN_REAL_Y;
			else if (imouse.sx > MAX_REAL_Y)
				imouse.sx = MAX_REAL_Y;
		}
		debug("\n\n\nbottoni impostati a %x", imouse.button);
		debug("\ndx vale %d", imouse.dx);
		debug("\nsx vale %d", imouse.sx);
	}

}

void mouse_init(unsigned char port)
{
	unsigned int id;

	if ((ser = uart_open(port, INTR, 1200, NO_PARITY, BITS_7_PER_WORD,
			     STOP_BIT_1, DEFAULT_IRQ)) == NO_DEVICE) {
		printf("\nERRORE: impossibile aprire COM%d", port);
		return;
	}

	while ((id = uart_getchar_interrupt(ser)) == NO_DATA);

	imouse.dx = imouse.sx = imouse.button = 0;

	add_task(mouse_task, "DRIVER MOUSE");

	printf("\nMouse con Id 0x%x connesso alla porta COM%d e' stato inizializzato", id, port);
	return;
}

struct mouse get_mouse_pos(void)
{
	struct mouse topo;
	if (imouse.dx > MAX_MOUSE_X) {	//se la posizione reale è maggiore di 127
		imouse.dx -= MAX_MOUSE_X;
		topo.dx = MAX_MOUSE_X;
	} else if (imouse.dx < MIN_MOUSE_X) {
		imouse.dx -= MIN_MOUSE_X;
		topo.dx = MIN_MOUSE_X;
	} else {
		topo.dx = imouse.dx;
		imouse.dx = 0;
	}

	if (imouse.sx > MAX_MOUSE_Y) {	//se la posizione reale è maggiore di 127
		imouse.sx -= MAX_MOUSE_Y;
		topo.sx = MAX_MOUSE_Y;
	} else if (imouse.sx < MIN_MOUSE_Y) {
		imouse.sx -= MIN_MOUSE_Y;
		topo.sx = MIN_MOUSE_Y;
	} else {
		topo.sx = imouse.sx;
		imouse.sx = 0;
	}

	topo.button = imouse.button;
	imouse.button = 0;
	return topo;
}
