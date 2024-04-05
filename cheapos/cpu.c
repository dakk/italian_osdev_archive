
#include "cpu.h"


//Inizzializza CPU
void cpu_init()
	{
	char cpumodel[KSTRNG_LUNG];
	struct time_tm_t orainiziale;

	kprint("CPU: ");

	//Rileva CPU Modello by Produttore
	cpu_detect(cpumodel);
	kprint(cpumodel);
	kprint("\n");

	//Riprogramma PIC
	irq_pic_init(IRQ_START);
	kprint("\tPIC rimappata da interrupt %d\n",IRQ_START);

	//Inizializzo la IDT
	isr_idt_init();
	kprint("\tIDT e TSS caricate\n");

	time_init();
	time_tm(&orainiziale);
	kprint("\tCLOCK inizzializzato\tTP:%d",TICK_PERIOD);
	kprint("\tInizio %2d:%2d:%2d\t%s\n",orainiziale.ore,orainiziale.min,orainiziale.sec,(!time_isinvalid())?"":"Ora CMOS non corretta!");
	}

inline void cpu_sti()
	{
	asm("sti");
	}

inline void cpu_cli()
	{
	asm("cli");
	}


