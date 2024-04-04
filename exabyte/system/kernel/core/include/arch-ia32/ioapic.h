/* ioapic.h -- I/O APIC definitions */
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

#ifndef IA32_IOAPIC_H
#define IA32_IOAPIC_H

#include <arch-ia32/config.h>

#define IOAPIC_OK	"ok\n"

#ifdef LANG_US
#define IOAPIC_LINIT	"IOAPIC%u: initializing... "
#elif defined LANG_IT
#define IOAPIC_LINIT	"IOAPIC%u: inizializzazione... "
#endif

#define IOAPIC_MAXIRQ			23
#define IOAPIC_NUMIRQ			IOAPIC_MAXIRQ+1

int ia32_ioapic_init(int ioapic);
int ia32_ioapic_enable_irq(int ioapic,unsigned int irqn,unsigned char vector,int cpu_dest);
int ia32_ioapic_disable_irq(int ioapic,unsigned int irqn);
int ia32_ioapic_disable_all_irqs(int ioapic);
int ia32_ioapic_enable_all_irqs(int ioapic,unsigned char *vectors,int *cpu_dests);
int ia32_ioapic_eoi(int ioapic,unsigned int irqn);

#endif	/* IA32_IOAPIC_H */
