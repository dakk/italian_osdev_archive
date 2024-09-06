 /*
 * Dreamos
 * clock.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

 /*
  * Autore Ivan Gualandri
  * Prima versione: 29/04/2004
  */

#ifndef __CLOCK_H
#define __CLOCK_H

#define SEC_RTC 0x00
#define MIN_RTC 0x02
#define HOUR_RTC 0x04
#define DAY_W_RTC 0x06
#define DAY_MONTH 0x07
#define MONTH 0x08
#define YEAR 0x09

void readrtc(void);
char* get_day_week(void);
char* get_month(void);
int get_hour(void);
int get_minute(void);
int get_year(void);
int get_seconds(void);
int get_daymonth(void);
#endif
