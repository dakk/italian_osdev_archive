/*ItaliOs

 Copyright  (c) <2003> Abruzzo Silvio

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
7
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef  _IO_H_
#define _IO_H_

#include <stddef.h>

	inline  byte inportb_p(word port) {
	    byte ret;
	    asm volatile ("inb %%dx,%%al;outb %%al,$0x80":"=a" (ret):"d"
			  (port));
	     return ret;
	}

	inline void outportb_p(word port, byte value) {
	    asm volatile ("outb %%al,%%dx;outb %%al,$0x80": :"d" (port),
			  "a"(value));
	}

	inline void outportw_p(word port, word value) {
	    asm volatile ("outw %%ax,%%dx;outb %%al,$0x80": :"d" (port),
			  "a"(value));
	}

	inline byte inportb(word port) {
	    byte ret;
	    asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));
	    return ret;
	}

	inline void outportb(word port, byte value) {
	    asm volatile ("outb %%al,%%dx": :"d" (port), "a"(value));
	}

	inline void outportw(word port, word value) {
	    asm volatile ("outw %%ax,%%dx": :"d" (port), "a"(value));
	}

	inline word inportw(word port) {
	    word _v;
	    __asm__ volatile ("inw %%dx,%%ax":"=a" (_v):"d"(port));
	    return _v;
	}


/*compatibilità alle chiamate di linux*/
#define outb(value, address) outportb(address, value)
#define inb(address) inportb(address)
#define inw(address) inportw(address)

#endif
