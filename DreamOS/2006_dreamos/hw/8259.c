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
  * Autore Ivan Gualandri
  * Prima versione: 27/10/2003
  */


#include <bitops.h>
#include <video.h>
#include <io.h>
#include <8259.h>
#include <irq.h>
  
void initPIC8259(void)
{
//Inizializza il processore per la gestione degl interrup PIC8259
    int irq_mask;
    //Spedisco al  PIC i comandi ICW per inizializzarlo
    outportb(ICW1, MASTER_PIC);
    outportb(ICW1, SLAVE_PIC);
    
    outportb(ICW2_M, MASTER2_PIC);
    outportb(ICW2_S, SLAVE2_PIC);
    
    outportb(ICW3_M, MASTER2_PIC);
    outportb(ICW3_S, SLAVE2_PIC);
    
    outportb(ICW4_M, MASTER2_PIC);
    outportb(ICW4_S, SLAVE2_PIC);
    
//Maschero tutti gli interrupt (1 li maschero e 0 li smaschero)
    irq_mask = 0xFFFF;
    outportb(irq_mask & 0xFF, MASTER2_PIC);
    outportb((irq_mask >> 8) & 0xFF, SLAVE2_PIC);
}

void enable_irq(byte irq_num, byte port_number)
{
//Abilita un interrupt - irq_num e' la nuova maschera
//L'irq da attivare va messo a 0 quindi se voglio attivare ad esempio il 3 irq faccio 11111011
    byte cur_mask;
    cur_mask = inportb(port_number); //Leggi la mask dal processore PIC
    cur_mask = irq_num & cur_mask;   // Metti ina AND, la nuova mask con quella vecchia
    outportb(cur_mask, port_number);  // Trasmetti la nuoa mask al PIC
}

void disable_irq(byte irq_num, byte port_number)
{
//Disabilita un particolare interrupt (da finire)
    byte cur_mask;
    cur_mask = inportb(port_number);
    cur_mask = irq_num | cur_mask;
}

