#include <mydef.h>
#include <idt.h>
#include <irq.h>
#include <video.h>


interrupt_desc idtdesc[IDT_SIZE];

void initIDT(void)
{

    int i=0;     
    int j=0;
    for(i=17; i<MAX_ELEM; i++)
        add(mint_null, i);

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
    add(int16, 17);
    for(j=32; j<48; j++)
        add(pic_handler, j);
  
    load_idt(idtdesc);
}

void add(void (*gestore)(), unsigned char interrupt)
{
    dword indirizzo;
    indirizzo=(dword)gestore;
    idtdesc[interrupt].offsetlow = (indirizzo & 0xFFFF);
    idtdesc[interrupt].offsethigh = indirizzo >> 16;
    idtdesc[interrupt].segmento = 0x8;
    idtdesc[interrupt].opzioni = 0x470;
}

void load_idt(interrupt_desc* indirizzo)
{
    dword idt_idtr[2];
    idt_idtr[0] = (IDT_SIZE * 8) << 16;
    idt_idtr[1] = (dword) indirizzo;
    __asm__ __volatile__ ("lidt (%0)" : : "g"  ((char *)idt_idtr+2));   
}

void panic(void)
{
    _kputs("\033\004\nKERNEL PANIC:\n");

}

  
void int0()
{
    panic();
    _kputs("\033\007 -- Int00 -- Divisione per zero\n");
   while(1);
}

void int1()
{
    panic();
    _kputs("\033\007 -- Int01 -- Exception of Debug\n");
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
    _kputs("\033\007 -- Int06 -- Not Valid Opcode\n");
    while(1);
}

void int7()
{
    panic();
    _kputs("\033\007 -- Int07 -- Device not available\n");
    while(1);
}

void int8()
{
    panic();
    _kputs("\033\007 -- Int08 -- Double Failure\n");
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
    _kputs("\033\007 -- Int10 -- State of not valid segment\n");
    while(1);
}

void int11()
{
    panic();
    _kputs("\033\007 -- Int11 -- Not present segment\n");
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
   panic();
   _kputs("\033\007 -- Int14 -- Page Fault\n");
   while(1);
}

void int15()
{
   panic();
   _kputs("\033\007 -- Int15 -- Disowned Error\n");
   while(1);
}

void int16()
{
   panic();
   _kputs("\033\007 -- Int16 -- Error of Mobile Virgola\n");
   while(1);
}

void int17()
{
   panic();
   _kputs("\033\007 -- Int17 -- Memory not aligned\n");
   while(1);
}

void pic_handler()
{
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
    _kputs("\033\007 -- Int :D -- :)\n");
    while(1);
}

void disable_interrupts()
{
    asm("cli");
}

void enable_interrupts()
{
    asm("sti");
}
