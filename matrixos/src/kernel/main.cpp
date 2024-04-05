/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: main.cpp			|
	| Date: 16/09/2004			|
	| Authors: Davide Bettio		|
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
#include <hardware/driver/vbeconsole.h>
#include <hardware/driver/initconsole.h>
#include <hardware/driver/floppy.h>
#include <hardware/driver/rtc.h>

//Hardware Include
#include <hardware/io.h>

//CPU Include
#include <hardware/cpu/gdt.h>
#include <hardware/cpu/idt.h>
#include <hardware/cpu/irq.h>
#include <hardware/cpu/cpuid.h>

//kservices Include
#include <kservices/kout.h>
#include <kservices/kdbg.h>
#include <kservices/kshell.h>

//Memory Include
#include <memory/physicalmem.h>
#include <memory/virtualmem.h>
#include <memory/paging.h>

//Task Include
#include <task/scheduler.h>

//Libreria C/C++
#include <new>
#include <string>

//MultiBoot
#include <boot/multiboot.h>
#include <boot/mboot.h>

//Syscall
#include <syscall.h>

/*
	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////
*/

extern "C"{

	/*
	  Viene chiamata da _start che si trova in boot.S
	  Verifica che il kernel sia stato avviato da un bootloader
	  conforme alle specifice MultiBoot e chiama main().
	*/
	void mbstart(unsigned long magic,multiboot_info * info){
		if (boot::mboot::Init(magic, info) == 0){
			main();
		}
		while(1);
	}

}

int main(){
	//Abilito il debuger di bochs
	hardware::driver::vm::BochsDebug::Init();

	//Inizializzo la console iniziale
	hardware::driver::video::InitConsole::Init(boot::mboot::getGraphicMode());

	//Scrivo il messaggio iniziale
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Green);
	kservices::KOut::WriteLine("MatrixOS");
	kservices::KOut::WriteLine("Build: " __DATE__ " " __TIME__);
	kservices::KOut::WriteLine();

	//Scrivo le informazioni sulla CPU
	hardware::cpu::CPUID::Init();

	//Inizializzo la GDT - Pericolo BUG
	//kservices::KOut::Write("GDT Init...");
	//hardware::cpu::GDT::Init();
	//kservices::KOut::WriteLine(" OK");

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

	//Inizializzo il gestore della memoria
	kservices::KOut::WriteLine("PhysicalMem Manager Init:");
	memory::PhysicalMem::Init();
	kservices::KOut::WriteLine("PhysicalMem Manager Init: OK");
	kservices::KOut::Write("Paging: ");
	memory::Paging::Init();
	
	//Faccio in modo che dopo aver abilitato il pagin la memoria video sia
	//ancora accessibile	
	if (boot::mboot::getGraphicMode() != 0){
		hardware::driver::video::VBE::ReInit();
	}

	kservices::KOut::WriteLine("Enabled");

	memory::VirtualMem::Init();
	

	//Driver Floppy
	hardware::driver::Floppy::Init();

	//Driver della tastiera
	kservices::KOut::Write("Keyboard driver Init...");
	hardware::driver::keyboard::Keyboard::Init();
	kservices::KOut::WriteLine(" OK");

	hardware::driver::RTC::Init();
	
	SysCall::Init();
	
	kservices::KOut::WriteLine();

	//Avvio il multitasking
	kservices::KOut::Write("Init Scheduler...");
	task::Scheduler::Init();
	kservices::KOut::WriteLine(" OK");

	//Avvio la shell
	kservices::KDbg::WriteLine("Starting the shell");
	kservices::KOut::WriteLine("Starting the shell");
	task::Scheduler::CreateNewTask(kservices::KShell::Start, "Shell");

	//Entro in un ciclo infinito
	while(1);
}
