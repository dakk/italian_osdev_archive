/* pic8259.c -- ia32 architecture pic8259 irq chip */
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

#include <arch-ia32/pic8259.h>
#include <arch-ia32/io.h>

static unsigned char cache_master = 0xFB;   // IRQ2 sul master è collegato allo slave, quindi lo lascio abilitato
static unsigned char cache_slave = 0xFF;

int ia32_pic8259_init(void)
{
	/* invia ICW1 */
	outportb(PIC_MASTER,PIC_ICW1);
	outportb(PIC_SLAVE,PIC_ICW1);
	/* invia ICW2 */
	outportb(PIC_MASTER + 1,0x20);
	outportb(PIC_SLAVE + 1,0x28);
	/* invia ICW3 */
	outportb(PIC_MASTER + 1,4);
	outportb(PIC_SLAVE + 1,2);
	/* invia ICW4 */
	outportb(PIC_MASTER + 1,PIC_ICW4);
	outportb(PIC_SLAVE + 1,PIC_ICW4);
	/* disabilita tutti gli irq */
	outportb(PIC_MASTER + 1,cache_master);
	outportb(PIC_SLAVE + 1,cache_slave);
	return 0;
}

/* abilita l'irq <nr> */
int ia32_pic8259_enable_irq(unsigned int nr)
{
	if(nr > PIC_MAXIRQ) return -1;
	unsigned char mask;
	mask = ~(1 << (nr & 7));
	if(nr < (PIC_NUMIRQ/2))	// bello :P
	{
		cache_master &= mask;
		outportb(PIC_MASTER + 1,cache_master);
	}
	else
	{
		cache_slave &= mask;
		outportb(PIC_SLAVE + 1,cache_slave);
	}
	return 0;
}

/* disabilita l'irq <nr> */
int ia32_pic8259_disable_irq(unsigned int nr)
{
	if(nr > PIC_MAXIRQ) return -1;
	unsigned char mask;
	mask = 1 << nr;
	if (nr < (PIC_NUMIRQ/2))
	{
		cache_master |= mask;
		outportb(PIC_MASTER + 1,cache_master);
	}
	else
	{
		cache_slave |= mask;
		outportb(PIC_SLAVE + 1,cache_slave);
	}
	return 0;
}

int ia32_pic8259_disable_all_irqs() {
	cache_master=0xFB; // questo deve essere 0xFB perché l'irq2 deve essere attivo per il collegamento con l'altro
	cache_slave=0xFF;
	outportb(PIC_MASTER+1,cache_master);
	outportb(PIC_SLAVE+1,cache_slave);
	return 0;
}

int ia32_pic8259_enable_all_irqs() {
	cache_master=0x00;
	cache_slave=0x00;
	outportb(PIC_MASTER+1,cache_master);
	outportb(PIC_SLAVE+1,cache_slave);
	return 0;
}

int ia32_pic8259_eoi(unsigned int irqn)
{
	if(irqn >= 8) outportb(PIC_SLAVE,PIC_EOI);
	outportb(PIC_MASTER,PIC_EOI);
	return 0;
}
