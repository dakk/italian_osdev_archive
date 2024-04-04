#include <pagine_libere.h>
#include <bitops.h>
#include <debug.h>


namespace{
        const size_t num_elem = 32768;
	const dword  VUOTO = 0;
	const dword  PIENO= 1;
	size_t num_pages;
	
	void setbitmapbit(size_t count, dword* pagine_libere, dword stato){
	        if(stato == VUOTO){
                        clear_bit(count, pagine_libere);
			return;
		}
		
		if(stato == PIENO){
                        set_bit(count, pagine_libere);
			return;
		}
		return;
	}
	
	//controlla se una pagina è settata oppure no
	//restituisce true in caso la pagina è occupata
	//restituisce false in caso la pagina è libera
	bool isset(size_t count, dword* pagine_libere){
	        return test_bit(count, pagine_libere);
	}
	
}

//pagine libere DEVE essere un vettore di 32786 elementi
void init_spazio_virtuale_libero(dword* pagine_libere){
        debug << "init_spazio_virtuale_libero: inizio" << endl;
        for(size_t i = 0; i < num_elem; i++)
	        pagine_libere[i] = VUOTO;
}

//questa funzione si occupa di 
//1) Cercare lo spazio libero per il numero di pagine richieste. Viene usato il algoritmo first fit. 
//Pertanto viene cercato il primo blocco grande a sufficenza per  contenere le pagine richiestre
//2)dopo averle cercate le marca come usate. NB E' compito di qualche altra funzione tenere conto delle pagine usate e poi liberarle 1 per 1
//
//restituisce il puntatore all'indirizzo della prima pagina
void* prendi_spazio_virtuale(size_t pagine, dword* pagine_libere){
        debug << "prendi_spazio_virtuale: pagine: " << pagine << endl;
	size_t count;
	size_t ptrov = 0;
	for (count = 0; count < pagine; count++) {
		if(!isset(count, pagine_libere)){
			ptrov++;
		}else
			ptrov = 0;
		if(ptrov == pagine){
			unsigned int final_page =  count;
			count -= ptrov-1;
			for (; count <= final_page; count++)
				setbitmapbit(count, pagine_libere, PIENO);
			return (void *) (4096 * (final_page-ptrov));
		}
	}
        debug << "\nMemoria Finita!!! Oleeeeeeeeee" << endl;
	return (void *) 0;
}

//marca come libera una pagina virtuale
void libera_pagina_virtuale(size_t pagina, dword* pagine_libere){
	setbitmapbit(pagina, pagine_libere, VUOTO);
	return;
}

