/* io.h -- ia32 Ports input/output functions */
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

#ifndef ARCH_IO_H
#define ARCH_IO_H

/* Single-byte */
#define outb(value,port) \
__asm__ ("outb %%al,%%dx"::"a" (value),"d" (port))

#define inb(port) ({ \
    unsigned char _v; \
    __asm__ volatile ("inb %%dx,%%al":"=a" (_v):"d" (port)); \
    _v; \
    })

#define outb_p(value,port) \
__asm__ ("outb %%al,%%dx\n" \
         "\tjmp 1f\n" \
         "1:\tjmp 1f\n" \
         "1:"::"a" (value),"d" (port))

#define inb_p(port) ({ \
unsigned char _v; \
__asm__ volatile ("inb %%dx,%%al\n" \
                  "\tjmp 1f\n" \
                  "1:\tjmp 1f\n" \
                  "1:":"=a" (_v):"d" (port)); \
                  _v; \
				  })

/* Word */
#define outw(value,port) \
__asm__ ("outw %%ax,%%dx"::"a" (value),"d" (port))

#define inw(port) ({ \
    unsigned short _v; \
    __asm__ volatile ("inw %%dx,%%ax":"=a" (_v):"d" (port)); \
    _v; \
    })

#define outw_p(value,port) \
__asm__ ("outw %%ax,%%dx\n" \
         "\tjmp 1f\n" \
         "1:\tjmp 1f\n" \
         "1:"::"a" (value),"d" (port))

#define inw_p(port) ({ \
unsigned short _v; \
__asm__ volatile ("inw %%dx,%%ax\n" \
                  "\tjmp 1f\n" \
                  "1:\tjmp 1f\n" \
                  "1:":"=a" (_v):"d" (port)); \
                  _v; \
				  })

/* Double-Word */
#define outl(value,port) \
__asm__ ("outl %%eax,%%dx"::"a" (value),"d" (port))

#define inl(port) ({ \
    unsigned long _v; \
    __asm__ volatile ("inl %%dx,%%eax":"=a" (_v):"d" (port)); \
    _v; \
    })

#define outl_p(value,port) \
__asm__ ("outl %%eax,%%dx\n" \
         "\tjmp 1f\n" \
         "1:\tjmp 1f\n" \
         "1:"::"a" (value),"d" (port))

#define inl_p(port) ({ \
unsigned long _v; \
__asm__ volatile ("inl %%dx,%%eax\n" \
                  "\tjmp 1f\n" \
                  "1:\tjmp 1f\n" \
                  "1:":"=a" (_v):"d" (port)); \
                  _v; \
				  })

/* varianti delle funzione sopra */

/* -- Input -- */

/* Single-byte */
#define inportb(port)		inb(port)
#define inportb_p(port)		inb_p(port)
/* Word */
#define inportw(port)		inw(port)
#define inportw_p(port)		inw_p(port)
/* Double-Word */
#define inportl(port)		inl(port)
#define inportl_p(port)		inl_p(port)

/* -- Output -- */

/* Single-byte */
#define outportb(port,value)	outb(value,port)
#define outportb_p(port,value)	outb_p(value,port)
/* Word */
#define outportw(port,value)	outw(value,port)
#define outportw_p(port,value)	outw_p(value,port)
/* Double-Word */
#define outportl(port,value)	outl(value,port)
#define outportl_p(port,value)	outl_p(value,port)

#endif
