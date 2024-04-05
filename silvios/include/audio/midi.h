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

#ifndef _MIDI_H

#define _MIDI_H

//normal midi mode
void write_midi_command(unsigned short int, unsigned short int);

//uart midi mode

unsigned short int reset_midi(unsigned short int);

unsigned short int check_midi_status(unsigned short int);

void send_uart_data_command(unsigned short int,unsigned short int);

unsigned short int enter_uart_mode(unsigned short int);

unsigned short int read_uart_data(unsigned short int);

#endif
