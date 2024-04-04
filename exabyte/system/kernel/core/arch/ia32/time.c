/* time.c */
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
#include <arch-ia32/cmos.h>
#include <arch-ia32/time.h>
#include <kernel.h>

#define BCD2BIN(n)			((n>>4)*10+(n&0x0F))

ia32_time_t boot_time;

void ia32_time_init() {
	ia32_time_read(&boot_time);
	kprint("cmos: now is %02u:%02u:%02u %02u/%02u/%02u\n",boot_time.hours,boot_time.minutes,boot_time.seconds,boot_time.day,boot_time.month,boot_time.year);
}

void ia32_time_read(ia32_time_t *time) {
	time->seconds=BCD2BIN(ia32_cmos_read(CMOS_RTC_SECONDS));
	time->minutes=BCD2BIN(ia32_cmos_read(CMOS_RTC_MINUTES));
	time->hours=BCD2BIN(ia32_cmos_read(CMOS_RTC_HOURS));
	time->day=BCD2BIN(ia32_cmos_read(CMOS_RTC_DAYOFMONTH));
	time->month=BCD2BIN(ia32_cmos_read(CMOS_RTC_MONTH));
	time->year=BCD2BIN(ia32_cmos_read(CMOS_RTC_CENTURY))*100+BCD2BIN(ia32_cmos_read(CMOS_RTC_YEAR));
}
