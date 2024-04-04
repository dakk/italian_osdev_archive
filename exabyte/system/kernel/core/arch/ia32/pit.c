/* pit.c */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#include <config.h>
#include <arch-ia32/arch.h>
#include <arch-ia32/pit.h>
#include <arch-ia32/io.h>
#include <kernel.h>

void ia32_pit_set_frequency(unsigned hz) {
#if 1
	unsigned short value=(3579545L/3)/hz;
#else
	unsigned short value=(1193180L+hz/2)/hz;
#endif
	outportb(0x43,0x36);
	outportb(0x40,value&0xFF);
	outportb(0x40,value>>8);
}

unsigned short ia32_pit_get_counter() {
	unsigned short ret;
	outportb(0x43,0x00);
	ret=inportb(0x40);
	ret+=inportb(0x40)<<8;
	return ret;
}
