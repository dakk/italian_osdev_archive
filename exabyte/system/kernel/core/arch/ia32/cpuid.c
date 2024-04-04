/* cpuid.c -- IA-32 CPUID instruction support */
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

#include <arch-ia32/cpuid.h>
#include <arch-ia32/config.h>

int ia32_cpuid_check() {
	unsigned long ret;
	asm("pushfl\n"
		"popl %%eax\n"
		"movl %%eax,%%ecx\n"
		"xorl $0x200000,%%eax\n"
		"pushl %%eax\n"
		"popfl\n"
		"pushfl\n"
		"popl %%eax\n"
		"xorl %%ecx,%%eax\n"
		"je no_cpuid\n"
		"pushl %%ecx\n"
		"popfl\n"
		"movl $1,%%eax\n"
		"jmp end_cpuid\n"
		"no_cpuid:\n"
		"movl $0,%%eax\n"
		"end_cpuid:\n"
		"movl %%eax,%0\n"
		: "=m"(ret)
	);
	if (ret)
		return 0;
	else
		return -1;
}

void ia32_cpuid(unsigned num,unsigned long *eax,unsigned long *ebx,unsigned long *ecx,unsigned long *edx) {
	unsigned long t1,t2,t3,t4;
	asm("movl %4,%%eax\n"
		"cpuid\n"
		"movl %%eax,%0\n"
		"movl %%ebx,%1\n"
		"movl %%ecx,%2\n"
		"movl %%edx,%3\n"
		: "=m"(t1), "=m"(t2), "=m"(t3), "=m"(t4) : "m"(num)
	);
	*eax=t1;
	*ebx=t2;
	*ecx=t3;
	*edx=t4;
}
