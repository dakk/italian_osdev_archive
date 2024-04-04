
#ifndef	_KEYCODA_
#define	_KEYCODA_

#include	"config.h"

#include	"kconsole.h"

#define	KEY_BUFFER_LUNG	20

struct keycoda_t
	{
	unsigned char dato[(KEY_BUFFER_LUNG+1)];
	int	inizio;
	int	 fine;
	};


void	keycoda_init(struct keycoda_t* keycoda);
int	keycoda_elements(struct keycoda_t* keycoda);

unsigned char	keycoda_pop(struct keycoda_t* keycoda);			//Restituisce 0 se la coda e' vuota
int	keycoda_ins(struct keycoda_t* keycoda,unsigned char key);		//Restituisce 0 se la coda e' piena

void	keycoda_dumpscreen(struct keycoda_t* keycoda);
int	keycoda_dump(char* buffer,struct keycoda_t* keycoda);

#endif
