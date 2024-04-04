#ifndef __IRQ_H
#define __IRQ_H

#include <mydef.h>
#define TOT_IRQ 16  

void (*irq_selector[TOT_IRQ])(void);

void exec_irq(dword);
void do_irq(dword *);
void init_irq();
void add_irq(void (*)(), int);

#endif
