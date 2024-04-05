/*
	+---------------------------------------+
	| Filename: io.h			|
	| Authors: Abruzzo Silvio		|
	| License: GNU General Public License	|
	+---------------------------------------+
	| Attenzione: Questo codice fa parte del|
	| sistema operativo ItaliOs.		|
	| Copyright  (c) <2003> Abruzzo Silvio	|
	+---------------------------------------+
*/

#ifndef _HARDWARE_IO_H_
#define _HARDWARE_IO_H_

/*
	**************************
	**	 Include	**
	**************************
*/

#include <kdef.h>

/*
	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////
*/

/*
	Le seguenti funzioni sono inline
*/

inline  byte inportb_p(word port){
	byte ret;

	asm volatile ("inb %%dx,%%al;outb %%al,$0x80":"=a" (ret):"d" (port));

	return ret;
}

inline void outportb_p(word port, byte value){
	asm volatile ("outb %%al,%%dx;outb %%al,$0x80": :"d" (port), "a"(value));
}

inline void outportw_p(word port, word value){
	asm volatile ("outw %%ax,%%dx;outb %%al,$0x80": :"d" (port), "a"(value));
}

inline byte inportb(word port){
	byte ret;

	asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));
	
	return ret;
}

inline void outportb(word port, byte value){
	asm volatile ("outb %%al,%%dx": :"d" (port), "a"(value));
}

inline void outportw(word port, word value){
	    asm volatile ("outw %%ax,%%dx": :"d" (port), "a"(value));
}

inline word inportw(word port){
	word _v;
	
	__asm__ volatile ("inw %%dx,%%ax":"=a" (_v):"d"(port));
	
	return _v;
}

/*
	**************************
	**	  define	**
	**************************
	** Compatibilità alle 	**
	** chiamate linux	**
	**************************
*/

#define outb(value, address) outportb(address, value)
#define inb(address) inportb(address)
#define inw(address) inportw(address)

#endif
