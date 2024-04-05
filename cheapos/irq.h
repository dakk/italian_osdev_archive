
#ifndef	_IRQ_
#define	_IRQ_

#include	"config.h"

#include	"io.h"

void irq_pic_init(unsigned char interrupt_irq_start);//Non usare inizzializza il gestore degli IRQ

void irq_enable(unsigned short irq);				//Abilita l'IRQ 'irq'
void irq_disable(unsigned short irq);			//Disabilita l'IRQ 'irq'

inline void irq_end();						//Comunica alla PIC la fine di un IRQ (ora ne possono accadere altri!)

#endif
