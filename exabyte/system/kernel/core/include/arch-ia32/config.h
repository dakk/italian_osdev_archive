/* config.h -- IA32 architecture configuration header */
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

#ifndef ARCH_CONFIG_H
#define ARCH_CONFIG_H

#include <config.h>

/** FILE DI CONFIGURAZIONE PER ARCHITETTURA ia32
 Da questa riga in poi e` tutto modificabile dall'utente per scegliere le
 caratteristiche da abilitare per l'architettura ia32. Commenta le righe
 CONFIG_* per disabilitarne la caratteristica, scommentale per abilitarla
 (anteponi due slash // alla riga che vuoi disabilitare per commentarla) **/

/* Configurazione mm */
//#define CONFIG_ARCH_PAE	// Abilita l'uso del Physical Address Extension

/* Configurazione interruzioni e descrittori */
//#define CONFIG_ARCH_FASTASM	// Abilita l'uso di alcune funzioni asm inline più veloci invece delle corrispondenti funzioni C più sicure

#define CONFIG_ARCH_SSE		// Abilita l'uso delle istruzioni SSE

#define CONFIG_ARCH_MAX_PROCESSORS	8	// Numero massimo di processori supportati

#define CONFIG_ARCH_MAX_IOAPICS		4	// Numero massimo di I/O APIC supportati

#endif	/* ARCH_CONFIG_H */
