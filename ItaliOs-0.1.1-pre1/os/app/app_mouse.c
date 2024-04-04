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

void mouse(void)
{
	unsigned char porta;
	struct mouse topo;
	printf("\ninserisci la porta in cui e' collegato il mouse: ");
	porta = get_number();
	mouse_init(porta);
	while (1) {
		topo = get_mouse_pos();
		printf("\nx = %d, y= %d, bottoni = %d", topo.dx, topo.sx,
		       topo.button);
	}
	return;
}
