/*
SilviOs

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
#include <video.h>
#include <time.h>
#include <io.h>
#include <stdio.h>

unsigned char read_cmos(unsigned char reg)
{
	unsigned char high_digit, low_digit;

	outportb(0x70, reg);
	high_digit = low_digit = inportb(0x71);
/* convert from BCD to binary */
	high_digit >>= 4;
	high_digit &= 0x0F;
	low_digit &= 0x0F;
	return 10 * high_digit + low_digit;
}

/*****************************************************************************
NOTE: this function works only with local time, stored in the CMOS clock.
It knows nothing of GMT or timezones. This is a feature, not a bug :)
*****************************************************************************/
//unsigned long sys_time(void)
void sys_time(int i)	// i==0 for date , i==1 for time
{
	static char init;
	short date, month, hour, minute, second;
	short year;

	//printstring("\none more",0x7);	delay(5);

	if(!init)
	{
/* b2=0 BCD mode, vs. binary (binary mode seems to be buggy)
b1=1	24-hour mode, vs. 12-hour mode */
		outportb(0x70, 11);
		outportb(0x71, (inportb(0x71) & ~6) | 2);
		init = 1;
	}
/* wait for stored time value to stop changing */
	outportb(0x70, 10);
	while(inportb(0x71) & 128)
		/* nothing */;
/* get year/month/date
	year = read_cmos(9) + 1900;	xxx - OH NO, Y2K!
	year = read_cmos(9) + 2000;
use the Microsoft method -- this should be good from 1970-2069 */
	year = read_cmos(9);	/* 0-99 */
	if(year < 70)
		year += 2000;
	else
		year += 1900;
	month = read_cmos(8);	/* 1-12 */
	date = read_cmos(7);	/* 1-31 */
/* get time */
	hour = read_cmos(4);	/* 0-23 */
	minute = read_cmos(2);	/* 0-59 */
	second = read_cmos(0);	/* 0-59 */


	if (i==0)
	{
	  kprint("\n Date :=  ");
	  printlong(date);
	  kprint("/");
 	  printlong(month);
	  kprint("/");
 	  printlong(year);
	  //printstring("\n end Date :=  ",0x7);
        }
	else
	{
  	  kprint("\n Time :=  ");
	  printlong(hour);
	  kprint(":");
 	  printlong(minute);
	  kprint(":");
 	  printlong(second);
	}

		//return date_time_to_time_t(year, month, date, hour, minute, second);
}


