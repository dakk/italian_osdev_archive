/*
 * Dreamos
 * pit.h
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
  * Thanks to: Silvio Abruzzo il mio maestro jedi :D
  */

  /*
   * Questo modulo contiene gli header per le funzioni di configurazione e utilizzo del PIT
   * (Programmable Interrupt Timer).
   */
   #ifndef  __PIT_H_
   #define __PIT_H_

   #define PIT_FREQ 1193181
   #define PIT_TIMER0 0x40
   #define PIT_TIMER1 0x41
   #define PIT_TIMER2 0x42
   #define PIT_CONTROL 0x43
   #define PIT_TIMER0_CW 0x37
   #define QUANTS 5

   extern int counter_0;    //contiene il valore del contatore del timer 0

   void InitPIT();               //Inizializza il processore PIT
   void PIT_irq();             //Irq che gestisce il pit
   void start_PIT();           //Fai partire il timer
   void maxQuants();

   #endif

