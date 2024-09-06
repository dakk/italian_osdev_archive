/*
 * Dreamos
 * irq.c
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
 /*
  * Autore Ivan Gualandri
  * Prima versione: 15/11/2003
  * Gestisce gli IRQ
  */

#include <mydef.h>
#include <bitops.h>
#include <video.h>
#include <irq.h>
#include <8259.h>  
#include <io.h>
#include <scheduler.h>

void init_irq()
{
//Initialize irq vector
}

void add_irq(void (*func)(), int position)
{
//add an isr to irq vector - position is the sama as irq number
    irq_selector[position] = func;
}

void do_irq(dword *esp)
{
//Questa funzione si ricerca l'irq da servire
    byte current_irq;
    int cur;
    cur=0;
    outportb(READ_ISR, MASTER_PIC);      //Comando di lettura ISR
    current_irq = inportb(MASTER_PIC);   //Lettura ISR
    if (current_irq==0)
    {
        outportb(READ_ISR, SLAVE_PIC);
        current_irq = inportb(SLAVE_PIC);
        cur=8;
    }
    cur = cur + find_first_bit(current_irq);

    irq_selector[cur]();
    if (cur >= 8)
        outportb(EOI, SLAVE_PIC);
    outportb(EOI, MASTER_PIC);

   schedule(esp);
   return;
}

void exec_irq(dword esp)
{
    do_irq(&esp);
    return;
}
