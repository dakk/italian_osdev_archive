/* irq.h -- ia32 architecture irq definitions */
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

#ifndef IA32_IRQ_H
#define IA32_IRQ_H

#ifdef LANG_US
#define IRQ_INIT	"irq: initialising ia32 IRQ management... "
#define IRQ_OK		"ok, %u IRQs\n"
#define IRQ_INVALID	"irq: invalid IRQ %u\n"
#elif defined LANG_IT
#define IRQ_INIT	"irq: inizializzazione gestione IRQ per ia32... "
#define IRQ_OK		"ok, %u IRQ\n"
#define IRQ_INVALID	"irq: IRQ %u non valido\n"
#endif

#define IRQ_MAXNUM		32
#define IRQ_MAXHANDLERS		8

int ia32_irq_init(unsigned char *vectors);
int ia32_irq_set_handler(unsigned int irqn,void *handler);
int ia32_irq_unset_handler(unsigned int irqn,void *handler);
int ia32_irq_enable(unsigned int irqn);
int ia32_irq_disable(unsigned int irqn);
void ia32_irq_eoi(unsigned int irqn,int ioapic);
void ia32_irq_handler(unsigned int irqn,int ioapic);

#endif	/* IA32_IRQ_H */
