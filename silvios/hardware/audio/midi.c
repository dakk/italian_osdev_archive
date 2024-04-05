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

#include <stdio.h>
#include <io.h>
#include <midi.h>
#include <dsp.h>
#include <const.h>
#include <mixer.h>

//normal midi mode
void write_midi_command(unsigned short int base, unsigned short int value)
{
	write_dsp(base, MIDI_WRITE);
	write_dsp(base, value);
}

//uart midi mode
unsigned short int reset_midi(unsigned short int base)
{
	unsigned short int i = 65535U;
	while ((inportb(base + STATUS_MIDI) & MIDI_WRITE != MIDI_WRITE)
	       && (i > 0)) {
		--i;
	}
	outportb(base + COMMAND_MIDI, MIDI_RESET);
	if (i == 0)
		return 0U;
	i = 65535U;

	while (((inportb(base + STATUS_MIDI) & DATA_AVAIBLE) == 0) && (i > 0)) {
		--i;
	}
	if (i == 0)
		return 0U;

	for (i = 65535; i > 0; --i) {
		if (inportb(base + DATA_MIDI) == MIDI_RESETTED) {
			break;
		}
	}
	if (i == 0)
		return 0U;


	return 1U;
}

unsigned short int check_midi_status(unsigned short int);

void send_uart_data_command(unsigned short int, unsigned short int);

unsigned short int enter_uart_mode(unsigned short int base)
{
	unsigned short int i = 65535U;
	while ((inportb(base + STATUS_MIDI) & MIDI_WRITE != MIDI_WRITE)
	       && (i > 0)) {
		--i;
	}
	outportb(base + COMMAND_MIDI, MIDI_UART_MODE);
	if (i == 0)
		return 0U;
	i = 65535U;
	while (((inportb(base + STATUS_MIDI) & DATA_AVAIBLE) != 0) && (i > 0)) {
		--i;
	}
	if (i == 0)
		return 0U;

	for (i = 65535; i > 0; --i) {
		if (inportb(base + DATA_MIDI) == MIDI_RESETTED) {
			break;
		}
	}
	if (i == 0)
		return 0U;


	return 1U;
}

unsigned short int read_uart_data(unsigned short int);
