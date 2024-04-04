/* exception.h -- ia32 exceptions definitions */
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

#ifndef IA32_EXCEPTION_H
#define IA32_EXCEPTION_H

#include <arch-ia32/config.h>
#include <arch-ia32/descriptor.h>

#ifdef CONFIG_ARCH_FASTASM
#define ia32_exc_set_handler(exc,sel,addr,flags,rpl) set_trap_gate(exc,sel,addr)
#else
int ia32_exc_set_handler(unsigned int exc,unsigned short selector,void * handler,unsigned int flags,unsigned char rpl);
#endif
int ia32_exc_init();

#endif /* IA32_EXCEPTION_H */
