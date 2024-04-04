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

#include <rtc.h>
#include <io.h>
#include <idt.h>
#include <8259.h>
#include <gdt.h>
#include <mem.h>
#include <video.h>

static DATE startup_date;
static TIME startup_time;

volatile unsigned char read_rtc_address(unsigned char address);
volatile void write_rtc_address(unsigned char address, unsigned int val);

volatile unsigned char read_rtc_address(unsigned char address)
{
	outportb(0x70, address);
	return inportb(0x71);
}

volatile void write_rtc_address(unsigned char address, unsigned int val)
{
	outportb(0x70, address);
	outportb(0x71, val);
}

void get_date(DATE * date)
{
	asm("cli");
	date->fields.dow = bcd2dec(read_rtc_address(6));
	date->fields.dom = bcd2dec(read_rtc_address(7));
	date->fields.month = bcd2dec(read_rtc_address(8));
	date->fields.year = bcd2dec(read_rtc_address(9));
	asm("sti");
}

void get_time(TIME * time)
{
	asm("cli");
	time->fields.hour = bcd2dec(read_rtc_address(4));
	time->fields.minute = bcd2dec(read_rtc_address(2));
	time->fields.second = bcd2dec(read_rtc_address(0));
	asm("sti");
}

void init_rtc()
{
	get_date(&startup_date);
	get_time(&startup_time);
	asm("cli");
	write_rtc_address(0xa, 38);
	write_rtc_address(0xb, 66);
	read_rtc_address(0xc);
	asm("sti");
}


DATE get_sys_date(void)
{
	DATE mydata;
	get_date(&mydata);
	return mydata;
}

TIME get_sys_time(void)
{
	TIME mytime;
	get_time(&mytime);
	return mytime;
}

unsigned int time()
{
	return get_sys_time().time;
}