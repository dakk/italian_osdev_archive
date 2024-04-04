
#ifndef	_PIDCODA_
#define	_PIDCODA_

#include	"config.h"

#include	"kconsole.h"

//Elemento della coda
struct pidcoda_t
	{
	int dato[(MAX_TASK_NUM+1)];
	int	inizio;
	int	 fine;
	};

//gestione della coda dei PIDs
void	pidcoda_init(struct pidcoda_t* pidcoda);			
int	pidcoda_elements(struct pidcoda_t* pidcoda);

int	pidcoda_pop(struct pidcoda_t* pidcoda);		//Restituisce -1 se la coda e' vuota
int	pidcoda_ins(struct pidcoda_t* pidcoda,int pid);

void	pidcoda_dumpscreen(struct pidcoda_t* pidcoda);
int	pidcoda_dump(char* buffer,struct pidcoda_t* pidcoda);

#endif
