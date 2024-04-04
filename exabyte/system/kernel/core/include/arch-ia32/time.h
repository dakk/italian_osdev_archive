/* time.h */
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

#ifndef IA32_TIME_H
#define IA32_TIME_H

typedef struct ia32_time_t {
	unsigned char seconds;
	unsigned char minutes;
	unsigned char hours;
	unsigned char day;
	unsigned char month;
	unsigned short year;
} ia32_time_t;

void ia32_time_init();
void ia32_time_read(ia32_time_t *time);

#endif	/* IA32_TIME_H */
