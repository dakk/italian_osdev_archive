/* init.c -- funzioni di avvio principali del kernel */
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

#include <config.h>		// questo va SEMPRE per primo
#include <arch-ia32/multiboot.h>
#include <arch-ia32/vga_text.h>
#include <arch-ia32/descriptor.h>
#include <arch-ia32/exception.h>
#include <arch-ia32/irq.h>
#include <arch-ia32/apic.h>
#include <arch-ia32/smp.h>
#include <arch-ia32/arch.h>
#include <arch-ia32/cpuid.h>
#include <arch-ia32/timer.h>
#include <arch-ia32/cmos.h>
#include <arch-ia32/time.h>
#include <arch-ia32/string.h>
#include <kernel.h>	// per le vars del linker script e per l'asmlinkage
#include <panic.h>

multiboot_info boot_info;

asmlinkage int arch_init(unsigned long boot_magic,multiboot_info * mb_info)
{
	if(videotext_init(80,25,0) < 0) for(;;);	// :P
	kprint("--- architecture startup ---\n");
	memcpy(&boot_info,mb_info,sizeof(multiboot_info));
	/* inizializza gdt, idt+exceptions */
	kprint(INIT_PMTABLES);
	ia32_gdt_init();
	ia32_idt_init();
	ia32_exc_init();
	kprint(INIT_OK);
	if(ia32_cpuid_check() < 0) panic(INIT_CPUIDERR);
	ia32_smp_init();
	ia32_timer_init();
	ia32_cmos_init();
	ia32_time_init();
	return boot_magic;
}
