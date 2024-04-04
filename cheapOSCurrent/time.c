
#include	"time.h"


ISR_DECLARE(time);

unsigned long long tick;    //intero a 64 bit 1tick= 10 millisec piu' o meno

bool	time_invalid;		//Se l'ora in CMOS non risulta valida!

struct time_table_t time_table[TIME_JOBS_NUM];


void time_init()
	{
	int i;
	unsigned int temp,freq;
	struct cmos_dati_t	daticmos;

	cmos_getdati(&daticmos);					//Prende i dati da CMOS
	tick=time_tm2tick(&daticmos.timeinfo.data);		//Data in tick

	time_invalid=daticmos.timeinfo.invalid;			//Ora non valida

	freq=1000/TICK_PERIOD;
	temp=1193280/freq;

	outportb(0x43,0x30+0x6);					//Inizializzo timer del PIC (generatore di onda!)

	outportb(0x40,(unsigned char)temp);			//Manda alla PIC la freq dell'onda!!
	outportb(0x40,(unsigned char)(temp>>8));

	for (i=0;i<TIME_JOBS_NUM;i++)
		time_table[i].tickperiod=0;

	isr_setISR(0x20,ISR_ADDRESS(time));

	irq_enable(0);
	}

unsigned long long time_tick()
	{
	return tick;
	}

bool time_isinvalid()
	{
	return time_invalid;
	}

void	time_tm(struct time_tm_t* buffer)
	{
	time_tick2tm(buffer,tick);

	return;
	}

unsigned long long	time_tm2tick(struct time_tm_t* data)
	{
	unsigned long long temp;

	temp=data->sec+data->min*60+data->ore*3600;

	temp*=(1000/TICK_PERIOD);

	return temp;
	}

void	time_tick2tm(struct time_tm_t*data,unsigned long long tick)
	{
	unsigned int sec,min,ore;

	sec=((unsigned long)tick/(1000/TICK_PERIOD));

	min=sec/60;
	ore=min/60;

	min-=ore*60;
	sec-=min*60+ore*3600;

	ore%=24;

	data->sec=sec;
	data->min=min;
	data->ore=ore;

	return;
	}

unsigned char time_addTJ(unsigned long tickperiod,unsigned int volte,time_job_t func)
	{
	int i;
	unsigned char jobnum;

	irq_disable(0);

	jobnum=0;

	for(i=1;i<TIME_JOBS_NUM && !jobnum;i++)
		{
		if(!time_table[i].tickperiod)
			{
			time_table[i].func=func;
			time_table[i].tickperiod=tickperiod;
			time_table[i].tickcount=1;
			time_table[i].volte=volte;
			jobnum=i;
			}
		}


	irq_enable(0);

	return jobnum;
	}


ISR_CODE_H(time)
	{
	int i;

	tick++;

	//	TIME JOBS!!!
	for(i=1;i<TIME_JOBS_NUM;i++)
		if(time_table[i].tickperiod)
			{
			if(time_table[i].tickcount>=time_table[i].tickperiod)
				{
				time_table[i].tickcount=0;
				time_table[i].func();

				if (time_table[i].volte==1)
					time_table[i].tickperiod=0;
				if (time_table[i].volte>1)
					time_table[i].volte--;
				}
			time_table[i].tickcount++;
			}

	irq_end();
	}
