
#include	"semaphoro.h"


void	semaphoro_down(semaphoro_t*	sem)
	{
	kwait(wsemaphoro,(unsigned long)sem);

	return;
	}

bool	wsemaphoro(unsigned long sem)
	{
	if(	*((semaphoro_t*)sem)	== ROSSO	)
		return FALSE;

	*((semaphoro_t*)sem)=ROSSO;

	return TRUE;
	}

void	semaphoro_up(semaphoro_t*	sem)
	{
	*((semaphoro_t*)sem)=VERDE;

	return;
	}

