/*
 * Dreamos
 * idt.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
 /*
  * Autore Ivan Gualandri
  * Prima versione: 27/10/2003
  * Visto che si cerchera di mantenere piena compatibilita con italios, questo header
  * e' identico a quello di italios
  */


#ifndef INCLUDE_IDT
#define INCLUDE_IDT

#include <mydef.h>

#define IRQ_MASK 0xFFFF

#define MAX_ELEM 256
#define D_PRESENT    0x8000
#define D_DPL0   0
#define D_DPL1   2000
#define D_DPL2   4000
#define D_DPL3   6000
#define D_OPTION    0x470
#define IDT_SIZE    256

struct intdesc {
    word offsetlow;   //low order word dell'indirizzo funzione 
    word segmento;    //indirizzo del segmento
    word riservato :5;
    word opzioni : 11;     //opzioni del descrittore
    word offsethigh;  //high order word dell'indirizzo funzion
    };
typedef struct intdesc interrupt_desc;
//typedef struct int_desc int_desc;

void add(void (*func)(), unsigned char);
void initIDT();
void mint_null();
void load_idt(interrupt_desc*);

void int0();
void int1();
void int2();
void int3();
void int4();
void int5();
void int6();
void int7();
void int8();
void int9();
void int10();
void int11();
void int12();
void int13();
void int14();
void int15();
void int16();
void int17();
void pic_handler();
//void int33();

void disable_interrupts();
void enable_interrupts();

#define cli() disable_interrupts()
#define sti() enable_interrupts()

#endif
