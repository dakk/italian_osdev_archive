#include <io.h>
#include <vga.h>
#include <stdio.h>
#include <speaker.h>
#include <stdlib.h>
#include <time.h>
#include <vga_mode.h>
#include <font.h>
#include <forme_vga.h>



#ifdef _SERIAL_DEBUG_
#include <serial.h>
SERIALE *ser;
#endif

struct palette {
	int rosso;
	int verde;
	int blu;
};

char *videomem = (char *) 0xA0000;

/*imposta tutti i registri della vga*/
void SetMode(unsigned char *registers)
{

	int i, j, k;

	i = 0;
	outb(registers[i++], MISC_ADDR_WR);
	DELAY();
	outb(registers[i++], STATUS_ADDR);
	DELAY();


	inb(STATUS_ADDR);
	for (j = 0; j < 5; j++) {
		outb(j, SEQ_ADDR);	//imposto il registro da modificare
		DELAY();
		outb(registers[i++], SEQ_DATA);	//gli scrivo dentro
		DELAY();
	}

	outb(0x11, CRTC_ADDR);
	DELAY();
	outb(0xE, CRTC_DATA);
	DELAY();
	for (j = 0; j < 25; j++) {
		outb(j, CRTC_ADDR);
		DELAY();
		outb(registers[i++], CRTC_DATA);
		DELAY();
	}


	inb(STATUS_ADDR);
	for (j = 0; j < 9; j++) {
		outb(j, GRACON_ADDR);
		DELAY();
		outb(registers[i++], GRACON_DATA);
		DELAY();
	}

	inb(STATUS_ADDR);
	for (j = 0; j < 21; j++) {
		inb(ATTRCON_ADDR);
		DELAY();
		outb(j, ATTRCON_ADDR);
		DELAY();
		outb(registers[i++], ATTRCON_ADDR);
		DELAY();
	}
	outb(0x20, ATTRCON_ADDR);
}




/*imposta tutti i registri della vga*/
void GetMode(unsigned char *registers)
{

	int i, j, k;

	i = 0;
	registers[i++] = inb(MISC_ADDR_RD);
	DELAY();
	registers[i++] = inb(STATUS_ADDR);
	DELAY();


	inb(STATUS_ADDR);
	for (j = 0; j < 5; j++) {
		outb(j, SEQ_ADDR);	//imposto il registro da modificare
		DELAY();
		registers[i++] = inb(SEQ_DATA);
		DELAY();
	}

	DELAY();
	for (j = 0; j < 25; j++) {
		outb(j, CRTC_ADDR);
		DELAY();
		registers[i++] = inb(CRTC_DATA);
		DELAY();
	}


	inb(STATUS_ADDR);
	for (j = 0; j < 9; j++) {
		outb(j, GRACON_ADDR);
		DELAY();
		registers[i++] = inb(GRACON_DATA);
		DELAY();
	}

	inb(STATUS_ADDR);
	for (j = 0; j < 21; j++) {
		inb(ATTRCON_ADDR);
		DELAY();
		outb(j, ATTRCON_ADDR);
		DELAY();
		//outb(registers[i++], ATTRCON_ADDR);
		registers[i++] = inb(ATTRCON_DATA);
		DELAY();
	}
}


void setpal(int color, unsigned char rosso, unsigned char verde,
	    unsigned char blu)
{
	outb(color, 0x3c8);
	// Send color
	outb(rosso, 0x3c9);
	outb(verde, 0x3c9);
	outb(blu, 0x3c9);
}

struct palette getpal(int color)
{
	struct palette my_pal;
	outb(color, 0x3c8);
	// get color
	my_pal.rosso = inb(0x3c9);
	my_pal.verde = inb(0x3c9);
	my_pal.blu = inb(0x3c9);
	return my_pal;
}


void set_256_pal(void)
{
	int i;
	int colore = 0;
	for (i = 0; i < 64; i++) {
		setpal(i + 30, i, 0, 0);
		setpal(i + 94, 0, i, 0);
		setpal(i + 158, 0, 0, i);
		//setpal(i+212, i,i,i);

		//colore += 2;
	}

	//imposto il giallo alla posizione 20
	setpal(20, 255, 255, 0);

	for (i = 0; i < 34; i++)
		setpal(i + 222, i + 30, i + 30, i + 30);

	/*for(; i<; i++){
	 * setpal(i+94, 0,i+16,0);
	 * }
	 * for(i=0; i<100; i++){
	 * setpal(i+144, 0,0,i+16);
	 * } */

	/*for(i=0; i<60; i++){
	 * setpal(i+164, i,i,i);
	 * } */

}



//da testare
void set_complete_palette(unsigned char *pal)
{
	int i = 0;
	for (i = 0; i < 100; i++)
		setpal(i, *pal++, *pal++, *pal++);
	return;
}

void putpixel(int x, int y, int color)
{
	unsigned char str[10];
	videomem[x + 320 * y] = color;
#ifdef _SERIAL_DEBUG_
	uart_puts_polling(ser, "\n\rSto stampando alla posizione: ");
	itoa(x, str);
	uart_puts_polling(ser, str);
	uart_puts_polling(ser, " ,  ");
	itoa(y, str);
	uart_puts_polling(ser, str);
	itoa(color, str);
	uart_puts_polling(ser, " il colore  ");
	uart_puts_polling(ser, str);
	uart_puts_polling(ser, "\n");
#endif
}

void init_vga(void)
{
	videomem = (unsigned char *) 0xA0000;
	//imposto la modalità 320x240x256

	GetMode(mode03h);
	SetMode(mode13h);
	////setpalette256();
	//setpal(VGA_GIALLO, 255, 255, 0);      //setto il colore giuallo che non è presente nella palette standard
	set_256_pal();
	//set_complete_palette(palette);
}

unsigned int get_max_x(void)
{
	return 320;
}


unsigned int get_max_y(void)
{
	return 200;
}


void putchar_graph(unsigned char c, int x, int y, int fgColor, int bgColor)
{
	int i, j;
	int glyphY = 8;

	for (i = 0; i < glyphY; i++) {
		for (j = 0; j < 8; j++) {
			if (font1[i + 8 * c] & (128 >> j)) {
				putpixel(x + j, y + i, fgColor);
			} else {
				putpixel(x + j, y + i, bgColor);
			}
		}
	}
}

void puts_graph(unsigned char *str, int x, int y, int fgColor, int bgColor)
{
	char *c;
	int dx, dy;
	int offset;
	offset = 0;
	dy = 0;
	dx = 8;
	c = str;

	while (*c != '\0') {
		putchar_graph(*c, x + offset, y + dy, fgColor, bgColor);
		c++;
		offset += dx;
	}
}

void clear_screen(unsigned int color)
{
	unsigned int i;
	for (i = 0; i < 320 * 200; i++)
		videomem[i] = color;
	return;
}

int test_vga(void)
{
	unsigned int i = 0;
	unsigned int colore;

	unsigned char str[10];

	unsigned char rosso[10];
	unsigned char verde[10];
	unsigned char blu[10];

	struct palette mypal;

#ifdef _SERIAL_DEBUG_
	if ((ser =
	     uart_open(1, INTR, 1200, NO_PARITY, BITS_8_PER_WORD,
		       STOP_BIT_1, DEFAULT_IRQ)) == NO_DEVICE) {
		printf("\nERRORE: impossibile aprire COM1");
		return;
	}
#endif

	init_vga();
	clear_screen(VGA_BIANCO);
	//for (i = 0; i < 256; i++) {
	i = 0;
	while (1) {
		clear_screen(i);
		mypal = getpal(i);
		//linea(i, 0, i, get_max_y(), i);
		itoa(i, str);
		itoa(mypal.rosso, rosso);
		itoa(mypal.verde, verde);
		itoa(mypal.blu, blu);

		puts_graph(str, 50, 50, 255 - i, i);

		puts_graph(rosso, 90, 50, 255 - i, i);
		puts_graph(verde, 115, 50, 255 - i, i);
		puts_graph(blu, 150, 50, 255 - i, i);

		colore = get_number();
		if (!colore)
			i++;
		else
			i = colore;
	}
	while (1);
	SetMode(mode03h);
//      outb(0x0c)
	cls();
	return 0;
}
