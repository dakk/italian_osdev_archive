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
#ifndef  _TIME_H_
#define  _TIME_H_

unsigned char read_cmos(unsigned char reg) ;
unsigned long date_time_to_time_t(unsigned short year, unsigned char month,unsigned char date, unsigned char hour, unsigned char min,unsigned char sec) ;
//unsigned long sys_time(void);
void sys_time(int i);

#endif
