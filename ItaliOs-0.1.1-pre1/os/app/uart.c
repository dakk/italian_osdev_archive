#include <app.h>
#include <keyboard.h>
#include <io.h>
#include <mytypes.h>
#include <video.h>
#include <stdio.h>
#include <serial.h>

#define ESC 0x1b


BOOL tastiera(SERIALE * ser)	//funzione che trasmette sulla seriale
{
	unsigned char c[100];
	unsigned char car;
	unsigned char text_color = get_text_color();
	set_text_color(VERDE);
	getstring(c);
	uart_puts_polling(ser, c);
	/*while(1){
	 * car = getch();
	 * putchar(car);
	 * uart_putchar_polling(ser, car);
	 * } */
	set_text_color(text_color);
	return (c[0] == ESC) ? TRUE : FALSE;
}

void lettura(SERIALE * ser)
{
	unsigned char text_color = get_text_color();
	unsigned int c;
	//set_text_color(VIOLA);
	c = uart_getchar_interrupt(ser);
	if (c != NO_DATA) {
		//printf("\nho letto all'indirizzo 0x%x il carattere %d", ser->address, c);
		putchar(c);
		//printf("\n%c", c);
	}
	//set_text_color(text_color);
	return;
}

void chatta()
{
	BOOL h = FALSE;
	SERIALE *ser, *ser2;
	unsigned int porta;
	printf("\nScegli la porta: ");
	 /** printf("\n1 : COM1");
	 * printf("\n1 : COM2");
	 * printf("\nScelta: ");
	 * port = get_number();
	 * if(port>2 || port <1){
	 * printf("\nPorta non valida */
	porta = get_number();

	if ((ser =
	     uart_open(porta, INTR, 1200, NO_PARITY, BITS_8_PER_WORD,
		       STOP_BIT_1, DEFAULT_IRQ)) == NO_DEVICE) {
		printf("\nERRORE: impossibile aprire COM1");
		return;
	}
	/*if ((ser2 = uart_open(2, POLLING, 1200, NO_PARITY, BITS_8_PER_WORD,
	 * STOP_BIT_1, DEFAULT_IRQ)) == NO_DEVICE) {
	 * printf("\nERRORE: impossibile aprire COM2");
	 * return;
	 * } */
	while (!h) {
		if (kbhit())
			h = tastiera(ser);
		else
			lettura(ser);
	}
}
