/* cmos.c */
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
#include <arch-ia32/io.h>
#include <arch-ia32/cmos.h>
#include <kernel.h>

#define IODELAY					{inportb(0x80); inportb(0x80); inportb(0x80); inportb(0x80); } // a caso

#define CMOS_ADDRESS			0x70
#define CMOS_DATA				0x71

inline unsigned char ia32_cmos_read(unsigned char address) {
	outportb(CMOS_ADDRESS,address);
	IODELAY;
	return inportb(CMOS_DATA);
}

void ia32_cmos_init() {
	unsigned char temp;
	temp=ia32_cmos_read(CMOS_RTC_STATUS_D);
	if (temp&(1<<7))
		kprint("cmos: battery power good\n");
	else
		kprint("cmos: battery dead\n");
}
