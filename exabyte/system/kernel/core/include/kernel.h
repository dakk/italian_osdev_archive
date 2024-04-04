/* kernel.h -- Kernel global definitions */
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

#ifndef KERNEL_H
#define KERNEL_H

/* la lunghezza dello stack (16KB). */
#define STACK_SIZE                      0x4000

/* formato dei simboli C. HAVE_ASM_USCORE è definito da configure */
#ifdef HAVE_ASM_USCORE
 #define EXT_C(sym)                     _ ## sym
#else
 #define EXT_C(sym)                     sym
#endif

#ifndef ASM

/* Linker Script variables */
extern const char g_start[];	// virtual kernel start address
extern const char g_end[];	// virtual kernel end address
extern const char load_adr[];	// kernel load address
extern const char virt_adr[];	// kernel virtual address

/* anteporre questo ad una funzione per farsi passare i parametri sullo stack */
#define asmlinkage	__attribute__((regparm(0)))

int kprint(char * format,...);
void print_klog(void);

#endif

#endif
