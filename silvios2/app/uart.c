#include <app.h>
#include <keyboard.h>
#include <io.h>
#include <mytypes.h>
#include <video.h>
#include <stdio.h>
#include <serial.h>

#define ESC 0x1b


BOOL tastiera(unsigned short int number)	//funzione che trasmette sulla seriale
{
	unsigned char c[100];
	unsigned char text_color = get_text_color();
	set_text_color(VERDE);
	getstring(c);
	uart_puts_polling(number, c);
	//putchar(c);
	set_text_color(text_color);
	return (c[0] == ESC) ? TRUE : FALSE;
}

void lettura(unsigned short int base)
{
	unsigned char text_color = get_text_color();
	unsigned int c;
	set_text_color(VIOLA);
	c = uart_getchar_interrupt(base);
	if (c != NO_DATA)
		putchar(c);
	set_text_color(text_color);
	return;
}

void chatta()
{
	BOOL h = FALSE;
	unsigned int port;
	/*printf("\nScegli la porta: ");
	 * printf("\n1 : COM1");
	 * printf("\n1 : COM2");
	 * printf("\nScelta: ");
	 * port = get_number();
	 * if(port>2 || port <1){
	 * printf("\nPorta non valida */

	if ((port =
	     uart_open(1, INTR, 1200, NO_PARITY, BITS_8_PER_WORD,
		       STOP_BIT_1)) == NO_DEVICE) {
		printf("\nERRORE: impossibile aprire COM1");
		return;
	}
/*	if ((in = uart_open(2, INTR, 155000)) == NO_DEVICE) {
		printf("\nERRORE: impossibile aprire COM2");
		return;
		}*/
	while (!h) {
		if (kbhit())
			h = tastiera(port);
		else
			lettura(port);
	}
}
