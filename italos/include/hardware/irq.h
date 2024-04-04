#ifndef _IRQ_H_
#define _IRQ_H_

#include <stddef.h>
#include <8259.h>

#ifdef __cplusplus
	extern "C"{
#endif

/*funzione per aggiungere un irq*/
void add_irq(void (*func)(), byte number);

void init_irq();

#ifdef __cplusplus
	};
#endif
	
#endif
