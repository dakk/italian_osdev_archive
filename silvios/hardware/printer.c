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

#include <parallel.h>
#include <printer.h>
#include <stdio.h>
#include <delay.h>

int reset_lp(unsigned int number)
{
	unsigned char command;
	command = LP_PSELECP | LP_PINITP;
	put_control(0, number);
	delay(1000);
	put_control(command, number);
	return get_status(number);
}


void print(unsigned char lpchar, unsigned int number)
{
/*    unsigned char status; */
	while (!(get_status(number)) & LP_PBUSY);
	put_data(lpchar, number);
	delay(10);
	put_control(LP_PSELECP | LP_PINITP | LP_PSTROBE, number);
	delay(10);
	put_control(LP_PSELECP | LP_PINITP, number);
	return;
}
