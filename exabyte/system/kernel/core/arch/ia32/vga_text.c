/* vga_text.c -- text video management definitions */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; either version 2              *
* of the License, or (at your option) any later version.                      *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License           *
* along with this program; if not, write to the Free Software                 *
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. *
*******************************************************************************/

#define VGA_TEXT_INTERNALS

#include <config.h>		// questo va SEMPRE per primo
#include <arch-ia32/vga_text.h>
#include <arch-ia32/io.h>
#include <arch-ia32/string.h>

#define VGA_COLOR_MEM		0xB8000
#define VGA_MONO_MEM		0xB0000

/* Porte VGA */
#define VGA_MISC_READ		0x3CC
#define VGA_COLOR_PORT		0x3D4
#define VGA_MONO_PORT		0x3B4

/* Comandi VGA */
#define VGA_SET_CURSOR_START	0xA
#define VGA_SET_CURSOR_END	0xB
#define VGA_SET_ADDRESS_HIGH	0xC
#define VGA_SET_ADDRESS_LOW	0xD
#define VGA_SET_CURSOR_HIGH	0xE
#define VGA_SET_CURSOR_LOW	0xF

static unsigned int max_x = 0;
static unsigned int max_y = 0;
static unsigned int cur_x = 0;
static unsigned int cur_y = 0;
static unsigned char * videomem = 0;
static unsigned short ioaddr = 0;

static void update_textcursor(void);
static void internal_putc(char c,unsigned char attr);
static void internal_puts(char * s,unsigned char attr);

/* inizializza il sistema videotext per arch ia32 */
int videotext_init(unsigned int max_w,unsigned int max_h,unsigned int last_y)
{
/* imposta altezza e larghezza dello schermo */
	if((!max_w) || (!max_h)) return -1;	// eh beh....
	max_x = max_w;
	max_y = max_h;
/* rileva tipo di monitor e relativo indirizzo video e porte i/o */
	if(inportb(VGA_MISC_READ) & 0x01)
	{	/* monitor a colori */
		videomem = (unsigned char *)VGA_COLOR_MEM;
		ioaddr = VGA_COLOR_PORT;
	}
	else
	{	/* monitor b/n */
		videomem = (unsigned char *)VGA_MONO_MEM;
		ioaddr = VGA_MONO_PORT;
	}
/* setta l'offset della memoria video */
	outb_p(VGA_SET_ADDRESS_HIGH,ioaddr);
	outb_p(0, ioaddr+1);
	outb_p(VGA_SET_ADDRESS_LOW,ioaddr);
	outb_p(0, ioaddr+1);
/* prepara la last_y e pulisce lo schermo se necessario */
	if(!last_y) videotext_clear(VDTXT_DEFAULT_BACK);
	cur_y = last_y + (last_y ? 1 : 0);
	if(cur_y >= max_y-1) scroll_up();
	update_textcursor();
	internal_puts(VIDEO_NORMAL1,VDTXT_DEFAULT_ATTR);
	if((unsigned long)videomem == VGA_COLOR_MEM) internal_puts(VIDEO_COLOR,VDTXT_DEFAULT_ATTR);
	else internal_puts(VIDEO_MONO,VDTXT_DEFAULT_ATTR);
	internal_puts(VIDEO_NORMAL2,VDTXT_DEFAULT_ATTR);
	return 0;
}

/* stampa un carattere <c> con attributi <attr> */
int videotext_putc(char c,unsigned char attr)
{
	unsigned int i;
	switch (c)
	{
		case '\0': /* non penso che stampare dei \0 ci faccia bene ;) */
			break;
		case '\r':	/* carriage return */
			cur_x = 0;
			break;
		case '\n':	/* new line */
			new_line();
			break;
		case '\b':	/* backspace */
			if(!cur_x) break;
			cur_x--;
			internal_putc(0x0,attr);
			break;
		case '\t':
			/* TODO: allinea il tab alla griglia */
			for(i=0;i<TAB_SIZE;i++) internal_putc(0x0,attr);
			cur_x += TAB_SIZE;	// incrementa x di TAB_SIZE
			break;
		default:
			internal_putc(c,attr);
			cur_x++;
	}
	if(cur_x >= max_x)
	{
		cur_y += (cur_x / max_x);  // new line
		cur_x = (cur_x % max_x);
	}
	if(cur_y >= max_y)
	{
		scroll_up();
		cur_y = max_y-1;
	}
	update_textcursor();
	return 0;
}

/* pulisce lo schermo con sfondo <attr> */
int videotext_clear(unsigned char attr)
{
	unsigned int i;
	for (i = 0; i < (max_x * max_y); i++)
	{
		videomem[i * 2] = 0x0;
		videomem[i * 2 + 1] = attr;
	}
	cur_x = 0;
	cur_y = 0;
	update_textcursor();
	return 0;
}

/* funzioni interne */

/* scrolla di una riga la schermata */
void scroll_up(void)
{
	short *memvideo = (short*)videomem;
	/* muove la memoria di una riga */
	memmove(memvideo,&memvideo[max_x],sizeof(short)*(max_x)*(max_y-1));
	/* cancella l'ultima riga */
	clear_line(max_y-1);
	cur_y = max_y - 1;
	update_textcursor();
}

/* pulisce una linea <n> */
void clear_line(int n)
{
	char *mem;
	char *line_end;
	if (n<0 || (unsigned int)n>max_y-1) return;
	mem = (char*)videomem + (max_x*2)*n;
	line_end = mem + (max_x*2);
	for (; mem<line_end; mem+=2)
	{
		*mem = 0x0;
		*(mem+1) = VDTXT_DEFAULT_BACK;
	}
	update_textcursor();
}

/* new line ;) */
void new_line(void)
{
	cur_x = 0;
	cur_y++;
	if(cur_y >= max_y-1) scroll_up();
	update_textcursor();
}

/* funzioni private */

/* aggiorna il cursore visivo */
static void update_textcursor(void)
{
	outb_p(VGA_SET_CURSOR_HIGH,ioaddr);	// low byte
	outb_p((cur_y * max_x + cur_x) >> 8,ioaddr+1);
	outb_p(VGA_SET_CURSOR_LOW,ioaddr);	// high byte
	outb_p(cur_y * max_x + cur_x,ioaddr+1);
}

/* internal putc ;) */
static void internal_putc(char c,unsigned char attr)
{
	*(char *)(videomem + 2 * cur_x + (max_x * 2) * cur_y) = c;
	*(char *)((videomem + 2 * cur_x + (max_x * 2) * cur_y)+1) = attr;
}

static void internal_puts(char * s,unsigned char attr)
{
	unsigned int i,c=strlen(s);
	for(i=0;i<c;i++) videotext_putc(s[i],attr);
}
