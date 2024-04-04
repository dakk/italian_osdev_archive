/*
ItaliOs
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <mem.h>
#include <stdio.h>
#include <serial.h>
#include <io.h>
#include <8259.h>
#include <gdt.h>
#include <idt.h>

extern void com1_3_isr(void);
extern void com2_4_isr(void);

/*dimensione del buffer di tutte le seriali.
Esso viene partizionato in parti uguali per ogni seriale.
QUindi se ci sono 2 seriali ognuna avrà a diosposizione 0x2500 bytes*/
#define DIM_BUFFER  0x5000


//numero di seriali massimo supportate.
//Non basta cambiare il valore per far funzionere + seriali
#define MAX_UART 2		//in futuro sarà 4

struct bufferpos {
	unsigned int fine;	//numero di byte dall'inizio del blocco
	unsigned char *inizio;	//puntatore all'inizio del blocco di memoria
	unsigned int posw;	//indica quanti bytes sono stti scritti
	unsigned int posr;	//indica quanti bytes sono stti lettti
};

unsigned short int NUMBER_OF_COMS;	//numero di porte instllate

unsigned char buffer[DIM_BUFFER];	//dichiarazione del buffer

struct bufferpos pbuffer[MAX_UART];	//dichiarazione dgli indici dei buffer

unsigned short int com[MAX_UART];	//indirizzo di ogni porta, se è a zero la porta non esiste



unsigned int uart_getchar_polling(unsigned short int number)
{
	if ((inb(com[number] + LSR) & RICEZIONE) == RICEZIONE)	//riceve
		return inb(com[number] + RX);
	return NO_DATA;
}

void uart_gets_polling(unsigned short int number, unsigned char *string)
{
	while ((*string++ = uart_getchar_polling(number)) != '\0');
}

void uart_puts_polling(unsigned short int number, unsigned char *string)
{
	while (*string != '\0')
		uart_putchar_polling(number, *string++);
	return;
}

void uart_putchar_polling(unsigned short int number, unsigned char car)
{
	while ((inb(com[number] + LSR) & TRASMISSIONE) != TRASMISSIONE);
	outportb(com[number] + TX, car);
};


//setto l'irq numero 3
void init_UART_irq3()
{
	disable_irq(3);
	set_vector(com2_4_isr, M_VEC + 3, (D_INT + D_PRESENT + D_DPL1));
	enable_irq(3);
}

//setto l'irq numero 4
void init_UART_irq4()
{
	disable_irq(4);
	set_vector(com1_3_isr, M_VEC + 4, (D_INT + D_PRESENT + D_DPL1));
	enable_irq(4);
}
unsigned int uart_open(unsigned char number, unsigned char mode,
		       unsigned int velocita, unsigned char parita,
		       unsigned char len_word, unsigned char stop_bit)
{
	if (!(number <= NUMBER_OF_COMS && number > 0))
		return NO_DEVICE;
	//abilito l'uso dell'interupt, forzo che la richiesta per spedire e
	//forzo che i dati terminali siano pronti
	outportb(com[number - 1] + MCR, 0xB);

	//abilito la con la parità alta
	outportb(com[number - 1] + LCR, parita | len_word | stop_bit);

	//decido se abilitare l'interrupt oppure no
	outportb(com[number - 1] + IER, mode);

	//setto la velocirtà della seriale
	set_UART_speed(number, velocita);

	pbuffer[number - 1].posr = pbuffer[number - 1].posw = 0;	//setto la posizione del buffer a 0
	return number - 1;
}

void detect_UARTs()
{
	unsigned short int *ptraddress;
	unsigned short int i = 0;
	ptraddress = (unsigned short int *) 0x00000400;
	NUMBER_OF_COMS = 0;
	for (i = 0; i < MAX_UART; i++) {
		if (*ptraddress) {
			com[i] = *ptraddress;
			printf("\nCOM%d settata all'indirizzo 0x%x", i + 1,
			       com[i]);
			++(ptraddress);
			NUMBER_OF_COMS++;
		}
	}
	if (com[0]) {
		init_UART_irq4();
		pbuffer[0].inizio = buffer;
		pbuffer[0].fine = DIM_BUFFER / NUMBER_OF_COMS;
		pbuffer[0].posr = pbuffer[0].posw = 0;
	}
	if (com[1]) {
		init_UART_irq3();
		pbuffer[1].inizio = &buffer[DIM_BUFFER / NUMBER_OF_COMS];
		pbuffer[1].fine = DIM_BUFFER / NUMBER_OF_COMS;
		pbuffer[1].posr = pbuffer[1].posw = 0;
	}
}

unsigned int set_UART_speed(unsigned short int number, unsigned int speed)
{

	unsigned short int divisor;
	unsigned char div, div1;
	unsigned char status;
	unsigned short int pcom = com[number];
	status = inb(pcom + LCR);
	outb(0x80, pcom + LCR);
	divisor = MAXSPEED / speed;
	div = (char) (divisor & 0x00FF);
	outb(div, pcom + DLL);
	div = (char) (divisor & 0xFF00) >> 8;
	outb(div, pcom + DLM);
	outb(status, pcom + LCR);
	divisor = 0;
	outb(0x80, pcom + LCR);
	div = inb(pcom + DLM);
	div1 = inb(pcom + DLL);
	divisor = (unsigned short int) (div << 8) + (unsigned short int) div1;
	outb(status, pcom + LCR);
	return (divisor) ? (MAXSPEED / divisor) : 0;
}


void isr_com13(void)
{
	unsigned char stato;
	//printf("\nisr chiamato");
	stato = inb(com[0] + LSR);
	/*
	 * se il bit 0 del LSR vale 1 allora un dato e pronto nel
	 * RDR (read data register). Questo bit viene automaticamente
	 * resettato quando si fa una lettura nell'RDR
	 */

	if ((stato & 1) > 0) {	/* se dato pronto */
//              if(pbuffer[1].posw-1 < pbuffer[1].fine)&&(pbuffer[1].posw != pbuffer[1].posr)
		pbuffer[0].inizio[pbuffer[0].posw++] = inb(com[0] + RX);	/* leggo il carattere dal read data register */
		if (pbuffer[0].posw == pbuffer[0].fine)
			pbuffer[0].posw = 0;

		//      putchar(pbuffer[0].inizio[pbuffer[0].pos-1]);
	}
	outb(EOI, M_PIC);
	return;
}

void isr_com24(void)
{
	unsigned char stato;
	//printf("\nisr chiamato");
	stato = inb(com[1] + LSR);
	/*
	 * se il bit 0 del LSR vale 1 allora un dato e pronto nel
	 * RDR (read data register). Questo bit viene automaticamente
	 * resettato quando si fa una lettura nell'RDR
	 */

	if ((stato & 1) > 0) {	/* se dato pronto */
//              if(pbuffer[1].posw-1 < pbuffer[1].fine)
		pbuffer[1].inizio[pbuffer[1].posw++] = inb(com[1] + RX);	/* leggo il carattere dal read data register */
		if (pbuffer[1].posw == pbuffer[1].fine)
			pbuffer[1].posw = 0;
//              else
//                      pbuffer[1].inizio[pbuffer[1].posw=0] = inb(com[1] + RX);        /* leggo il carattere dal read data register */
		//putchar(pbuffer[1].inizio[pbuffer[1].pos-1]);
	}
	outb(EOI, M_PIC);
	return;
}


unsigned short int get_int_status(unsigned short int port,
				  unsigned short int interrupt)
{
	return (inb(port + IER) & IDMASK & interrupt) == interrupt ? 1 : 0;
}

unsigned int uart_getchar_interrupt(unsigned short int number)
{
	if (pbuffer[number].posr == pbuffer[number].fine)
		pbuffer[number].posr = 0;
	if (pbuffer[number].posr == pbuffer[number].posw)
		return NO_DATA;
	return pbuffer[number].inizio[pbuffer[number].posr++];
}


void uart_gets_interrupt(unsigned short int number, unsigned char *string)
{
	while ((*string++ = uart_getchar_interrupt(number)) != '\0');
}

asm(".globl com1_3_isr        \n" "com1_3_isr:              \n" "   pusha               \n"	/* Save all registers               */
    "   pushw %ds           \n"	/* Set up the data segment          */
    "   pushw %es           \n" "   pushw %ss           \n"	/* Note that ss is always valid     */
    "   pushw %ss           \n" "   popw %ds            \n" "   popw %es            \n" "                       \n" "   call isr_com13           \n" "                       \n" "   popw %es            \n" "   popw %ds            \n"	/* Restore registers                */
    "   popa                \n" "   iret                \n"	/* Exit interrupt                   */
	);


asm(".globl com2_4_isr        \n" "com2_4_isr:              \n" "   pusha               \n"	/* Save all registers               */
    "   pushw %ds           \n"	/* Set up the data segment          */
    "   pushw %es           \n" "   pushw %ss           \n"	/* Note that ss is always valid     */
    "   pushw %ss           \n" "   popw %ds            \n" "   popw %es            \n" "                       \n" "   call isr_com24           \n" "                       \n" "   popw %es            \n" "   popw %ds            \n"	/* Restore registers                */
    "   popa                \n" "   iret                \n"	/* Exit interrupt                   */
	);
