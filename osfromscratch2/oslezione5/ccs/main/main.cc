#include <video.h>
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


mem_desc kernel_desc;

int main();

void __do_global_ctors (void);

/*è la prima funzione che sarà chiamata!!!
IMPORTANTE: deve essere anche la prima implementata nel file dove c'è il main*/
void _start(){
	__do_global_ctors ();
	main();
	while(1);
}

extern page_dir_entry* kernel_dir;

int main(){
	debug << "\nItalios avviato" << endl;
	kout  << endl << endl <<  "\t\t\t\tItaliOs " << endl << endl;
	kout << "Benvenuto nell' Italia Operating System" << endl;
	init_gdt();
	idt::init();
	kout << "e' stato premuto un tasto" << endl;
	asm("int $13");
        init_spazio_fisico();
	init_kmmu();
	init_mmu(&kernel_desc);
	alloca(50, &kernel_desc);
	return 0;
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
