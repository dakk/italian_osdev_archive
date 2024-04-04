/* mem.h -- IA32 Miscellaneous memory managent definitions */
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

/** questo file deve essere architecture-standard!!!!! * <-- no, solo compatibile ia32, se no non si troverebbe in arch-ia32... è mirrorato in arch/arch.h*/

#ifndef IA32_MEM_H
#define IA32_MEM_H

unsigned long long ia32_get_total_mem(void);

#endif	/* IA32_MEM_H */
