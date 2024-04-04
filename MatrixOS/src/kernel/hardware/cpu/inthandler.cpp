/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: inthandler.cpp		|
	| Date: 29/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/cpu/inthandler.h>

#include <kservices/kerr.h>
#include <hardware/driver/initconsole.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::cpu;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////



void IntHandler::IntNull(){
	asm("cli");

	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int Null !!!");
	
	while(1);
}

void IntHandler::Int0(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);

	kservices::KErr::WriteLine("Int 0 - Divide Error !!");
	
	while(1);
}

void IntHandler::Int1(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 1 - Debug Interrupt !!");

	while(1);
}

void IntHandler::Int2(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 2  - NMI Interrupt !!");

	while(1);
}

void IntHandler::Int3(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 3  - Breakpoint !!");

	while(1);
}

void IntHandler::Int4(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 4  - Overflow !!");

	while(1);
}

void IntHandler::Int5(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 5  - BOUND Range Exceeded !!");

	while(1);
}
void IntHandler::Int6(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 6  - Invalid Opcode !!");

	while(1);
}
void IntHandler::Int7(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 7  - Device Not Avaible !!");

	while(1);
}
void IntHandler::Int8(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 8  - Double Fault !!");

	while(1);
}
void IntHandler::Int10(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 10  - Invalid TSS !!");

	while(1);
}

void IntHandler::Int11(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 11  - Segment Not Present !!");

	while(1);
}

void IntHandler::Int12(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 12  - Stack-Segment Fault !!");

	while(1);
}

void IntHandler::Int13(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 13  - General Protection !!");

	while(1);
}

void IntHandler::Int14(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 14  - Page Fault !!");

	while(1);
}

void IntHandler::Int16(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 16  - Floating-Point Error !!");

	while(1);
}
void IntHandler::Int17(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 17  - Alignment Check !!");

	while(1);
}
void IntHandler::Int18(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 18  - Machine Check !!");

	while(1);
}
void IntHandler::Int19(){
	asm("cli");
	
	hardware::driver::video::InitConsole::setTextColor(hardware::driver::video::Red);
	
	kservices::KErr::WriteLine("Int 19  - Streaming SIMD Extensions !!");

	while(1);
}
