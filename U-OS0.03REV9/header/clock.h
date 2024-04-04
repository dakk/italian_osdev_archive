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
void readdate(void);
void readtime(void);

char* get_day_week(void);
char* get_month(void);
int get_hour(void);
int get_minute(void);
int get_year(void);
int get_seconds(void);
int get_daymonth(void);
#endif
