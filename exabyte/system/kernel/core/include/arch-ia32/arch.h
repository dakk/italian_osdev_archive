/* arch.h -- ia32 architecture global definitions */
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

#ifndef IA32_ARCH_H
#define IA32_ARCH_H

#include <arch-ia32/config.h>

#define INIT_OK		"ok\n"

/* lingua predefinita: nessuna */
#ifdef LANG_US
#define INIT_PMTABLES	"init: setting up system tables..."
#define INIT_CPUIDERR	"init: CPUID instruction not supported\n"
#define SETUP_TODO	"setup-ia32: nothing to do\n"
#elif defined LANG_IT
#define INIT_PMTABLES	"init: creazione tabelle di sistema..."
#define INIT_CPUIDERR	"init: istruzione CPUID non supportata\n"
#define SETUP_TODO	"setup-ia32: niente da fare\n"
#endif

#define cli() __asm__("cli");
#define sti() __asm__("sti");
#define nop() __asm__("nop");
#define hlt() __asm__("hlt");
#define ret() __asm__("ret");
#define iret() __asm__("iret");

#define save_flags(x) \
    __asm__ __volatile__("pushfl ; popl %0":"=r" (x): /* no input */ :"memory")

#define restore_flags(x) \
    __asm__ __volatile__("pushl %0 ; popfl": /* no output */ :"r" (x):"memory")

#define read_cr0() ({ \
        unsigned int __dummy; \
        __asm__( \
                "movl %%cr0,%0\n\t" \
                :"=r" (__dummy)); \
        __dummy; \
})
#define write_cr0(x) __asm__("movl %0,%%cr0": :"r" (x));

#define ia32_halt() \
	__asm__ __volatile__("cli; hlt;");

#endif	/* IA32_ARCH_H */
