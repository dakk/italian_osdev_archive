#include <8259.h>
#include <cstddef>
#include <io.h>
#include <idt.h>
#include <task.h>
#include <video.h>

extern "C" void my_print(char *str)
{
	kout << str;
}


//il tutto è dichiaro come extern C per non avere problemi nel chiamare le varie variabili o funzioni
extern "C" {

	//vettore per i 16 interrupt
	void (*handler[16]) ();

	//handler di default 
	extern void internal_handler(void);

	//quando nessun handler è impostato per quell'irq
	void null_handler() {
		return;
	} void free_irq(size_t number) {
		//si occupa di dare l'eoi void free_irq(size_t number) {
		if (number < 8)
			outportb(M_PIC, 0x20);	//0x20 è il comando dell'eoi
		else
			outportb(S_PIC, 0x20);
		return;
	}

	void do_irq(dword * esp, byte number) {
		//      my_print("\neseguo\n");
		(*handler[number]) ();
		//my_print("\neseguito\n");
		//eseguo lo scheduler solo per l'interrupt del timer in modo da non creare troppi problemi
		scheduler(esp);
		//my_print("\nesco\n");
		return;
	}


	void _do_irq(dword esp) {
		//determino quel'è l'irq che si sta servendo
		byte current_irq = get_cur_irq();

		//quindi lo eseguo
		do_irq(&esp, current_irq);

		//dò l'eoi
		free_irq(current_irq);
		return;
	}


	asm(".globl internal_handler  \n"
	    "internal_handler:     \n"
	    "cli  /*disabilito gli interrupt*/	\n"
	    "pusha /*salvo tutti i registri*/	\n"
	    "movl %esp, %eax  /*salvo eax*/	\n"
	    "pushl %eax	/*pusho il valore dello stack*/\n"
	    "                       \n"
	    "call _do_irq  /*eseguo l'handler vero e proprio*/ \n"
	    "popl %eax /*mi riprendo lo stack*/	\n"
	    "mov %eax, %esp /*reimposto lo stack*/	\n"
	    "popa /*ripristino tutti i registri*/	\n"
	    "sti  /*riabilito gli interrupt*/	\n"
	    "iret /*esco dall'interrupt     */   \n");



}				//fine dell'extern


/*funzione per aggiungere un irq*/
extern "C" void add_irq(void (*func) (), byte number)
{
	handler[number] = func;
	return;
}


void init_irq()
{
	init8259();		//inizializzo l'8259
	//imposto i vari handler
	for (int i = 0; i < 16; i++) {
		if (i < 8)
			idt::add(internal_handler, M_VEC + i);
		else
			idt::add(internal_handler, S_VEC + i);
		add_irq(null_handler, i);
	}
	return;
}
