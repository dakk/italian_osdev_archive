#ifndef _IDT_H_
#define _IDT_H_

#include <cstddef>

namespace idt{        
        //inizializza la idt
	void init();	

	//installa l'handler func per l'interrupt index
	bool add(void (*func)(), size_t index);
	
	bool add(void (*func)(int), size_t index);

	//cancella l'handler di un interrupt e restituisce il puntatore al precedente handler
	//se non è installato nessun handler restituisce 0
	void* del(size_t index);

	//restituisce l'indirizzo di un handler
	//se non è installato nessun handler restituisce 0
	void* get(size_t index);

	void sti();
	void cli();
};

#endif
