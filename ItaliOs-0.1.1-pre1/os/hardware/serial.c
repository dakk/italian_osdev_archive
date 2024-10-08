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
#include <mytypes.h>

//commentare queste 2 righe per aggiungere i messaggi di debug
#undef debug
#define debug(str, ...)

extern void comisr(void);



struct internal_serial_info {
	unsigned short int address;	/*indirizzo in cui � memorizzata la seriale */
	unsigned char irq;	/*numero dell'irq */
	BOOL occupato;
};

//numero di seriali massimo supportate.
#define MAX_UART 4		//in futuro sar� 4

unsigned short int NUMBER_OF_COMS;	//numero di porte instllate

unsigned char default_irq[] = { 4, 3, 4, 3 };

struct internal_serial_info serial[MAX_UART];	//definisco i descrittori per le 4 seriali

SERIALE *pserial[MAX_UART];

unsigned int uart_getchar_polling(SERIALE * ser)
{
	if ((inb(ser->address + LSR) & RICEZIONE) == RICEZIONE)	//riceve
		return inb(ser->address + RX);
	return NO_DATA;
}

void uart_gets_polling(SERIALE * ser, unsigned char *string)
{
	while ((*string++ = uart_getchar_polling(ser)) != '\0');
}

void uart_puts_polling(SERIALE * ser, unsigned char *string)
{
	while (*string != '\0')
		uart_putchar_polling(ser, *string++);
	return;
}

void uart_putchar_polling(SERIALE * ser, unsigned char car)
{
	while ((inb(ser->address + LSR) & TRASMISSIONE) != TRASMISSIONE);
	outportb(ser->address + TX, car);
	debug("\nho scritto all'indirizzo 0x%x il carattere %d", ser->address,
	      car);
};



SERIALE *uart_open(unsigned char number, unsigned char mode,
		   unsigned int velocita, unsigned char parita,
		   unsigned char len_word, unsigned char stop_bit,
		   unsigned char irq)
{
	SERIALE *ser;
	if (serial[number - 1].occupato == TRUE)
		return NULL;
	debug("\nApro la seriale numero %d", number - 1);
	pserial[number - 1] = ser = malloc(sizeof(SERIALE));
	if (!(number <= NUMBER_OF_COMS && number > 0))
		return NULL;

	//setto il descrittore  
	ser->address = serial[number - 1].address;	//setto l'indirizzo della porta
	debug("\nil suo indirizzo vale 0x%x", ser->address);
	//ser->chip = serial[number - 1].chip;  //setto il chip

	if (mode == INTR) {	//se � settato in modalit� interrupt
		if (irq == DEFAULT_IRQ) {
			serial[number - 1].irq = ser->irq = default_irq[number - 1];	//setto l'irq
			debug("\nirq settato a %d", ser->irq);
		} else if (irq < 16) {
			serial[number - 1].irq = ser->irq = irq;
			debug("\nirq settato a %d", ser->irq);
		} else {
			debug("\nERRORE: irq %d non valido", irq);
			return NULL;	//se � un irq non valido non alloco la porta
		}
		ser->buffer = malloc(sizeof(unsigned char) * DIM_BUFFER);
		ser->pos.inizio = ser->buffer;
		ser->pos.fine = DIM_BUFFER;
		ser->pos.posr = ser->pos.posw = 0;
		disable_irq(ser->irq);
		set_vector(comisr, M_VEC + ser->irq,
			   (D_INT + D_PRESENT + D_DPL1));
		enable_irq(ser->irq);
	} else {		//vado in polling e il buffer non serve
		debug("\nConfigurazione del polling");
		ser->buffer = NULL;
	}

	//abilito l'uso dell'interupt, forzo che la richiesta per spedire e
	//forzo che i dati terminali siano pronti
	outportb(ser->address + MCR, 0xB);
	debug("\nMCR impostato");

	//abilito la con la parit� alta
	outportb(ser->address + LCR, parita | len_word | stop_bit);
	debug("\nLCR impostato");

	//decido se abilitare l'interrupt oppure no
	outportb(ser->address + IER, mode);
	debug("\nIER impostato");

	//setto la velocirt� della seriale
	set_UART_speed(ser, velocita);
	debug("\nvelcit� impostata");

	serial[number - 1].occupato == TRUE;

	return ser;
}

void detect_UARTs()
{
	unsigned short int *ptraddress;
	unsigned short int i = 0;
	ptraddress = (unsigned short int *) 0x00000400;
	NUMBER_OF_COMS = 0;
	for (i = 0; i < MAX_UART; i++) {
		if (*ptraddress) {
			serial[i].address = *ptraddress;
			serial[i].occupato == FALSE;
			printf("\nCOM%d settata all'indirizzo 0x%x ", i + 1,
			       serial[i].address);
			++(ptraddress);
			NUMBER_OF_COMS++;
		}
	}
}

unsigned int set_UART_speed(SERIALE * ser, unsigned int speed)
{
	unsigned short int divisor;
	unsigned char div, div1;
	unsigned char status;
	unsigned short int pcom = ser->address;
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

void close_port(SERIALE * ser)
{
	if (ser->buffer != NULL)
		free(ser->buffer);
	free(ser);
}

void isr_com(void)
{
	unsigned int irq, i;
	unsigned char stato;

	asm("cli");

	irq = get_cur_irq();
	//irq = 3;
	debug("\nsono dentro l'irq %d", irq);
	i = 0;
	while ((irq != serial[i].irq) && (i <= NUMBER_OF_COMS))
		i++;		//cero la porta corrispondente a quell'irq
	debug("\ndevo usare la seriale %d", i);
	do {
		stato = inb(serial[i].address + LSR);
		if ((stato & 1) > 0) {	/* se dato pronto */
			debug("\nc'� un dato da leggere");
			add_char_to_buffer(pserial[i],
					   inb(pserial[i]->address + RX));
		}
	} while (stato & 1);
	outb(EOI, M_PIC);
	asm("sti");
	return;
}

void add_char_to_buffer(SERIALE * ser, unsigned char car)
{
	debug("\naggiungo al buffer %d", car);
	ser->pos.inizio[ser->pos.posw++] = car;
	debug("\nho aggiunto il carattere nella posizione %d", ser->pos.posw);
	if (ser->pos.posw == ser->pos.fine) {
		ser->pos.posw = 0;
		debug("\nho dovuto resettare il buffer, e ora punta a zero");
	}
}

unsigned int uart_getchar_interrupt(SERIALE * ser)
{
	//debug("\nla lettura � posizionata %d", ser->pos.posr);
	if (ser->pos.posr == ser->pos.fine) {
		ser->pos.posr = 0;
		debug("\nho letto fine alla fine quindi riposizione il buffer all'inizio");
	}
	if (ser->pos.posr == ser->pos.posw)
		return NO_DATA;
	debug("\nho un dato nel buffer ed esso � %d",
	      ser->pos.inizio[ser->pos.posr]);
	return ser->pos.inizio[ser->pos.posr++];
}


void uart_gets_interrupt(SERIALE * ser, unsigned char *string)
{
	while ((*string++ = uart_getchar_interrupt(ser)) != '\0');
}


asm(".globl comisr        \n" "comisr:              \n" "   pusha               \n"	/* Save all registers               */
    "   pushw %ds           \n"	/* Set up the data segment          */
    "   pushw %es           \n" "   pushw %ss           \n"	/* Note that ss is always valid     */
    "   pushw %ss           \n" "   popw %ds            \n" "   popw %es            \n" "                       \n" "   call isr_com           \n" "                       \n" "   popw %es            \n" "   popw %ds            \n"	/* Restore registers                */
    "   popa                \n" "   iret                \n"	/* Exit interrupt                   */
	);
