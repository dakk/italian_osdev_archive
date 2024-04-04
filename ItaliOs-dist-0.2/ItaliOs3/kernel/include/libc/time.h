#ifndef _TIME_H_
#define _TIME_H_

typedef int clock_t;
typedef unsigned int time_t;

struct tm{
  int tm_sec;			/* Secondi	[0-60] */
  int tm_min;			/* Minuti	[0-59] */
  int tm_hour;			/* Ore		[0-23] */
  int tm_mday;			/* Giorni	[1-31] */
  int tm_mon;			/* Mesi.	[0-11] */
  int tm_year;			/* Anno	- 1900.  */
  int tm_wday;			/* Giorno della settimana	[0-6] 0 == Domenica */
  int tm_yday;			/* Giorno dell'anno [0-365]	*/
  int tm_isdst;			/* Flag dell'ora legale		[-1/0/1]*/
};

/* Ritorna il tempo corrente. Se timer non vale NULL imposta anche la bariabile timer*/
time_t time (time_t *timer);

/* Ritorna time1 - time0 */
double difftime (time_t time1, time_t time0);

/* Ritorna la rappresentazione in time_t di tp e normalizza TP*/
time_t mktime (struct tm *tp);


/* Ritorna la struct tm che rappresenta il tempo timer */
struct tm *gmtime (time_t *timer);

/* Per il momento identica a gmtime */
struct tm *localtime (time_t *timer);

/* Restituisce un puntatore ad una stringha statica che ha questo formato  "Gio Mes dd hh:mm:ss yyyy\n"*/
char *asctime (struct tm *tp);

/* Idendica ad  `asctime (localtime (timer))'.  */
char *ctime (time_t *timer); 

#endif
