 /*
 * Dreamos
 * clock.c
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

#include <clock.h>
#include <io.h>
#include <stdio.h>

void readrtc()
{
//Legge il clock del cmos
    asm("cli"); //Disabilito gli interrupt

    printf("Ora: %02x:%02x.%02x - %s %02x %s - %02x\n",get_hour(),get_minute(), get_seconds(), get_day_week(),get_daymonth(), get_month(),0x2000+get_year());
   asm("sti");
}

char* get_day_week()
{
    int c;
    outportb(DAY_W_RTC, 0x70);
    c=inportb(0x71);
    switch(c) {
        case 1:
                return "Dom";
                break;
        case 2:
                return "Lun";
                break;
        case 3:
                return "Mar";
                break;
        case 4:
                return "Merc";
                break;
        case 5:
                return "Gio";
                break;
        case 6:
                return "Ven";
                break;
        case 7:
                return "Sab";
                break;
        }
return "";
}

char *get_month()
{
//Function that translate month number in char.
int c;
outportb(MONTH, 0x70);
c = inportb(0x71);

    switch(c) {
        case 1:
                return "Gen";
                break;
        case 2:
                return "Feb";
                break;
        case 3:
                return "Mar";
                break;
        case 4:
                return "Apr";
                break;
        case 5:
                return "Mag";
                break;
        case 6:
                return "Giu";
                break;
        case 7:
                return "Lug";
                break;
        case 8:
                return "Ago";
                break;
        case 9:
                return "Set";
                break;
        case 10:
                return "Ott";
                break;
        case 11:
                printf("Nov");
                break;
        case 12:
                printf("Dec");
                break;
        }
        return " ";
}

int get_hour()
{
    int c;
    outportb(HOUR_RTC, 0x70);
    c = inportb(0x71);
    return c;
}

int get_minute()
{
    int c;
    outportb(MIN_RTC, 0x70);
    c = inportb(0x71);
    return c;
}

int get_seconds()
{
    int c;
    outportb(SEC_RTC, 0x70);
    c = inportb(0x71);
    return c;
}

int get_daymonth()
{
    int c;
    outportb(DAY_MONTH, 0x70);
    c = inportb(0x71);
    return c;
}

int get_year()
{
    int c;
    outportb(YEAR, 0x70);
    c = inportb(0x71);
    return c;
}
