
#ifndef	_SEMAPHORO_
#define	_SEMAPHORO_

#include	"config.h"

#include	"scheduler.h"

#define	ROSSO	0
#define	VERDE	1

typedef	int	semaphoro_t;

void	semaphoro_down(semaphoro_t*	sem);
void	semaphoro_up(semaphoro_t*	sem);

bool	wsemaphoro(unsigned long sem);

#endif

