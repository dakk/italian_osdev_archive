/* stddef.h -- standard datatype definitions */
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

#ifndef STDDEF_H
#define STDDEF_H

typedef unsigned int size_t;

typedef unsigned char byte;	  // 1 byte (8 bit)
typedef unsigned short int word;  // 2 byte (16 bit)
typedef unsigned int dword;	  // 4 byte (32 bit)
typedef unsigned long long qword; // 8 byte (64 bit)

#ifndef NULL
//#define NULL	((void *)0)	m'hanno detto che è sbagliato...
#define NULL	0
#endif /* NULL */

#endif
