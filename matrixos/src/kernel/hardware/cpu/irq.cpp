/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: irq.cpp			|
	| Date: 30/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
	| Atenzione: Parte del codice fa parte	|
	|	del sistema operativo ItaliOS.	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <hardware/cpu/irq.h>

#include <hardware/cpu/idt.h>

#include <hardware/cpu/bitops.h>

#include <hardware/io.h>

#include <task/scheduler.h>

#include <kdef.h>


//Include per il debug
#include <kservices/kdbg.h>
#include <kframework/kframework.h>

//DEBUG: Per abilitare il debug decomentare la seguente riga
//#define IRQ_DEBUG
/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace hardware::cpu;




	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////




word IRQ::irq_mask;

/************************************
	Codice EXTERN "C"
*************************************/
extern "C"{
	//vettore per i 16 interrupt
	void (*handler[16])();

	//handler di default 
	extern void internal_handler(void);

	//quando nessun handler è impostato per quell'irq
	void null_handler(){return;}
}
/************************************
	Fine del codice extern "c"
*************************************/

/*
  Inizializzo gli IRQ
*/
void IRQ::Init(){
	Init8259();
	
	for (int i = 0; i < 16; i++) {
		if (i < 8)
			IDT::Add(internal_handler, M_VEC + i);
		else
			IDT::Add(internal_handler, S_VEC + i);
		Add(null_handler, i);
	}
}


/*
  Funzione per aggiungere un irq
*/
bool IRQ::Add(void (*func) (), byte number){
	
	#ifdef IRQ_DEBUG
		kservices::KDbg::Write("irq.cpp - Aggiungo irq: ");
		kservices::KDbg::WriteLine(number);
	#endif

	if (number < IRQ_NUM){
		handler[number] = func;
		
		return true;
	}
	
	return false;
}

/*
  Abilita un IRQ
*/
bool IRQ::EnableIRQ(int irq_no){

	#ifdef IRQ_DEBUG
		kservices::KDbg::Write("irq.cpp - Abilito l'irq: ");
		kservices::KDbg::WriteLine(irq_no);
	#endif

	//controllo se è un irq valido
	if (irq_no <= 15) {
		//faccio in modo che il bit corrispondente all'irq valga 0.
		//Es. se irq_no vale 6 succede ciò:
		//(1 << irq_no)   =  1000000
		//~(1 << irq_no) = 0111111
		//irq_mask &= ~(1 << irq_no);  =  xxxx xxxx x0xx xxxx
		irq_mask &= ~(1 << irq_no);

		//se l'irq è maggiore di 8 mi assicuro che la linea 2 (quella della cascata) sia attivata
		if (irq_no >= 8)
			irq_mask &= ~(1 << 2);

		//sendo all'8259 la nuova configurazione dell'IMR
		outportb_p(M_PIC + 1, irq_mask & 0xFF);
		outportb_p(S_PIC + 1, (irq_mask >> 8) & 0xFF);
		return true;
	}
	return false;
}

/*
  Disabilita un IRQ
*/
bool IRQ::DisableIRQ(int irq_no){

	#ifdef IRQ_DEBUG
		kservices::KDbg::Write("irq.cpp - disabilito l'irq: ");
		kservices::KDbg::WriteLine(irq_no);
	#endif

	if (irq_no <= 15) {
		//mi occupo di mettere ad 1 il bit corrisponde all'irq che ci interessa
		//es. se irq_no vale 3
		//(1 << irq_no)  =  1000
		//irq_mask |= (1 << irq_no)  =  xxxx xxxx xxxx 1xxx
		irq_mask |= (1 << irq_no);

		//se gli irq alti (del pic slave) lo disabilito così è un pò + efficente ^__^
		if ((irq_mask & 0xFF00) == 0xFF00)
			irq_mask |= (1 << 2);

		outportb_p(M_PIC + 1, irq_mask & 0xFF);
		outportb_p(S_PIC + 1, (irq_mask >> 8) & 0xFF);
		return true;
	}
	return false;
}

/*
  Inizializza il pic 8259
*/
void IRQ::Init8259(){
	// Inizio l'inizializzazione 
	outportb_p(M_PIC, ICW1);
	outportb_p(S_PIC, ICW1);

	//la base nel vettore di interrupt
	outportb_p(M_PIC + 1, ICW2_M);
	outportb_p(S_PIC + 1, ICW2_S);

	//la cascata
	outportb_p(M_PIC + 1, ICW3_M);
	outportb_p(S_PIC + 1, ICW3_S);

	//finisco l'inizializzazione
	outportb_p(M_PIC + 1, ICW4);
	outportb_p(S_PIC + 1, ICW4);

	irq_mask = 0xFFFF;
	outportb_p(M_PIC + 1, irq_mask & 0xFF);
	outportb_p(S_PIC + 1, (irq_mask >> 8) & 0xFF);
}

/************************************
	Codice EXTERN "C"
*************************************/
extern "C"{

	size_t getCurrentIRQ(){
		byte irq;

		//indico che voglio leggere l'ISR       
		outportb_p(M_PIC, 0x0B);
		irq = inportb_p(M_PIC);

		//se l'8259 master non ha nessun interrupt attivo controllo lo slave
		if (irq == 0) {
			outportb_p(S_PIC, 0x0B);
			irq = inportb_p(S_PIC);
			return find_first_bit(&irq, sizeof(byte));
		}
		
		return find_first_bit(&irq, sizeof(byte));
	}

	//si occupa di dare l'eoi
	void free_irq(size_t number){
		if(number < 8){
			outportb(M_PIC, 0x20);
		}else{
			outportb(S_PIC, 0x20);
		}
	}

	void  _do_irq(dword esp){
		//determino quel'è l'irq che si sta servendo
		byte current_irq = getCurrentIRQ();

		//quindi lo eseguo
		(*handler[current_irq])();

		//Se l'irq è del timer eseguo lo scheduler
		if (current_irq == 0) task::Scheduler::Schedule(&esp);

		//dò l'eoi
		free_irq(current_irq);
	}

		asm(".globl internal_handler					\n"
		    "internal_handler:						\n"
		    "cli		/*disabilito gli interrupt*/		\n"
		    "pusha		/*salvo tutti i registri*/		\n"
		    "movl %esp, %eax	/*Salvo il contenuto di esp*/		\n"
		    "pushl %eax		/*Salvo nello stack eax*/		\n"
		    "call _do_irq	/*eseguo l'handler vero e proprio*/	\n"
		    "popl %eax		/*Prendo eax dallo stack*/		\n"
		    "mov %eax, %esp	/*Cambio il puntatore dello stack*/	\n"
		    "popa		/*ripristino i registri*/		\n"
		    "sti		/*riabilito gli interrupt*/		\n"
		    "iret		/* esco dagli interrupt*/		\n"        
		);
}
/************************************
	Fine del codice extern "c"
*************************************/
