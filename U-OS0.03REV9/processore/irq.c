#include <mydef.h>
#include <bitops.h>
#include <video.h>
#include <irq.h>
#include <8259.h>  
#include <io.h>
#include <scheduler.h>

void init_irq()
{
}

void add_irq(void (*func)(), int position)
{
    irq_selector[position] = func;
}

void do_irq(dword *esp)
{
    byte current_irq;
    int cur;
    cur=0;
    outportb(READ_ISR, MASTER_PIC);
    current_irq = inportb(MASTER_PIC);
    if (current_irq==0)
    {
        outportb(READ_ISR, SLAVE_PIC);
        current_irq = inportb(SLAVE_PIC);
        cur=8;
    }
    cur = cur + find_first_bit(current_irq);

    irq_selector[cur]();
    if (cur >= 8)
        outportb(EOI, SLAVE_PIC);
    outportb(EOI, MASTER_PIC);

   schedule(esp);
   return;
}

void exec_irq(dword esp)
{
    do_irq(&esp);
    return;
}
