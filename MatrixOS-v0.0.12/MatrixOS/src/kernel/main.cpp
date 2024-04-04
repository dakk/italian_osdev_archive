/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: main.cpp			|
	| Date: 16/09/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	Prototipi	**
	**************************
*/

int main();

/*
	**************************
	**	 Include	**
	**************************
*/

//Driver Include
#include <hardware/driver/video.h>
#include <hardware/driver/bochsdebug.h>
#include <hardware/driver/keyboard.h>
#include <hardware/driver/pit.h>
#include <hardware/driver/vbe.h> 

//Hardware Include
#include <hardware/io.h>

//CPU Include
#include <hardware/cpu/gdt.h>
#include <hardware/cpu/idt.h>
#include <hardware/cpu/irq.h>

//kservices Include
#include <kservices/kout.h>
#include <kservices/kdbg.h>
#include <kservices/kshell.h>

//Memory Include
#include <memory/physicalmem.h>
#include <memory/virtualmem.h>

//Libreria C/C++
#include <new>
#include <string>

/*
	**************************
	**	 Namespace	**
	**************************
*/

//using namespace


/*
	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////
*/

extern "C"{

	void _start(){
		main();
		while(1);
	}

}

int main(){
	//Abilito il debuger di bochs
	hardware::driver::vm::BochsDebug::Init();

	//Inizializo i driver video e pulisco lo schermo
	hardware::driver::video::Video::Init();
	hardware::driver::video::Video::Clear();

	//Scrivo il messaggio iniziale
	hardware::driver::video::Video::setTextColor(hardware::driver::video::Green);
	kservices::KOut::WriteLine("MatrixOS");
	kservices::KOut::WriteLine("Build: " __DATE__ " " __TIME__);
	kservices::KOut::WriteLine();

	//Inizializzo la GDT
	kservices::KOut::Write("GDT Init...");
	hardware::cpu::GDT::Init();
	kservices::KOut::WriteLine(" OK");

	//Inizializzo la IDT
	kservices::KOut::Write("IDT Init...");
	hardware::cpu::IDT::Init();
	kservices::KOut::WriteLine(" OK");
	
	//Inizializzo gli IRQ
	kservices::KOut::Write("IRQ Init...");
	hardware::cpu::IRQ::Init();
	kservices::KOut::WriteLine(" OK");
	
	//Abilito gli interrupt - sti: Abilito cli: Disabilito
	asm("sti");

	//Iniziallizo la modalità grafica VBE (VESA BIOS EXTENSION).
        //hardware::driver::video::VBE::Init();

	//Inizializzo il gestore della memoria
	kservices::KOut::Write("PhysicalMem Manager Init...");
	memory::PhysicalMem::Init();
	memory::VirtualMem::Init();
	kservices::KOut::WriteLine(" OK");
	
	//Dimensione della memoria e numero di pagine
	kservices::KOut::Write("MemSize: ");
	kservices::KOut::Write((int) (memory::PhysicalMem::getMemSize() / 1024));
	kservices::KOut::Write(" Kb  ");
	kservices::KOut::Write((int) memory::PhysicalMem::getPageNum());
	kservices::KOut::WriteLine(" page found.");

	//Driver della tastiera
	kservices::KOut::Write("Keyboard driver Init...");
	hardware::driver::keyboard::Keyboard::Init();
	kservices::KOut::WriteLine(" OK");
	
	kservices::KOut::WriteLine("");
	
	//Avvio la shell
	kservices::KDbg::WriteLine("Starting the shell");
	kservices::KOut::WriteLine("Starting the shell");
	kservices::KShell::Start();

	//Entro in un ciclo infinito
	while(1);
}
