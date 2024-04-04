#include <clock.h>
#include <io.h>
#include <stdio.h>

void readrtc()
{
    asm("cli");
    	printf("rtc: %02x:%02x.%02x - %s %02x %s - %02x\n",get_hour(),get_minute(), get_seconds(), get_day_week(),get_daymonth(), get_month(),0x2000+get_year());
    asm("sti");
}

void readtime()
{
    asm("cli");
    	printf("time: %02x:%02x:%02x\n",get_hour(),get_minute(),get_seconds());
    asm("sti");
}

void readdate()
{
    asm("cli");
    	printf("date: %02x %s %02x\n",get_daymonth(), get_month(),0x2000+get_year());
    asm("sti");
}

char* get_day_week()
{
    int c;
    outportb(DAY_W_RTC, 0x70);
    c=inportb(0x71);
    switch(c) {
        case 1:
                return "Su";
                break;
        case 2:
                return "Mo";
		
                break;
        case 3:
                return "Tu";
                break;
        case 4:
                return "We";
                break;
        case 5:
                return "Th";
                break;
        case 6:
                return "Fr";
                break;
        case 7:
                return "Sa";
                break;
        }
return "";
}

char *get_month()
{

int c;
outportb(MONTH, 0x70);
c = inportb(0x71);

    switch(c) {
        case 1:
                return "January";
                break;
        case 2:
                return "February";
                break;
        case 3:
                return "March";
                break;
        case 4:
                return "April";
                break;
        case 5:
                return "May";
                break;
        case 6:
                return "June";
                break;
        case 7:
                return "July";
                break;
        case 8:
                return "August";
                break;
        case 9:
                return "September";
                break;
        case 10:
                return "October";
                break;
        case 11:
                return "November";
                break;
        case 12:
                return "December";
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
