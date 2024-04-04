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
#include <stdio.h>
#include <mytypes.h>
#include <io.h>

unsigned short int base[3];

void detect_lpt_base_address()
{
	unsigned short int *ptaddress;
	unsigned short int address;
	int a;
	ptaddress = (unsigned short int *) 0x408;
	for (a = 0; a < 3; a++) {
		address = *ptaddress;
		if (!address) {
			printf("\nLPT%d non presente", a + 1);
			base[a] = 0;
		} else {
			printf("\nIndirizzo della porta LPT%d =  %xh", a + 1,
			       address);
			base[a] = address;
		}
		ptaddress++;
	}
	return;
}



/*num_port può assumere valori 1 , 2 , 3 e indica il numero della porta*/
int get_data(unsigned int num_port)
{
	if (num_port > 3)
		return -1;
	if (base[num_port - 1] == 0)
		return -1;
	return inportb(base[num_port - 1] + DATA);
}

/*num_port può assumere valori 1 , 2 , 3 e indica il numero della porta*/
BOOL put_data(unsigned char data, unsigned int num_port)
{
	if (num_port > 3)
		return FALSE;
	if (base[num_port - 1] == 0)
		return FALSE;
	//getch();
	outportb(base[num_port - 1] + DATA, data);
	return TRUE;
}

/*num_port può assumere valori 1 , 2 , 3 e indica il numero della porta*/
int get_status(unsigned int num_port)
{
	if (num_port > 3)
		return -1;
	if (base[num_port - 1] == 0)
		return -1;
	return inportb(base[num_port - 1] + STATUS);
}

/*num_port può assumere valori 1 , 2 , 3 e indica il numero della porta*/
BOOL put_control(unsigned char control, unsigned int num_port)
{
	if (num_port > 3)
		return FALSE;
	if (base[num_port - 1] == 0)
		return FALSE;
	outportb(base[num_port - 1] + CONTROL, control);
	return TRUE;
}
