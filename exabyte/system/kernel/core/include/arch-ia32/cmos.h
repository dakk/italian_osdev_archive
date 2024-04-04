/* cmos.h */
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

#ifndef IA32_CMOS_H
#define IA32_CMOS_H

#define CMOS_RTC_SECONDS				0x00
#define CMOS_RTC_SECOND_ALARM			0x01
#define CMOS_RTC_MINUTES				0x02
#define CMOS_RTC_MINUTE_ALARM			0x03
#define CMOS_RTC_HOURS					0x04
#define CMOS_RTC_HOUR_ALARM				0x05
#define CMOS_RTC_DAYOFWEEK				0x06
#define CMOS_RTC_DAYOFMONTH				0x07
#define CMOS_RTC_MONTH					0x08
#define CMOS_RTC_YEAR					0x09
#define CMOS_RTC_STATUS_A				0x0A
#define CMOS_RTC_STATUS_B				0x0B
#define CMOS_RTC_STATUS_C				0x0C
#define CMOS_RTC_STATUS_D				0x0D
#define CMOS_RTC_CENTURY				0x32

void ia32_cmos_init();
inline unsigned char ia32_cmos_read(unsigned char address);

#endif	/* IA32_CMOS_H */
