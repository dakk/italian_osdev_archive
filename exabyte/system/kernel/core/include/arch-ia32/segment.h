/* segment.h -- Kernel segments definitions */
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

#ifndef IA32_SEGMENT_H
#define IA32_SEGMENT_H

/* Selector offsets */
#define KERNEL_CS	0x8
#define KERNEL_DS	0x10
#define USER_CS		0x18
#define USER_DS		0x20
#define KERNEL_TSS	0x28

/* Segment Selectors */
#define KERNEL_CS_SEL	(KERNEL_CS & 0xFFF8)
#define KERNEL_DS_SEL	(KERNEL_DS & 0xFFF8)
#define USER_CS_SEL	((USER_CS & 0xFFF8)|3)
#define USER_DS_SEL	((USER_DS & 0xFFF8)|3)
#define KERNEL_TSS_SEL	(KERNEL_TSS & 0xFFF8)

#endif	/* IA32_SEGMENT_H */
