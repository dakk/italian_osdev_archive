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
    word offsetlow;
    word segmento;
    word riservato :5;
    word opzioni : 11;
    word offsethigh;
    };
typedef struct intdesc interrupt_desc;

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
