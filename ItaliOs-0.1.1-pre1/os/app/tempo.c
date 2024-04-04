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

#include <stdio.h>
#include <rtc.h>

void date(void)
{
	DATE oggi;
	get_date(&oggi);
	printf("\nData = %d/%d/%d", oggi.fields.dom,
	       oggi.fields.month, (oggi.fields.year + 2000));
	return;
}

void ora(void)
{
	TIME adesso;
	get_time(&adesso);
	kprint("\nOra = %d:%d:%d", adesso.fields.hour,
	       adesso.fields.minute, adesso.fields.second);
	return;
}
