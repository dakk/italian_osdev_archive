/* apic.h -- APICs support definitions */
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

#ifndef IA32_APIC_H
#define IA32_APIC_H

#include <arch-ia32/config.h>

#ifdef LANG_US
#define APIC_INFO	"APIC: Local APIC ID is 0x%02x, version 0x%02x ("
#define APIC_EXTERNAL	"82489DX external APIC"
#define APIC_P4		"Pentium 4 Local APIC"
#define APIC_LAPIC	"generic Local APIC"
#define APIC_UNKNOWN	"unknown"
#define APIC_LVTS	"), %u LVTs\n"
#define APIC_INITOK	"APIC: Local APIC for CPU%u has been initialized\n"
#define APIC_BOOTING	"APIC: booting CPU%u...\n"
#define APIC_BOOTED	"CPU%u has been successfully booted\n"
#elif defined LANG_IT
#define APIC_INFO	"APIC: Local APIC ID 0x%02x, versione 0x%02x ("
#define APIC_EXTERNAL	"APIC esterno 82489DX"
#define APIC_P4		"Local APIC Pentium 4"
#define APIC_LAPIC	"Local APIC generico"
#define APIC_UNKNOWN	"sconosciuto"
#define APIC_LVTS	"), %u LVT\n"
#define APIC_INITOK	"APIC: Local APIC per la CPU%u inizializzato correttamente\n"
#define APIC_BOOTING	"APIC: avvio la CPU%u...\n"
#define APIC_BOOTED	"CPU%u avviata correttamente\n"
#endif

void ia32_apic_init(unsigned int cpu);
void ia32_apic_boot_ap(unsigned int cpu,unsigned char boot_page);
void ia32_apic_setup_timer(unsigned int cpu,unsigned long ticks);
unsigned long ia32_apic_get_timer(unsigned int cpu);
void ia32_apic_eoi(unsigned int cpu);

#endif	/* IA32_APIC_H */
