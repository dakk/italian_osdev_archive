#include <stdlib.h>
#include <io.h>
#include <vga.h>
#include <stdio.h>
#include <speaker.h>
#include <delay.h>
#include <time.h>


unsigned char modeC4[62] = {
	// MISC reg,  STATUS reg,    SEQ regs
	0x63, 0x00, 0x03, 0x01, 0x0F, 0x00, 0x06,
	// CRTC regs
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0BF, 0x1F, 0x00, 0x41, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28, 0x00, 0x96, 0xB9, 0xE3, 0xFF,
	// GRAPHICS regs
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
	// ATTRIBUTE CONTROLLER regs
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
	0x0B,
	0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00
};

unsigned char mode12h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
	0xE3, 0x00, 0x03, 0x01, 0x0F, 0x00, 0x06,
// CRTC regs
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E, 0x00, 0x40, 0x00,
	0x00,
	0x00, 0x00, 0x00, 0x59,
	0xEA, 0x8C, 0x0DF, 0x28, 0x00, 0x0E7, 0x04, 0xE3, 0xFF,
// GRAPHICS regs
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F, 0xFF,
// ATTRIBUTE CONTROLLER regs
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
	0x0B,
	0x0C, 0x0D, 0x0E, 0x0F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};

unsigned char mode13h[62] = {
	// MISC reg,  STATUS reg,    SEQ regs
	0x63, 0x00, 0x03, 0x01, 0x0F, 0x00, 0x0E,
	// CRTC regs
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0BF, 0x1F, 0x00, 0x41, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0x0E3, 0xFF,
	// GRAPHICS regs
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF,
	// ATTRIBUTE CONTROLLER regs
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
	0x0B,
	0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00
};


void SetMode(unsigned char *registers)
{

	int i, j, k;

	i = 0;

	//****  regs in eax

	outb(registers[i++], MISC_ADDR);
	DELAY();
	outb(registers[i++], STATUS_ADDR);
	DELAY();
	for (j = 0; j < 5; j++) {
		outb(j, SEQ_ADDR);
		DELAY();
		outb(registers[i++], SEQ_ADDR + 1);
		DELAY();
	}

	outb(0x11, CRTC_ADDR);
	DELAY();
	outb(0xE, CRTC_ADDR + 1);
	DELAY();
	for (j = 0; j < 25; j++) {
		outb(j, CRTC_ADDR);
		DELAY();
		outb(registers[i++], CRTC_ADDR + 1);
		DELAY();
	}

	for (j = 0; j < 9; j++) {
		outb(j, GRACON_ADDR);
		DELAY();
		outb(registers[i++], GRACON_ADDR + 1);
		DELAY();
	}

	j = inb(STATUS_ADDR);
	for (j = 0; j < 21; j++) {
		inw(ATTRCON_ADDR);
		DELAY();
		outb(j, ATTRCON_ADDR);
		DELAY();
		outb(registers[i++], ATTRCON_ADDR);
		DELAY();
	}
	outb(0x20, ATTRCON_ADDR);


}


void setpal(int color, char r, char g, char b)
{
	__asm__("out %%al,%%dx\n"::"a"(color), "d"(0x3C8));
	// Send color
	__asm__("out %%al,%%dx\n"::"a"(r), "d"(0x3C9));
	__asm__("out %%al,%%dx\n"::"a"(g), "d"(0x3C9));
	__asm__("out %%al,%%dx\n"::"a"(b), "d"(0x3C9));


}

void set_palette(TYPE_PALETTE * palette)
{
	int i;

	for (i = 0; i < 256; i++)
		setpal(i, palette[i].r, palette[i].g, palette[i].b);
}

void setpalette4()
{
	setpal(0, 0, 0, 0);
	setpal(1, 0, 42, 42);
	setpal(2, 42, 0, 42);
	setpal(3, 63, 63, 63);
}

void setpalette256()
{
	int j = 0;
	for (j = 0; j < 64; j++)
		setpal(j, j, 0, 0);

	for (j = 0; j < 64; j++)
		setpal(j + 64, 0, j, 0);

	for (j = 0; j < 32; j++)
		setpal(j + 128, 0, 0, j * 2);
	for (j = 32; j < 64; j++)
		setpal(j + 128, 0, 0, 64 - 2 * (j - 32));

	for (j = 0; j < 64; j++)
		setpal(j + 192, j, j, j);
}



typedef char row[320];

char *videomem;

risoluzione attuale;

void putpixel(int x, int y, int color)
{
	videomem[x + attuale.width * y] = color;
}

void PutLine(int x1, int y1, int x2, int y2, char color)
{
	int i, deltax, deltay, numpixels,
		d, dinc1, dinc2, x, xinc1, xinc2, y, yinc1, yinc2;

	/* Calcoliamo il deltax ed il deltay della linea, ovvero il numero di pixel presenti a livello
	 * orizzontale e verticale. Utilizziamo la funzione abs() poichè a noi interessa il loro
	 * valore assoluto. */
	deltax = abs(x2 - x1);
	deltay = abs(y2 - y1);

	/* Adesso controlliamo se la linea è più "orizzontale" o "verticale", ed inizializziamo
	 * in maniera appropriate le variabili di comodo. */
	if (deltax >= deltay) {
		/* La linea risulta maggiormente schiacciata sull' ascissa */
		numpixels = deltax + 1;

		/* La nostra variabile decisionale, basata sulla x della linea */
		d = (2 * deltay) - deltax;

		/* Settiamo gli incrementi */
		dinc1 = deltay * 2;
		dinc2 = (deltay - deltax) * 2;
		xinc1 = xinc2 = 1;
		yinc1 = 0;
		yinc2 = 1;
	} else {
		/* La linea risulta maggiormente schiacciata sull' ordinata */
		numpixels = deltay + 1;

		/* La nostra variabile decisionale, basata sulla y della linea */
		d = (2 * deltax) - deltay;

		/* Settiamo gli incrementi */
		dinc1 = deltax * 2;
		dinc2 = (deltax - deltay) * 2;
		xinc1 = 0;
		xinc2 = 1;
		yinc1 = yinc2 = 1;
	}

	/* Eseguiamo un controllo per settare il corretto
	 * andamento della linea */
	if (x1 > x2) {
		xinc1 = -xinc1;
		xinc2 = -xinc2;
	}
	if (y1 > y2) {
		yinc1 = -yinc1;
		yinc2 = -yinc2;
	}

	/* Settiamo le coordinate iniziali  */
	x = x1;
	y = y1;

	/* Stampiamo la linea */
	for (i = 1; i < numpixels; i++) {
		putpixel(x, y, color);

		/* Scegliamo l' incremento del "passo" a seconda dellla
		 * variabile decisionale */
		if (d < 0) {
			d = d + dinc1;
			x = x + xinc1;
			y = y + yinc1;
		} else {
			d = d + dinc2;
			x = x + xinc2;
			y = y + yinc2;
		}
	}
}



void drawCircle(int mX, int mY, int radius, int c)
{
	int schmu = 10000;
	int x, y, d;

	x = 0;
	y = radius;
	d = (5 * schmu / 4) - (radius * schmu);
	putpixel(x + mX, y + mY, c);
	putpixel(y + mX, x + mY, c);
	putpixel(y + mX, -x + mY, c);
	putpixel(x + mX, -y + mY, c);
	putpixel(-x + mX, -y + mY, c);
	putpixel(-y + mX, -x + mY, c);
	putpixel(-y + mX, x + mY, c);
	putpixel(-x + mX, y + mY, c);

	while (y > x) {
		if (d < 0) {
			d = d + (2 * x + 3) * schmu;
			x++;
		} else {
			d = d + (2 * (x - y) + 5) * schmu;
			x++;
			y--;
		}
		putpixel(x + mX, y + mY, c);
		putpixel(y + mX, x + mY, c);
		putpixel(y + mX, -x + mY, c);
		putpixel(x + mX, -y + mY, c);
		putpixel(-x + mX, -y + mY, c);
		putpixel(-y + mX, -x + mY, c);
		putpixel(-y + mX, x + mY, c);
		putpixel(-x + mX, y + mY, c);
	}
}

void clear(unsigned char color)
{
	row *VideoMem;
	int i;
	VideoMem = (row *) 0xA0000;
	for (i = 0; i < 0xFFFF; i++)
		videomem[i] = color;
	return;
}

void init_vga(void)
{
	row *VideoMem;

	// MODE X  :  320 x 240 x 256
	VideoMem = (row *) 0xA0000;
	attuale.width = 320;
	attuale.heigth = 240;
	attuale.depth = 8;

	/*outb(MISC_ADDR, 0xE3);
	 * // turn off write protect
	 * outb(CRTC_ADDR, 0x2C11);
	 * // vertical total
	 * outb(CRTC_ADDR, 0x0D06);
	 * // overflow register
	 * outb(CRTC_ADDR, 0x3E07);
	 * // vertical retrace start
	 * outb(CRTC_ADDR, 0xEA10);
	 * // vertical retrace end AND wr.prot
	 * outb(CRTC_ADDR, 0xAC11);
	 * // vertical display enable end
	 * outb(CRTC_ADDR, 0xDF12);
	 * // start vertical blanking
	 * outb(CRTC_ADDR, 0xE715);
	 * // end vertical blanking
	 * outb(CRTC_ADDR, 0x0616); */


	outb(0xF, 0x3D4);
	outb(0, 0x3D5);
	outb(0xE, 0x3D4);
	outb(0, 0x3D5);

	SetMode(mode13h);
	setpalette256();

	outb(0xD, 0x3D4);
	outb(0, 0x3D5);
	outb(0xC, 0x3D4);
	outb(0, 0x3D5);

}

void init_text()
{
	SetMode(modeC4);
	return;
}


int test_vga(void)
{
	init_vga();
	srand((unsigned int) time());
	while (1) {
		drawCircle(rand() % 320, rand() % 240, rand() % 100,
			   rand() % 256);
		delay(50);
	}
	return 0;
}
