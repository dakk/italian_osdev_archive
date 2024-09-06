/*
 * Dreamos
 * 8253.c
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
  * Thanks to: Silvio Abruzzo, il mio maestro jedi :D
  */

  /*
   * Questo modulo contiene le funzioni di configurazione e utilizzo del PIT
   * (Programmable Interrupt Timer).
   */

   #include <8253.h>
   #include <io.h>
   #include <irq.h>
   #include <idt.h>
   #include <8259.h>
   #include <video.h>
   #include <scheduler.h>

   int counter_0;
   int quants;

   void InitPIT()                //Inizializza il processore PIT
   {   //Per sicurezza vengono disabilitati gli interrupt
       disable_interrupts();
       //Qui va il  codice di configurazione del PIT
       counter_0 = 65535;
       outportb ( PIT_TIMER0_CW, PIT_CONTROL); //Configuro il timer 0
       outportb (counter_0 & 0xFF, PIT_TIMER0);  //Imposto il valore del contatore mando la prima word LSB
       outportb (counter_0 >> 8, PIT_TIMER0);  //Imposto il valore del contatore mando la seconda word MSB
       start_PIT();
       enable_interrupts();
   }

   void start_PIT()
   {
       quants=0;
       enable_irq(0xFC, MASTER2_PIC);         //Abilito interrupt 0
       add_irq(PIT_irq, 0);                              //Aggiungo la funzione che gestisce l'irq
   }

   void PIT_irq()
   {   //Questa funzione si occupa di gestire l'interrupt0 che arriva dal processore pit.
       if(quants<QUANTS) quants++;
       else {
           quants=0;
           //printf("mah");
           preSchedule();
       }
    }

    void maxQuants() {quants = QUANTS;}
