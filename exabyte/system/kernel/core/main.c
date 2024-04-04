/* main.c */
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

#include <config.h>
#include <kernel.h>
#include <arch/arch.h>
#include <mm/physical.h>

void kernel_main() {
	kprint("--- kernel startup ---\n");
	//mm_physical_init();
	/* TEMP */ unsigned long mem=(unsigned long)(arch_get_total_mem()&0xFFFFFFFF)/1024/1024; kprint("mm: found %u mb of memory\n",mem);
	arch_enable_interrupt();
	while (1);
}
