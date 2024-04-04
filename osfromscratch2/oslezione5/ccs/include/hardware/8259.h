#ifndef _8259_H_
#define  _8259_H_

#include <stddef.h>

enum{
        M_PIC =  0x20,     // I/O for master PIC              
        S_PIC =  0xA0   , // I/O for slave PIC               

	
        M_VEC =  0x30 ,    //Dove viene mappato IRQ0
        S_VEC =  M_VEC+8   ,//Dove viene mappato IRQ8
	
        ICW1 =  0x11 ,   // 0001   0001      
	
        ICW2_M = M_VEC,                        // ICW2 per il master
	ICW2_S = S_VEC    ,                    // ICW2 per lo slave
	
	ICW3_M =  (1 << 2),             // ICW3 per il master
	ICW3_S = 3                ,        // ICW3 per lo slave

        ICW4 =  0x01, //ICW4
};

//Inizializza l'8259
void init8259();
	
/*abilita la ricezioni di un irq*/
bool enable_irq(word irq_no);

/* disabilita la ricezione di un irq */
bool disable_irq(word irq_no);

//ritorna l'irq corrente
size_t get_cur_irq();

#endif
