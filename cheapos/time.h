
#ifndef	_TIME_
#define	_TIME_

#include	"config.h"

//Brutto ma necessario!
struct time_tm_t
	{
	unsigned char ore;				//(0 - 23)
	unsigned char min;				//(0 - 59)
	unsigned char sec;				//(0 - 59)
	};

struct time_infocmos_t
	{
	struct time_tm_t data;
	bool invalid;
	};

#include	"io.h"
#include	"isr.h"
#include	"irq.h"

#include	"cmos.h"
#include	"kmath.h"


#define	TICK_PERIOD		10			//	Tempo di richiamo della ISR time(in millisecondi!)
#define	TIME_JOBS_NUM	256


typedef void (*time_job_t)(void);

struct time_table_t
	{
	time_job_t func;			//Funzione
	unsigned long	tickperiod;	//Periodo in tick
	unsigned long	tickcount;		//Conto dei tick
	unsigned int volte;			//Numero di volte in cui ripetere l'azione : se zero ripetuta all'infinito!
	};

void time_init();

unsigned long long	time_tick();								//Restituisce l'ora del sistema in tick
void				time_tm(struct time_tm_t* buffer);				//riempie Buffer con una struttura del tipo tm_t

unsigned long long	time_tm2tick(struct time_tm_t* data);			//Da tm_t ticks
void	time_tick2tm(struct time_tm_t* data,unsigned long long tick);		//Dat ticks a tm_t

unsigned char time_addTJ(unsigned long tickperiod,unsigned int volte,time_job_t func);
//bool time_clearTJ(unsigned long tickperiod,time_job_t func);

bool time_isinvalid();

#endif
