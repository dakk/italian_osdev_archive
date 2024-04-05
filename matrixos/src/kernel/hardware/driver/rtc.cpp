/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: rtc.cpp			|
	| Date: 31/07/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

/*
	**************************
	**	 Include	**
	**************************
*/ 

#include <hardware/io.h>
#include <kdef.h>
#include <hardware/driver/rtc.h>
#include <kservices/kout.h>

using namespace hardware::driver;

/*
  Legge dalla memoria CMOS
*/
int cmosread(int addr){
	outportb(0x70, addr|0x80);
	return inportb(0x71);
}

/*
  Converte un numero da BCD a binario
*/
int bcd2bin(byte number){
	return ((number & 0x0F) + (number >> 4)*10);
}

/*
  Inizializza il driver per il real time clock
*/
void RTC::Init(){

	asm("cli");
	
	time rtc_time;

	do{
		rtc_time.sec = bcd2bin(cmosread(RTC_SECONDS));
		rtc_time.min = bcd2bin(cmosread(RTC_MINUTES));
		rtc_time.hour = bcd2bin(cmosread(RTC_HOURS));
	
		rtc_time.day = bcd2bin(cmosread(RTC_DAY_OF_MONTH));
		rtc_time.month = bcd2bin(cmosread(RTC_MONTH));
		rtc_time.year = bcd2bin(cmosread(RTC_YEAR));
	
	}while(rtc_time.sec != bcd2bin(cmosread(RTC_SECONDS)));
	
	asm("sti");

	kservices::KOut::Write("Hardware time: ");
	
	kservices::KOut::Write((unsigned int) rtc_time.hour);
	kservices::KOut::Write(":");
	kservices::KOut::Write((unsigned int) rtc_time.min);
	kservices::KOut::Write(":");
	kservices::KOut::Write((unsigned int) rtc_time.sec);
	
	kservices::KOut::Write(" ");
	
	kservices::KOut::Write((unsigned int) rtc_time.day);
	kservices::KOut::Write("/");
	kservices::KOut::Write((unsigned int) rtc_time.month);
	kservices::KOut::Write("/");
	kservices::KOut::WriteLine((unsigned int) rtc_time.year);
}
