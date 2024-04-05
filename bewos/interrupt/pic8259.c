#include<interrupt.h>


/*Questa che segue è un array che serve per abilitare il giusto irq. Ricordo che se vogliamo abilitare l' irq
 0 (timer) dobbiamo inviare alla giusta porta (0x21 per master e 0xA1 per slave) 11111110 ovvero
 l' irq da attivare deve essere posto a 0. Se vogliamo abilitare il 1° irq (tastiera) allora inviamo 
 11111101 e così via. */

unsigned int irq_number[8] = {
			    0xfe, 	//11111110 - Master PIC: Timer     --  Slave PIC: Real Time Clock
			    0xfd,  	//11111101 - Master PIC: Tastiera  --  Slave PIC: Free
			    0xfb,	//11111011 - Master PIC: Slave PIC --  Slave PIC: Free
			    0xf7,	//11110111 - Master PIC: COM2/COM4 --  Slave PIC: Free
			    0xef,	//11101111 - Master PIC: COM1/COM3 --  Slave PIC: Free
			    0xdf,	//11011111 - Master PIC: LPT2      --  Slave PIC: Coprocessore
			    0xbf,	//10111111 - Master PIC: Floppy    --  Slave PIC: 1° hardisk
			    0x7f	//01111111 - Master PIC: LPT1      --  Slave PIC: 2° hardisk
			    };

/********************************************************************************
********************************************************************************/
// Inizializzo il PIC - Con il pic si comunica tramite le porte 0x20 - 0x21 per il master e 0xA0 - 0xA1 per il slave
void Init_PIC8259(void)
{

static const unsigned irq0_int = 0x20, irq8_int = 0x28;

/* Initialization Control Word #1 (ICW1) */
	outportb(0x11, 0x20);
	outportb(0x11, 0xA0);

/* Initialization Control Word #2 (ICW2) */

//route IRQs 0-7 to INTs 20h-27h
	outportb(irq0_int, 0x21); //Con questo comando facciamo in modo che all' irq0 corrisponda il vettore di interruzione 0x20, all' irq1 0x21 ecc.

// route IRQs 8-15 to INTs 28h-2Fh
	outportb(irq8_int, 0xA1); //Idem ma per lo slave

/* Initialization Control Word #3 (ICW3) */
	outportb(0x04, 0x21);
	outportb(0x02, 0xA1);

/* Initialization Control Word #4 (ICW4) */
	outportb(0x01, 0x21);
	outportb(0x01, 0xA1);

}
/********************************************************************************
********************************************************************************/
void abilita_irq(unsigned char irq_num, unsigned char port_num)
{
    outportb(irq_number[irq_num], port_num);  /* Invia a portnum (0x21 o 0xA1) la mask dell' irq da attivare. 
					         L'array irq_number è definita in interrupt.h dove è possibile
					         trovare maggiori chiarmenti. */
}
