#include <video.h>


int main();

void __do_global_ctors (void);

/*è la prima funzione che sarà chiamata!!!
IMPORTANTE: deve essere anche la prima implementata nel file dove c'è il main*/
void _start(){
	__do_global_ctors ();
	main();
	while(1);
}

int main(){
	kout  << endl << endl <<  "\t\t\t\tItaliOs " << endl << endl;
	kout << "Benvenuto nell' Italia Operating System" << endl;
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
