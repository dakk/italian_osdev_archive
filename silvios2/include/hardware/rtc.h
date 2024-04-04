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

#ifndef _RTC_H_
#define _RTC_H_

#define bcd2dec(bcd) ((((bcd) >> 4) * 0xa) + ((bcd) & 0xf))

typedef union {
  unsigned long time;
  struct {
    unsigned long hour:5;
    unsigned long minute:6;
    unsigned long second:6;
  }fields;
}TIME;

typedef union {
  unsigned long date;
  struct {
    unsigned long dow:3; /*Day of week*/
    unsigned long dom:5; /*Day of month*/
    unsigned long month:4;
    unsigned long year:7;
  }fields;
}DATE;

void init_rtc();
void get_date(DATE *date);
void get_time(TIME *time);
void set_alarm(TIME *time);
DATE get_sys_date(void);
TIME get_sys_time(void);

#ifdef _ITALIOS_
	unsigned int time();
#endif

#endif
