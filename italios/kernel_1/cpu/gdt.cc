#include <cstddef>
#include <gdt.h>
#include <debug.h>

qword gdt[5];

void set_gdtr(qword * base, size_t num_desc)
{
	dword gdt_reg[2];
	gdt_reg[0] = (num_desc * 8) << 16;
	gdt_reg[1] = (dword) base;
	debug << "Sto per settare le gdt" << endl;
	debug.register_trace(ON);
	__asm__ __volatile__("lgdt (%0)"::"g"((char *) gdt_reg + 2));
	debug.register_trace(OFF);
}



void init_gdt()
{
	gdt[0] = 0;
	//Limite = 0xFFFFF (4 gb)
	//Base= 0
	//Ring= 0
	//Granularità = si
	//Tipo= 0xA  (codice con permesso di esecuzione e lettura)
	//Presente= si
	gdt[1] = 0x00CF9A000000FFFFLL;


	//Limite = 0xFFFFF (4 gb)
	//Base= 0
	//Ring= 0
	//Granularità = si
	//Tipo= 2  (Dati con permesso di lettura e scrittura)
	//Presente= si                  
	gdt[2] = 0x00CF92000000FFFFLL;

	set_gdtr(gdt, 5);
	//setto gli eflag o meglio li azzero tutti

	debug << "sto per settare gli eflag e i registri segmento" << endl;
	debug.register_trace(ON);

	asm("pushl $2; popf");
	//l'indice del segmento dei dati è 2, quindi la sua distanza dall'inizio è 0x10 (16 = 8 * 2)
	//mentre l'indice del segmento dei dati è 1 quindi la sua distanza è 8
	asm volatile ("movw $0x10,%ax     \n"
		      "movw %ax,%ds      \n"
		      "movw %ax,%es      \n"
		      "movw %ax,%fs      \n"
		      "movw %ax,%gs      \n"
		      "movw %ax,%ss      \n"
		      "ljmp $0x08,$next    \n" "nop\n" "nop\n" "next:\n");

	debug.register_trace(OFF);
	return;
}
