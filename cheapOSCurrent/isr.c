
#include	"isr.h"


extern void asm_ISRnull();

ISR_DECLARE(exception00);
ISR_DECLARE(exception01);
ISR_DECLARE(exception02);
ISR_DECLARE(exception03);
ISR_DECLARE(exception04);
ISR_DECLARE(exception05);
ISR_DECLARE(exception06);
ISR_DECLARE(exception07);
ISR_DECLARE(exception08);
ISR_DECLARE(exception09);
ISR_DECLARE(exception10);
ISR_DECLARE(exception11);
ISR_DECLARE(exception12);
ISR_DECLARE(exception13);
ISR_DECLARE(exception14);
ISR_DECLARE(exception15);
ISR_DECLARE(exception16);


desc_table(IDT, 256)
	{
	};

struct
	{
	unsigned short		limit		__attribute__ ((packed));
	union DT_entry *	idt		__attribute__ ((packed));
	} loadidt= { (256 * sizeof(union DT_entry)-1), IDT };


//Registra IDT e la carica con cpu_ISRnull
void isr_idt_init()
	{
	unsigned char i;

	//Inizzializza TSS per i cambi di privilegio!!
	task_tss_init();
	
	//ISR nulli
	for(i=0;i<255;i++)//Perche' 255?
		isr_setISR(i,asm_ISRnull);

	//ISR essenziali
	isr_setISR(0,ISR_ADDRESS(exception00));
	isr_setISR(1,ISR_ADDRESS(exception01));
	isr_setISR(2,ISR_ADDRESS(exception02));
	isr_setISR(3,ISR_ADDRESS(exception03));
	isr_setISR(4,ISR_ADDRESS(exception04));
	isr_setISR(5,ISR_ADDRESS(exception05));
	isr_setISR(6,ISR_ADDRESS(exception06));
	isr_setISR(7,ISR_ADDRESS(exception07));
	isr_setISR(8,ISR_ADDRESS(exception08));
	isr_setISR(9,ISR_ADDRESS(exception09));
	isr_setISR(10,ISR_ADDRESS(exception10));
	isr_setISR(11,ISR_ADDRESS(exception11));
	isr_setISR(12,ISR_ADDRESS(exception12));
	isr_setISR(13,ISR_ADDRESS(exception13));
	isr_setISR(14,ISR_ADDRESS(exception14));
	isr_setISR(15,ISR_ADDRESS(exception15));
	isr_setISR(16,ISR_ADDRESS(exception16));

	asm (
		"lidt (%0)                 \n"   /* Load the IDT                */
        	"pushfl                    \n"   /* Clear the NT flag           */
	        "andl $0xffffbfff,(%%esp)  \n"
	        "popfl                     \n"
	        :
	        : "r" ((char *) &loadidt)
		);

	return;
	}


void	isr_setISR(unsigned char interrupt,void *handler)	//Setta un ISR
	{
	unsigned short codesegment = 0x08;

	asm volatile("movw %%cs,%0":"=g" (codesegment));

	desc_setgate(&(IDT[interrupt]),codesegment,(unsigned long)handler,D_PRESENT + D_INT);

	return;
	}


//	ISR	che non fa' niente
asm (
	".globl asm_ISRnull	\n"
	"asm_ISRnull:		\n"
	"iret				\n"
	);


//	ISR delle eccezzioni
ISR_CODE_H(exception00)
	{
	kprint("Exception! Divide Error");
	 while(1);
	 }

ISR_CODE_H(exception01)
	{
	kprint("Exception! Debug Exceptions");
	 while(1);
	 }

ISR_CODE_H(exception02)
	{
	kprint("Excpetion! Intel reserved");
	 while(1);
	 }

ISR_CODE_H(exception03)
	{
	kprint("Exception! Breakpoint");
	 while(1);
	 }

ISR_CODE_H(exception04)
	{
	kprint("Exception! Overflow");
	 while(1);
	 }

ISR_CODE_H(exception05)
	{
	kprint("Exception! Bounds Check");
	 while(1);
	 }

ISR_CODE_H(exception06)
	{
	kprint("Exception! Invalid Opcode");
	 while(1);
	 }
ISR_CODE_H(exception07)
	{
	kprint("Exception!  Coprocessor Not Available");
	 while(1);
	 }

ISR_CODE_H(exception08)
	{
	kprint("Exception! Double Fault");
	 while(1);
	 }

ISR_CODE_H(exception09)
	{
	kprint("Excpetion! Coprocessor Segment Overrun");
	 while(1);
	 }

ISR_CODE_H(exception10)
	{
	kprint("Exception! Invalid TSS");
	 while(1);
	 }

ISR_CODE_H(exception11)
	{
	kprint("Exception! Segment Not Present");
	 while(1);
	 }

ISR_CODE_H(exception12)
	{
	kprint("Exception!  Stack Exception");
	 while(1);
	 }

ISR_CODE_H(exception13)
	{
	kprint("Exception! General Protection Exception(Triple Fault)");
	 while(1);
	 }

ISR_CODE_H(exception14)
	{
	kprint("Exception! Page Fault");
	 while(1);
	 }

ISR_CODE_H(exception15)
	{
	kprint("Exception! Intel reserved");
	 while(1);
	 }

ISR_CODE_H(exception16)
	{
	kprint("Exception! ICoprocessor Error");
	 while(1);
	 }

