/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: idt.cpp			|
	| Date: 28/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

/*
	**************************
	**	 Include	**
	**************************
*/

//Include per la IDT
#include <hardware/cpu/idt.h>

//Include per il debug
#include <kservices/kdbg.h>
#include <kframework/kframework.h>

//IRQ e IntHandler
#include <hardware/cpu/irq.h>
#include <hardware/cpu/inthandler.h>

//Tipi definiti
#include <kdef.h>

//DEBUG: Per abilitare il debug decomentare la seguente riga
//#define IDT_DEBUG

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::cpu;



	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////



//La tabella degl interrupt - qword = 64 bit
qword IDT::idt[IDT_SIZE];

/*
  Inizializzo gli interrupt
*/
void IDT::Init(){
	
	//Riempo la IDT di IntNull
	for(size_t i = 0; i < IDT_SIZE; i++){
		Add(IntHandler::IntNull,i);
	}

	/*
		Aggiungo gli interrupt del processore
	*/
	Add(IntHandler::Int0,0); //#DE - Divide Error
	Add(IntHandler::Int1,1); //#DB - Debug
	Add(IntHandler::Int2,2); //NMI Interrupt
	Add(IntHandler::Int3,3); //#BP - Breakpoint
	Add(IntHandler::Int4,4); //#OF - Overflow
	Add(IntHandler::Int5,5); //#BR - BOUND Range Exceeded
	Add(IntHandler::Int6,6); //#UD - Invalid Opcode
	Add(IntHandler::Int7,7); //#NM - Device Not Avaible
	Add(IntHandler::Int8,8); //#DF - Double Fault
	
	Add(IntHandler::Int10,10); //#TS - Invalid TSS
	Add(IntHandler::Int11,11); //#NP - Segment Not Present
	Add(IntHandler::Int12,12); //#SS - Stack-Segment Fault
	Add(IntHandler::Int13,13); //#GP - General Protection
	Add(IntHandler::Int14,14); //#PF - Page Fault
	
	Add(IntHandler::Int16,16); //#MF - Floating-Point Error
	Add(IntHandler::Int17,17); //#AC - Alignment Check
	Add(IntHandler::Int18,18); //#MC - Machine Check
	Add(IntHandler::Int19,19); //#XF - Streaming SIMD Extensions

	//Aggiorno il registro
	setIDTR(idt, IDT_SIZE);
}

/*
  Aggiungo l'handler alla IDT
*/
bool IDT::Add(void (*func)(), size_t index){
	//Verifico se index ha un valore valido
	if (index < IDT_SIZE){
		dword addr_func = (dword)func;
	
		bool present = true;
		int ring = 0;
		bool dimensione32 = true;
	
		qword tmp;
	
		tmp = addr_func & 0xFFFF;
		tmp = tmp | (((qword)addr_func) & 0x00000000FFFF0000LL)<<32;
	
		//Segmento di codice
		tmp = tmp | 0x8LL<<16;
	
		//Presente
		if(present == true) tmp = tmp | 0x0000800000000000LL;
	
		//Ring
		tmp = tmp | (((qword)ring) & 0x0000000000000003LL)<<44;
	
		//Dimensione
		if(dimensione32 == true){
			tmp = tmp | 0x00000E0000000000LL;
		}else{
			tmp = tmp | 0x0000060000000000LL;
		}
	
		#ifdef IDT_DEBUG
			kservices::KDbg::Write("idt.cpp - Aggiungo un interrupt int=");
			kservices::KDbg::WriteLine((int) index);
		#endif
		
		idt[index] = tmp;
	
		return true;
	}
	
	//L'indice non è valido
	return false;
}

/*
  Elimino l'handler dalla IDT
*/
bool IDT::Del(size_t index){

	#ifdef IDT_DEBUG
		kservices::KDbg::Write("idt.cpp - Elimino l'interrupt index=");
		kservices::KDbg::WriteLine((int) index);
	#endif

	//Verifico se index ha un valore valido
	if (index < IDT_SIZE){
		//Handler Null
		Add(IntHandler::IntNull,index);

		return true;
	}
	
	//L'indice non è valido
	return false;
}

/*
  Setto il registro che contiene l'indirizzo della IDT
*/
void IDT::setIDTR(qword *base, size_t num_desc){
	dword idt_reg[2];
	
	idt_reg[0] = (num_desc*8) << 16;
	idt_reg[1] = (dword)base;
	__asm__ __volatile__ ("lidt (%0)": :"g" ((char *)idt_reg+2));
}
