#include <video.h>
#include <debug.h>
#include <gdt.h>
#include <idt.h>
#include <8259.h>
#include <irq.h>
#include <io.h>

int main();

void tastiera_handler();

void __do_global_ctors (void);

/*è la prima funzione che sarà chiamata!!!
IMPORTANTE: deve essere anche la prima implementata nel file dove c'è il main*/
void _start(){
	__do_global_ctors ();
	main();
	while(1);
}

int main(){
	debug << "\nItalios avviato" << endl;
	kout  << endl << endl <<  "\t\t\t\tItaliOs " << endl << endl;
	kout << "Benvenuto nell' Italia Operating System" << endl;
	init_gdt();
	idt::init();
	add_irq(tastiera_handler, 1);
	enable_irq(1);
	return 0;
}

void tastiera_handler(){
      //svuotiamo il buffer della tastiera
        inportb(0x60);
	kout << "e' stato premuto un tasto    " << get_cur_irq() <<  endl;
}

void __do_global_ctors (void)
{
    //la lista dei costruttori è definita nello scrpt di ld
  extern void (*__CTOR_LIST__)();
  void (**constructor)() = &__CTOR_LIST__;
  //il primo intero è il numero di costruttori
  int total = *(int *)constructor;
  constructor++;
  // eseguo i costruttori uno alla volta
  while(total--)
    {
      (*constructor)();
      constructor++;
    }
}
