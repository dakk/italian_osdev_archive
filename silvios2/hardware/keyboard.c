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

#include <keyboard.h>
#include <io.h>
#include <video.h>
#include <idt.h>
#include <8259.h>
#include <gdt.h>
#include <stdio.h>
#include <map.h>

unsigned char map[104][11];

unsigned char led_status = 0;

/* keyboard_buffer stores every key typed */
unsigned char keyboard_buffer[255];

unsigned int peppe = 0;
int *buffer_size = &peppe;

unsigned char control_keys = 0;
unsigned char code224 = 0;

extern void keyb_ISR();






unsigned int oldkey = 0;

asm(".globl keyb_ISR        \n" "keyb_ISR:              \n" "   pusha               \n"	/* Save all registers               */
    "   pushw %ds           \n"	/* Set up the data segment          */
    "   pushw %es           \n" "   pushw %ss           \n"	/* Note that ss is always valid     */
    "   pushw %ss           \n" "   popw %ds            \n" "   popw %es            \n" "                       \n" "   call keyb_handler   \n" "                       \n" "   popw %es            \n" "   popw %ds            \n"	/* Restore registers                */
    "   popa                \n" "   iret                \n"	/* Exit interrupt                   */
	);



void setleds()
{
	while (inportb(0x64) & 2);
	outportb(0x60, 0xED);
	while (inportb(0x64) & 2);
	outportb(0x60, led_status);
}

void InitKeyboard()
{
	set_vector(keyb_ISR, M_VEC + 1, D_PRESENT + D_INT + D_DPL3);	/* IRQ1 Handler */
	enable_irq(1);

	led_status = 0;
	setleds();

	init_map();

	peppe = 0;
}

void keyb_handler(void)
{
	unsigned int key = inportb(0x60);



	unsigned char ch = 0;

	//printf("\n%d", key);

	//printf("\n%d", key);

	/*codice 224 */


	if (code224 && key < 128) {
		oldkey = key;
	}

	if (key == 224) {
		if (!code224)
			code224 = !code224;
	}

	/* 'LED Keys', ie, Scroll lock, Num lock, and Caps lock */
	else if (key == 0x3A) {	/* Caps Lock */
		led_status ^= LED_CAPS_LOCK;
		setleds();
	} else if (key == 0x45) {	/* Num Lock */
		led_status ^= LED_NUM_LOCK;
		setleds();
	} else if (key == 0x46) {	/* Scroll Lock */
		led_status ^= LED_SCROLL_LOCK;
		setleds();
	} else if (key == 29) {	/* Ctrl key */
		control_keys |= CK_CTRL;
	} else if (key == (29 + 128)) {	/* Ctrl key depressed */
		control_keys &= ~CK_CTRL;
	} else if ((key == 42 || key == 54)) {	/* Shift key */
		control_keys |= CK_SHIFT;
	} else if ((key == (42 + 128)) || (key == (54 + 128))) {	/* Shift key depressed */
		control_keys &= ~CK_SHIFT;
	} else if (key == 56) {	/*alt key */
		control_keys |= CK_ALT;
	} else if (key == (56 + 128)) {	/*alt key depressed */
		control_keys &= ~CK_ALT;
	}

	if (!code224) {
		if (control_keys == (CK_CTRL | CK_SHIFT | CK_ALT)) {
			ch = map[key][8];
		} else if (control_keys == (CK_CTRL | CK_ALT)) {
			ch = map[key][7];
		} else if (control_keys == (CK_CTRL | CK_SHIFT)) {
			ch = map[key][6];
		} else if (control_keys == (CK_ALT | CK_SHIFT)) {
			ch = map[key][5];
		} else if ((control_keys & CK_SHIFT)
			   && (led_status & LED_CAPS_LOCK)) {
			ch = map[key][10];
		} else if ((control_keys == 0)
			   && ((led_status & LED_CAPS_LOCK))) {
			ch = map[key][9];
		} else if ((control_keys == 0)
			   && (!(led_status & LED_CAPS_LOCK))) {
			ch = map[key][1];
		} else if (control_keys & CK_CTRL) {
			ch = map[key][3];
		} else if (control_keys & CK_ALT) {
			ch = map[key][4];
		} else if (control_keys & CK_SHIFT) {
			ch = map[key][2];
		}
	} else {
		//tabella speciale
		if (key == oldkey + 128)	//disabilito il code224
			code224 = !code224;
		if (key == 73)	/*pagina su */
			scroll_video_up();
		else if (key == 81)
			scroll_video_down();
	}
	if (key <= 103) {

		if (ch) {
			keyboard_buffer[peppe] = ch;
			peppe++;
		}
	}

	outportb(M_PIC, EOI);

	/*kprint("\n");
	 * printlong(peppe); */
}

unsigned char kbhit()
{
	if (peppe == 0)
		return 0;

	return 1;
}
