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
#include <video.h>
#include <io.h>
#include <stdio.h>
#include <delay.h>
#include <stdarg.h>
#include <mytypes.h>
#include <speaker.h>

extern unsigned char text_color;
extern unsigned char back_color;

static int x = 0, y = 0;	/* cursor coords */

int abs_y = 0;

//unsigned long long SEME;

//void srand(unsigned long long seed){
//SEME = seed;
//}

//unsigned long long rand(void){
//return get_ticks()*SEME/987654321;
//}



void getstring(char *string)
{
	unsigned long long count = 0;
	unsigned char c = 0;
	unsigned char tempstring[2];
	int myx;

	while (!get_key_control());

	myx = x;

	tempstring[1] = 0;

	while (1) {
		c = nbgetch();
		if (c == 8 && myx >= x)
			continue;
		if (c == 10)
			break;
		else if (c == 8 && count > 0)
			count -= 2;
		else if (c == 255 || c == 1)
			continue;
		else
			string[count] = c;
		tempstring[0] = c;
		kprint(tempstring);
		count++;
	}

	put_key_control();
	string[count] = '\0';
}

static BOOL key_control = FALSE;	//inizialmente nessuno prende il controllo della tastiera

BOOL get_key_control()
{
	if (!key_control)	//se non è settato
		return key_control = TRUE;	//quel task prende il controllo della tastirta
	return FALSE;		//pasienza la tastiera è occupata
}

void put_key_control()
{
	key_control = FALSE;
	return;
}

//legge solo le prime cinque cifre di un int
int get_number()
{
	unsigned char c[5];
	int number = 0;
	unsigned char tempstring[2];
	int index = 0;
	int i;

	while (!get_key_control());

	while (1) {
		if (index < 5) {
			c[index] = nbgetch();
			tempstring[1] = 0;
			if ((c[index] >= '0') && (c[index] <= '9')) {
				tempstring[0] = c[index];
				kprint(tempstring);
				index++;
				continue;
			}
			if (c[index] == 10)
				break;
			if (c[index] == 8) {
				tempstring[0] = c[index];
				kprint(tempstring);
				index--;
				continue;
			}
		} else {
			if (nbgetch() == '\n')
				break;
		}
	}


	for (i = 0; i < index; i++) {
		number *= 10;
		number += (c[i] - '0');
	}

	put_key_control();

	return number;
}


unsigned char getch()
{
	unsigned char ret_key = 0;
	unsigned int loop = 0;

	while (!get_key_control());

	while (*buffer_size == 0) {
		delay(10);
	}

	ret_key = keyboard_buffer[0];
	peppe--;

	for (loop = 0; loop < 254; loop++)
		keyboard_buffer[loop] = keyboard_buffer[loop + 1];

	put_key_control();

	return ret_key;
}

//questa getch non riechiede l'uso esclusibvo della tastiera
unsigned char nbgetch()
{
	unsigned char ret_key = 0;
	unsigned int loop = 0;


	while (*buffer_size == 0) {
		delay(10);
	}

	ret_key = keyboard_buffer[0];
	peppe--;

	for (loop = 0; loop < 254; loop++)
		keyboard_buffer[loop] = keyboard_buffer[loop + 1];

	return ret_key;
}

void scroll_up(void);
void update_HW_cursor(void);

//setta da dove deve iniziare la visualizzazione della memoria video
void set_videomem_start_offset(unsigned short int offset)
{
	outb(0xD, 0x3D4);
	outb((offset & 0xFF), 0x3D5);
	outb(0xC, 0x3D4);
	outb((offset >> 8), 0x3D5);
}

//determina da aove inizia la visualizzazione della sk video
unsigned short int get_videomem_start_offset(void)
{
	unsigned short int t;
	outb(0xD, 0x3D4);
	t = inb(0x3D5);
	outb(0xC, 0x3D4);
	t |= inb(0x3D5) << 8;
	return t;
}

//determina la posizone del cursore
void get_initial_cursor_pos(void)
{
	char *punt;
	char data;
	punt = (char *) 0x450;
	data = *punt++;
	x = data;
	data = *punt;
	y = data;
	abs_y = y;
}

//stampa un accapo
void accapo(char **dest)
{
	char *oldest;
	oldest = *dest;
	x = 0;

	if (y >= 24) {
		scroll_up();
	} else {
		y++;
		abs_y++;
	}
	*dest = (char *) 0xb8000 + 160 * abs_y;

//riempio l'ultima righa
	for (; oldest <= (*dest) + 160 * 2; oldest += 2) {
		*oldest = ' ';
		*(oldest + 1) = text_color + back_color;
	}
	update_HW_cursor();
}

//stampa una stringha
void print_string(const char attribute, const char *str)
{
	int i;
	char *dest;
	dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */
	for (i = 0; str[i] != '\0'; i++) {
		if (str[i] == '\n')
			accapo(&dest);
		else {
			if (x == 80)
				accapo(&dest);
			*dest++ = str[i];
			*dest++ = attribute;
			x++;

		}
	}
	update_HW_cursor();
}



//stampa un carattere
void putc(const char attribute, const char ch)
{
	char *dest;
	dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */

	if (ch == '\a') {
		beep();
		return;
	}

	if (ch == '\n')
		accapo(&dest);
	else if (ch == 127) {
		if (x > 0) {
			x--;
			dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */
			*dest = ' ';
		}
/*
 * else do nothing 
 */
	} else if (ch == 8) {	/*backspace */
		x--;
		dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */
		*dest = 0x20;
	} else if (ch == '\t') {
		x += 8;
		dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */
		*dest = ' ';
	} else {
		if (x == 80)
			accapo(&dest);
		*dest++ = ch;
		*dest++ = attribute;
		x++;
	}

	update_HW_cursor();
}

void putchar(unsigned char car)
{
	putc(get_text_color() + get_back_color(), car);
	return;
}

//stampa un numeroi intero con segno
void print_decimal(const char attribute, signed int d)
{
	char tmp[20];
	int i;
	char *dest;
	dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */
	i = 0;
	if (d < 0) {
		if (x == 80)
			accapo(&dest);
		*dest++ = '-';
		*dest++ = attribute;
		d = -d;
		x++;
	}
	do {
		tmp[i++] = '0' + d % 10;
		d /= 10;
	}
	while (d > 0);
	for (i--; i >= 0; i--) {
		if (x == 80)
			accapo(&dest);
		*dest++ = tmp[i];
		*dest++ = attribute;

		x++;
	}

	update_HW_cursor();
}

//stampa un numero intero senza segnop
void print_unsigned(const char attribute, unsigned int d)
{
	char tmp[20];
	int i;
	char *dest;
	dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */
	i = 0;
	do {
		tmp[i++] = '0' + d % 10;
		d /= 10;
	}
	while (d > 0);
	for (i--; i >= 0; i--) {
		if (x == 80)
			accapo(&dest);
		*dest++ = tmp[i];
		*dest++ = attribute;

		x++;
	}
	update_HW_cursor();
}

//stampa un numero binario
void print_binary(const char attribute, unsigned int d)
{
	char tmp[35];
	int i;
	char *dest;
	dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */
	i = 0;
	do {
		tmp[i++] = '0' + (d & 1);
		d >>= 1;
	}
	while (d > 0);
	for (i--; i >= 0; i--) {
		if (x == 80)
			accapo(&dest);
		*dest++ = tmp[i];
		*dest++ = attribute;

		x++;
	}
	update_HW_cursor();
}

//stampa un numero esadecimale
void print_ex(const char attribute, unsigned int d)
{
	char tmp[10];
	int i;
	char *dest;
	dest = (char *) (0xb8000 + 2 * x + 160 * abs_y);	/* Video memory coords */
	i = 0;
	do {
		if ((d & 0xF) > 9)
			tmp[i++] = 'A' + (d & 0xF) - 10;
		else
			tmp[i++] = '0' + (d & 0xF);
		d >>= 4;
	}
	while (d > 0);
	for (i--; i >= 0; i--) {
		if (x == 80)
			accapo(&dest);
		*dest++ = tmp[i];
		*dest++ = attribute;

		x++;
	}

	update_HW_cursor();
}


void print_float(float num)
{
	int i, v;
	i = num;
	num -= i;
	v = num * 10000;
	printf("%d.%d", i, v);
	return;
}

unsigned char get_tot_color(void)
{
	return text_color + back_color;
}

//kernel print
void kprint(char *format, ...)
{
	unsigned int curchar, varnumber = 0, curpos = 0;
	unsigned char *tmp;
	unsigned char attribute = get_tot_color();
	va_list pt = pt;	//serve solo ad eliminare un warning :-)

//detrmino il numero di argomenti che mi vengono passati
	while ((curchar = format[curpos++]))
		if (curchar == '%')
			varnumber += 1;	//memorizza la quantità di argomenti

	if (varnumber)		// inizializzo le ellissi
		va_start(pt, format);

	while ((curchar = *format++)) {	/* Loop through the string        */
		switch (curchar) {	/* Is it a special character ?  */
		case '\n':	// travato un invio
		case '\t':	//trovato un tab
		case 8:	//trovato un backspace
			putc(attribute, curchar);
			break;

		case '%':	//elaboro le variabili
			curchar = *format++;
			switch (curchar) {

			case 'd':
			case 'l':	//nella maggior parte delle implementazione ungiuale a %d
				print_decimal(attribute,
					      (int) va_arg(pt, int));
				break;

			case 'u':	//stampo un intero senza segno
				print_unsigned(attribute,
					       (unsigned int) va_arg(pt,
								     unsigned
								     int));
				break;
			case 'f':
				print_float((float) va_arg(pt, float));
				break;


			case 'b':	//stampo un numero binario
				print_binary(attribute,
					     va_arg(pt, unsigned long));
				break;
			case 'x':	//stampo un numero esadecimale
				print_ex(attribute,
					 va_arg(pt, unsigned long));
				break;

			case 's':	//stampo una stringa
				tmp = va_arg(pt, unsigned char *);
				print_string(attribute, tmp);
				break;
			}
			break;

		default:	//stampo un carattere normale
			putc(attribute, curchar);
			break;
		}

	}
	va_end(pt);

	while (x >= 80) {	// vedo se sono all'ultima colonna
		x -= 80;
		if (y >= 24)
			scroll_up();
		else {
			y++;
			abs_y++;
		}
	}

	update_HW_cursor();

}


//resetto i registri del cursore
void update_HW_cursor(void)
{
	unsigned int i;
	i = x + 80 * abs_y;	// calcolo la coordinata

	if (abs_y >= 24)
		set_videomem_start_offset(80 * (abs_y - 24));	//se supero le 21 righe setto un nuovo inizio per la memoria video
//aggiorno i registri con la nuova posizione
	outb(0xF, 0x3D4);
	outb((i & 0xFF), 0x3D5);
	outb(0xE, 0x3D4);
	outb((i >> 8), 0x3D5);
}


/* Scrolling */
void scroll_up(void)
{
	int i;
	unsigned int *source, *dest, top;


	if (abs_y >= 150) {
		source = (unsigned int *) (0xb8000 + 160);	/* Second row ... */
		dest = (unsigned int *) 0xb8000;	/* First row */
		top = (160 * (abs_y)) / 4;

		for (i = 0; i < top; i++)
			*dest++ = *source++;	/* COPIA */
		for (i = 0; i < 80; i++) {	/* Empties last row */
			*((unsigned char *) dest)++ = ' ';
			*((unsigned char *) dest)++ = text_color + back_color;	/* Setting attribute */
		}
	} else
		abs_y++;


}



//scorre il video in alto
void scroll_video_down(void)
{
	unsigned short int t;
	t = get_videomem_start_offset();
	if (t < (abs_y - 24) * 80) {
		if (t <= (abs_y - 34) * 80)
			set_videomem_start_offset(t + 800);
		else
			set_videomem_start_offset((abs_y - 24) * 80);
	}
	return;
}

//scorre il video in basso
void scroll_video_up(void)
{
	unsigned short int t;
	t = get_videomem_start_offset();
	if (t > 0) {
		if (t >= 800)
			set_videomem_start_offset(t - 800);
		else
			set_videomem_start_offset(0);
	}
	return;
}







/*gestione dei  file */
//FILE *fopen(unsigned char *filename, unsigned char *mode){
