/* arch.h */
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

#ifndef ARCH_ARCH_H
#define ARCH_ARCH_H

#include <config.h>

#ifdef IA32

	#include <arch-ia32/config.h>

	#include <arch-ia32/arch.h>
	#define arch_halt()					ia32_halt()
	#define arch_enable_interrupt()		sti()
	#define arch_disable_interrupt()	cli()

	#include <arch-ia32/mm/mem.h>
	#define arch_get_total_mem()		ia32_get_total_mem()

#else

	#error "you have to choose an architecture"

#endif /* IA32 */

#endif /* ARCH_ARCH_H */
