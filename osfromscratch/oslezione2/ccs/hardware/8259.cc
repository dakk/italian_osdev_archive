 /*ItaliOs
 
 Copyright  (c) <2003> Abruzzo Silvio
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
7
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <IA32.h>
#include <8259.h>
#include <stddef.h>

using namespace itos;
using namespace itos::IA32;


dword irq_mask = 0xFFFF;	// All IRQs disabled initially

/*Inizializza l'8259*/
void Init8259(void)
{
	outportb_p(m_pic, icw1);	/* Start 8259 initialization    */
	outportb_p(s_pic, icw1);

	outportb_p(m_pic + 1, m_vec);	/* Base interrupt vector        */
	outportb_p(s_pic + 1, s_vec);

	outportb_p(m_pic + 1, 1 << 2);	/* Bitmask for cascade on IRQ 2 */
	outportb_p(s_pic + 1, 2);	/* Cascade on IRQ 2             */

	outportb_p(m_pic + 1, icw4);	/* Finish 8259 initialization   */
	outportb_p(s_pic + 1, icw4);

	outportb_p(m_imr, 0xff);	/* Mask all interrupts          */
	outportb_p(s_imr, 0xff);
}

/* enables irq irq_no */
void enable_irq(word irq_no)
{
	irq_mask &= ~(1 << irq_no);
	if (irq_no >= 8)
		irq_mask &= ~(1 << 2);

	outportb_p(m_pic + 1, irq_mask & 0xFF);
	outportb_p(s_pic + 1, (irq_mask >> 8) & 0xFF);
}

/* disables irq irq_no */
void disable_irq(word irq_no)
{
	irq_mask |= (1 << irq_no);
	if ((irq_mask & 0xFF00) == 0xFF00)
		irq_mask |= (1 << 2);

	outportb_p(m_pic + 1, irq_mask & 0xFF);
	outportb_p(s_pic + 1, (irq_mask >> 8) & 0xFF);
}


static __inline__ int ffs(int x)
{
	int r;

      __asm__("bsfl %1,%0\n\t" "jnz 1f\n\t" "movl $-1,%0\n" "1:": "=r"(r):"rm"(x));
	return r + 1;
}

byte get_cur_irq(void)
{
	unsigned int irq;
	outportb_p(m_pic, ocw3_isr);
	irq = inportb_p(m_pic);
	if (irq == 0) {
		outportb_p(s_pic, ocw3_isr);
		return ffs(inportb_p(s_pic)) - 1;
	}
	return ffs(irq) - 1;
}
