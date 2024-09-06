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
  * Thanks to: Silvio Abruzzo ^_^ che come sempre senza di lui nn potevo fare nemmeno questa :)
  */

#include <mydef.h>
#include <idt.h>
#include <irq.h>
#include <video.h>
#include <syscall.h>


interrupt_desc idtdesc[IDT_SIZE];

void initIDT(void)
{
//Inizializza la Interrupt Descriptor Table
    int i=0;     
    int j=0;
    for(i=17; i<MAX_ELEM; i++)
        //riempie il vettore di null_int
        add(mint_null, i);

    //Inizializzare i 16 descrittore dei primi 16 interrupt piu quelli del pic
    add(int0, 0);
    add(int1, 1);
    add(int2, 2);
    add(int3, 3);
    add(int4, 4);
    add(int5, 5);
    add(int6, 6);
    add(int7, 7);
    add(int8, 8);
    add(int9, 9);
    add(int10, 10);
    add(int11, 11);
    add(int12, 12);
    add(int13, 13);
    add(int14, 14);
    add(int15, 15);
    add(int16, 16);
    add(int17, 17);
    for(j=32; j<48; j++)
        add(pic_handler, j);
		init_syscall();
    load_idt(idtdesc);
}

void add(void (*gestore)(), unsigned char interrupt)
{
    //Questa funzione si occupa di preparare il descrittore dell'interrupt.
    dword indirizzo;
    //Divido l'indirizzo della funzione in due blocchi di 16bit
    indirizzo=(dword)gestore;
    idtdesc[interrupt].offsetlow = (indirizzo & 0xFFFF);    //Low order word - bit 0-16 prima word del descrittore
    idtdesc[interrupt].offsethigh = indirizzo >> 16;    //High order word - bit 16-31 seconda word del descittore
    idtdesc[interrupt].segmento = 0x8;    //indirizzo del segmento
    idtdesc[interrupt].opzioni = 0x470;     //opzioni del descrittore  
}

void load_idt(interrupt_desc* indirizzo)
{
    dword idt_idtr[2];    //Doppia dword che conterra il dato da mettere in idtr
    
    idt_idtr[0] = (IDT_SIZE * 8) << 16;
    idt_idtr[1] = (dword) indirizzo;
    __asm__ __volatile__ ("lidt (%0)" : : "g"  ((char *)idt_idtr+2));   
}

//Panic Stampa un messaggio Kernel Panic
void panic(void)
{
    _kputs("\033\004\nKERNEL PANIC:\n");

}

  
//Interrupts da 0 a 16 Inizializzo eccezioni del processore
//Sono semplici funzioni che stampano un messaggio
void int0()
{
    panic();
    _kputs("\033\007 -- Int00 -- Divisione per zero\n");
   while(1);
}

void int1()
{
    panic();
    _kputs("\033\007 -- Int01 -- Eccezione di Debug\n");
    while(1);
}

void int2()
{
    panic();
    _kputs("\033\007 -- Int02 -- Not Maskable Interrupt (NMI)\n");
    while(1);
}

void int3()
{
    panic();
    _kputs("\033\007 -- Int03 -- BreakPoint\n");
    while(1);
}

void int4()
{
    panic();
    _kputs("\033\007 -- Int04 -- Overflow\n");
    while(1);
}

void int5()
{
    panic();
    _kputs("\033\007 -- Int05 -- Bound Check\n");
    while(1);
}

void int6()
{
    panic();
    _kputs("\033\007 -- Int06 -- Opcode Non Valido\n");
    while(1);
}

void int7()
{
    panic();
    _kputs("\033\007 -- Int07 -- Dispositivo non disponibile\n");
    while(1);
}

void int8()
{
    panic();
    _kputs("\033\007 -- Int08 -- Doppio Fallimento\n");
    while(1);
}

void int9()
{
    panic();
    _kputs("\033\007 -- Int09 -- Coprocessor Segment Overrun\n");
    while(1);
}

void int10()
{
    panic();
    _kputs("\033\007 -- Int10 -- Stato del segmento non valido\n");
    while(1);
}

void int11()
{
    panic();
    _kputs("\033\007 -- Int11 -- Segmento non presente\n");
    while(1);
}

void int12()
{
    panic();
    _kputs("\033\007 -- Int12 -- Stack Fault\n");
    while(1);
}

void int13()
{
   panic();
   _kputs("\033\007 -- Int13 -- General Protection\n");
   while(1);
}

void int14()
{
//Andra' modificata, con l'aggiunta del supporto della paginazione.
   panic();
   _kputs("\033\007 -- Int14 -- Page Fault\n");
   while(1);
}

void int15()
{
   panic();
   _kputs("\033\007 -- Int15 -- Errore Sconosciuto\n");
   while(1);
}

void int16()
{
   panic();
   _kputs("\033\007 -- Int16 -- Errore di Virgola Mobile\n");
   while(1);
}

void int17()
{
   panic();
   _kputs("\033\007 -- Int17 -- Memoria non allineata\n");
   while(1);
}

void pic_handler()
{
/*
 * Grazie a Silvio Abruzzo che ci ha fornito questa funzione
 * Questa funzione si occupa di salvare il contesto dei registri prima di eseguire
 * il nuovo interrupt, poi chiama il gestore che selezionera' la funzione appropriata.
 */
     asm(".globl pic_handler  \n"
         "pic_handler:     \n"
         "cli  /*disabilito gli interrupt*/ \n"
         "pusha /*salvo tutti i registri*/ \n"
         "movl %esp, %eax  /*salvo esp*/ \n"
         "pushl %eax /*pusho il valore dello stack*/\n"
         "call exec_irq  /*eseguo l'handler vero e proprio*/ \n"
         "popl %eax /*mi riprendo lo stack*/ \n"
         "mov %eax, %esp /*reimposto lo stack*/ \n"
         "popa /*ripristino tutti i registri*/ \n"
         "sti  /*riabilito gli interrupt*/ \n"
         "iret /*esco dall'interrupt     */   \n"
    );
}

void mint_null()
{   
    _kputs("\033\007 -- Int:D -- Ah veditela te io nn so cosa e' arrivato :)\n");
    while(1);
  //asm("iret");  
}

//Funzioni per abilitare e disabilitare gli interrupt
void disable_interrupts()
{
    asm("cli");
}

void enable_interrupts()
{
    asm("sti");
}
