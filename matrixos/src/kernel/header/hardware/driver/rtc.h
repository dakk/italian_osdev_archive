/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: rtc.h			|
	| Date: 31/07/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

#ifndef _HARDWARE_DRIVER_RTC_H_
#define _HARDWARE_DRIVER_RTC_H_


/*
	**************************
	**	 Include	**
	**************************
*/

#define RTC_SECONDS      0
#define RTC_MINUTES      2
#define RTC_HOURS        4
#define RTC_DAY_OF_MONTH 7
#define RTC_MONTH        8
#define RTC_YEAR         9
        

struct time{
	int sec;
	int min;
	int hour;
	int day;
	int month;
	int year;
};

namespace hardware{

	namespace driver{
		class RTC{
		public:
			static void Init(); //Inizializza il driver per il real time clock
		};	
	
	
	}

}

#endif
