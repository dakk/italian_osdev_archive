#ifndef _IRQ_H_
#define _IRQ_H_

/*funzione per aggiungere un irq*/
void add_irq(void (*func)(), byte number);

void init_irq();

#endif
