/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: kshell.cpp			|
	| Date: 31/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <kservices/kshell.h>
#include <kservices/kout.h>
#include <hardware/driver/keyboard.h>
#include <hardware/io.h>
#include <string.h>
#include <hardware/driver/floppy.h>
#include <task/scheduler.h>
#include <hardware/cpu/cpuid.h>
#include <hardware/driver/rtc.h>
#include <boot/mboot.h>
#include <memory/physicalmem.h>
#include <stdlib.h>
#include <kdef.h>


/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace kservices;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////

/*
 Task A
*/
void A(){
	while(1){
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		
		asm("cli");
		kservices::KOut::Write("A");
		asm("sti");

		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		
	}
}

/*
  Task B
*/
void B(){
	while(1){
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		
		asm("cli");
		kservices::KOut::Write("B");
		asm("sti");

		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		
	}
}

/*
  Avvio la shell
*/
void KShell::Start(){

	kservices::KOut::WriteLine("For the command list type 'help'");
	
	char str[225];
	while(1){
		kservices::KOut::Write("MatrixKSH>");

		hardware::driver::keyboard::Keyboard::getStr(str,225);

		if(!strcmp(str, "help")){
			kservices::KOut::WriteLine("reboot");
			kservices::KOut::WriteLine("clear");
			kservices::KOut::WriteLine("authors");
			kservices::KOut::WriteLine("version");
			kservices::KOut::WriteLine("syscalltest");
			kservices::KOut::WriteLine("tasktest");
			kservices::KOut::WriteLine("ps");
			kservices::KOut::WriteLine("bootinfo");
			kservices::KOut::WriteLine("meminfo");
			kservices::KOut::WriteLine("cpuinfo");
			kservices::KOut::WriteLine("date");
			kservices::KOut::WriteLine("fdump");

		}else if(!strcmp(str, "reboot")){
			kservices::KOut::WriteLine("Rebooting...");
			//Disabilito gli interrupt
			asm("cli");
			//Mando un segnale di reboot alla tastiera
			outportb(0x64,0xFE);
			//Entro in un ciclo infinito
			while(1);

		}else if(!strcmp(str, "clear")){
			hardware::driver::video::InitConsole::Clear();

		}else if(!strcmp(str, "authors")){
			kservices::KOut::WriteLine("Davide B. 'WindowsUninstall'\n"
						"Enrico S. 'Ada Byron'\n"
						"Michele 'jockerfox'\n"
						"Giuseppe M. 'Denever'\n"
						"Ringrazio anche Silvio Abruzzo e Ivan Gualandri.");

		}else if(!strcmp(str, "version")){
			kservices::KOut::WriteLine("0.0.20 - " __DATE__ " " __TIME__);

		}else if(!strcmp(str, "syscalltest")){
			//Chiama la SysCall WRITE
			asm("mov $0x04, %eax");
			asm("Int $0x80");

			//Chiama una SysCall nulla
			asm("mov $0x10, %eax");
			asm("Int $0x80");

		}else if(!strcmp(str, "tasktest")){
			task::Scheduler::CreateNewTask(A, "Print A");
			task::Scheduler::CreateNewTask(B, "Print B");

		}else if(!strcmp(str, "ps")){
			kservices::KOut::WriteLine("PID    Task");
			int pid = 0;
			char pname[64];

			while(task::Scheduler::getTaskName(pid, pname, 64)){
				kservices::KOut::Write((unsigned int)pid);
				kservices::KOut::Write("       ");
				kservices::KOut::WriteLine(pname);
				pid++;
			}

		}else if(!strcmp(str, "cpuinfo")){
			kservices::KOut::WriteLine("CPUINFO:");
			hardware::cpu::CPUID::PrintCPUInfo();

		}else if(!strcmp(str, "bootinfo")){
			kservices::KOut::WriteLine("BOOTINFO:");
			kservices::KOut::Write("BOOT DEVICE: ");
			kservices::KOut::WriteLine((unsigned dword) boot::mboot::getBootDevice());
			kservices::KOut::Write("MEM LOWER: ");
			kservices::KOut::WriteLine((unsigned dword) boot::mboot::getMemLower());
			kservices::KOut::Write("MEM UPPER: ");
			kservices::KOut::WriteLine((unsigned dword) boot::mboot::getMemUpper());

		}else if(!strcmp(str, "meminfo")){
 			kservices::KOut::WriteLine("MEMINFO: ");
			kservices::KOut::Write("PYSICAL PAGES: ");
			kservices::KOut::WriteLine(memory::PhysicalMem::getPageNumber());
			kservices::KOut::Write("PYSICAL FREE PAGES: ");
			kservices::KOut::WriteLine(memory::PhysicalMem::getFreePageNumber());
		}else if(!strcmp(str, "date")){
			hardware::driver::RTC::Init();

		}else if(!strcmp(str, "fdump")){
			static int block = 0;
			static byte *mem = (byte*) malloc(1024);//Errore

			hardware::driver::Floppy::ReadBlock(block, 2, mem);

			for (int i = 0; i < 1024; i++){
				kservices::KOut::Write((byte) mem[i], kframework::HEX);
				kservices::KOut::Write(" ");
			}
			kservices::KOut::WriteLine();

			block += 2;
		}else if(!strcmp(str,"")){
		
		}else{
			kservices::KOut::Write("MatrixKSH: ");
			kservices::KOut::Write(str);
			kservices::KOut::WriteLine(": command not found");
		}

		for (int stri = 0; stri <255;stri++) str[stri] = '\0';
	}
}

