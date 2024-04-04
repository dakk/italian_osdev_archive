#include <video.h>
#include <exec.h>
#include <fat.h>
#include <floppy.h>
#include <cstring>
#include <debug.h>
#include <gdt.h>
#include <idt.h>
#include <fis_mmu.h>
#include <kmmu.h>
#include <irq.h>
#include <mmu.h>
#include <pagine_libere.h>
#include <fis_mmu.h>
#include <pagine_virtuali.h>
#include <pit.h>
#include <8259.h>
#include <task.h>
#include <tree>
#include <cstddef>
#include <cstdlib>
#include <rtc.h>
#include <ctime>
#include <syscall.h>
#include <messaggi.h>
#include <tastiera.h>
#include <delay.h>
#include <hmu.h>
#include <speaker.h>
#include <key_traslation.h>
#include <cstdio>
#include <vt100.h>
#include <serial.h>
#include <var_amb.h>


mem_desc kernel_desc;

int main();

void __do_global_ctors(void);

/*è la prima funzione che sarà chiamata!!!
IMPORTANTE: deve essere anche la prima implementata nel file dove c'è il main*/
extern "C" void _start()
{
	__do_global_ctors();
	main();
	while (1);
}

extern volatile tick_t tick;

inline void time_irq()
{
	tick++;
}

extern page_dir_entry *kernel_dir;

void process0()
{
	kout << "\nsono un processo che rompe le scatole\n";
	while (1);
}

void process1()
{
	int i;
	for (i = 0; i < 100; i++)
		kout << i << endl;
}

//task inutile che non sarà mai usato
void dummy()
{
	while (1);
}

/*quando viene inizializzata la mmu si perde il valore del bios, allora mi salvo la roba che mi server*/
word bios_ser_mem[4];
void get_ser_bios_mem()
{
	word *ptraddress;
	word i = 0;
	ptraddress = (word *) 0x400;
	for (i = 0; i < 4; i++) {
		bios_ser_mem[i] = *ptraddress;
		++(ptraddress);
	}
}

extern "C" int main()
{
	debug << "\nItalios avviato" << endl;
	kout << endl << endl << "\t\tItaliOs 0.3" << endl << endl;
	kout << "Benvenuto nell' Italia Operating System" << endl;
	init_gdt();
	idt::init();
	asm("finit");
	get_ser_bios_mem();
	init_spazio_fisico();
	init_kernel_mmu(&kernel_desc);
	kout << "calibro il  delay" << endl;
	//TODO: AGGIUSTARE LA FUNZIONE PER IL DELAY PER FARLA ANDARE SUI P4
	//calibrateDelayLoop();
	add_irq(time_irq, 0);
	pit_count mypit;
	mypit.set_frequenza(100);
	mypit.carica();
	init_rtc();
	init_scheduler();
	enable_irq(0);
	init_syscall();
	init_msg();
	init_variabili_ambiente();
	add_task(dummy, "inutile");
	init_hmu();
	init_tastiera();
	init_map();
	init_uart();
	init_floppy();
	init_fat();
	init_dll_manager();
	kout << "Sto per lanciare shell.elf....\n";
while(1)
	exec("shell.elf", "ciao");
	return 0;
}

void __do_global_ctors(void)
{
	//la lista dei costruttori è definita nello scrpt di ld
	extern void (*__CTOR_LIST__) ();
	void (**constructor) () = &__CTOR_LIST__;
	//il primo intero è il numero di costruttori
	int total = *(int *) constructor;
	constructor++;
	// eseguo i costruttori uno alla volta
	while (total--) {
		(*constructor) ();
		constructor++;
	}
}
