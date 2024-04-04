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

#include <register.h>

unsigned int get_ds(void)
{
	unsigned int a;
      __asm__("xorl %%eax,%%eax\n" "movw %%ds,%%ax\n": "=a"(a):);
	return a;
}


unsigned int get_ss(void)
{
	unsigned int a;
      __asm__("xorl %%eax,%%eax\n" "movw %%ss,%%ax\n": "=a"(a):);
	return a;
}


unsigned int get_cs(void)
{
	unsigned int a;
      __asm__("xorl %%eax,%%eax\n" "movw %%cs,%%ax\n": "=a"(a):);
	return a;
}
