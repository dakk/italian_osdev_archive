
#include	"irq.h"


unsigned int irq_mask;

//Rimappa gli IRQ sugli interrupt a partire da interrupt_irq_start
void irq_pic_init(unsigned char interrupt_irq_start)
	{
	//ICW0 a PIC1 e PIC2
	outportb(0x20,0x11);
	outportb(0xA0,0x11);

	//Rimappa gli IRQ il secondo PIC e' subito dopo il secondo!
	outportb(0x21,interrupt_irq_start);
	outportb(0xA1,interrupt_irq_start+8);

	//Altre impostazioni degli
	outportb(0x21,0x04);
	outportb(0xA1,0x02);

	//Mada ICW4 a PIC1 e PIC2
	outportb(0x21,0x01);
	outportb(0xA1,0x01);

	// Irqs disabilitati dal'inizio!!
	irq_mask=0xFFFF;
	}

void irq_enable(unsigned short irq)
	{
	irq_mask &= ~(1 << irq);
	if(irq>9)
		irq_mask &= ~(1 << 2);

	outportb(0x21,irq_mask & 0xFF);
	outportb(0xA1,(irq_mask >> 8) & 0xFF);
	}

void irq_disable(unsigned short irq)
	{
	irq_mask|=(1<<irq);
	if((irq_mask & 0xFF00)==0xFF00)
		irq_mask|=(1<<2);

	outportb(0x21,irq_mask & 0xFF);
	outportb(0xA1,(irq_mask>>8)&0xFF);
	}

inline void irq_end()
	{
	outportb(0x20,0x20);
	}

