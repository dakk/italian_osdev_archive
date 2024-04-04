#ifndef _DELAY_H_
#define _DELAY_H_

#include <stddef.h>

#ifdef __cplusplus
	extern "C" void delay(dword milliseconds);
#else
	void delay(dword milliseconds);
#endif

dword calibrateDelayLoop(void);

#endif
