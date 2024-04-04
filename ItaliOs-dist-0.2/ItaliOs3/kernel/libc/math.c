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
7
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <math.h>

double fmod(double x, double y)
{
	int n = x / y;
	return x - n * y;
}


unsigned long pow(unsigned long x, unsigned long power)
{
	unsigned long count, y = x;
	if (power == 1)
		return x;
	if (power == 0)
		return 1;
	for (count = 0; count <= (power - 2); count++)
		y = y * x;
	return y;
}
