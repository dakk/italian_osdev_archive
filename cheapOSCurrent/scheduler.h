
#ifndef	_SCHEDULER_
#define	_SCHEDULER_


#include	"config.h"

#include	"kconsole.h"
#include	"task.h"
#include	"pidcoda.h"
#include	"isr.h"
#include	"syscall_task.h"	//Un po' Brutto!

#define	SCHEDULER_PERIOD	5		//Lo scheduler e' chiamato ogni ToT tick

#define	WAITCHECK_PERIOD	2		//Il WaitCheck	e' chiamato ogni ToT tick
#define	KWAIT_INT	0x42				//Interrupt per il kwait

typedef bool (*kwait_event_t)(unsigned long param);

struct kwait_table_element_t
	{
	kwait_event_t	func;
	unsigned long	param;
	};

bool	scheduler_init();
//bool	scheduler_shutdown();

void	scheduler();
void	kwait_check();

inline void kwait(kwait_event_t func,unsigned long param);	//Funzione da chiamare (usare questa per i driver!!)

#endif
