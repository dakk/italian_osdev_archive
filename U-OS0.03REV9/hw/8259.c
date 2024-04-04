#include <bitops.h>
#include <video.h>
#include <io.h>
#include <8259.h>
#include <irq.h>
  
void initPIC8259(void)
{

    int irq_mask;

    outportb(ICW1, MASTER_PIC);
    outportb(ICW1, SLAVE_PIC);
    
    outportb(ICW2_M, MASTER2_PIC);
    outportb(ICW2_S, SLAVE2_PIC);
    
    outportb(ICW3_M, MASTER2_PIC);
    outportb(ICW3_S, SLAVE2_PIC);
    
    outportb(ICW4_M, MASTER2_PIC);
    outportb(ICW4_S, SLAVE2_PIC);
    

    irq_mask = 0xFFFF;
    outportb(irq_mask & 0xFF, MASTER2_PIC);
    outportb((irq_mask >> 8) & 0xFF, SLAVE2_PIC);
}

void enable_irq(byte irq_num, byte port_number)
{

    byte cur_mask;
    cur_mask = inportb(port_number);
    cur_mask = irq_num & cur_mask;
    outportb(irq_num, port_number);
}

void disable_irq(byte irq_num, byte port_number)
{
    byte cur_mask;
    cur_mask = inportb(port_number);
    cur_mask = irq_num | cur_mask;
}

