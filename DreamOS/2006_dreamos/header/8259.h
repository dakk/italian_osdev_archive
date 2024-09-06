/*
 * Dreamos
 * 8259.h
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
  * Autore: Ivan Gualandri
  * Prima versione: 4/11/2003
  * Contiene i prototipi di funzione e le #define utili per riconfigurare il processore pic8259
  */

#ifndef __8259_H
#define __8259_H

#include <mydef.h>

#define MASTER_PIC 0x20
#define SLAVE_PIC 0xA0
 
#define MASTER2_PIC 0x21
#define SLAVE2_PIC 0xA1

#define ICW1 0x11      //ICW sono comandi di nizializzazione del PIC
 
#define ICW2_M 0x20
#define ICW2_S 0x28

#define ICW3_M 0x04
#define ICW3_S 0x03

#define ICW4_M 0x01
#define ICW4_S 0x01

#define READ_ISR 0x0B
#define EOI 0x20

void initPIC8259();
void enable_irq(byte, byte);
void disable_irq(byte, byte);
//void exec_irq();

#endif
