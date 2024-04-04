#include <fis_mmu.h>
#include <bitops.h>
#include <video.h>
#include <debug.h>

#define PATTERN 0x55AA55AA
#define ANTIPATTERN 0xAA55AA55

//calcola la quantità di ram presente nel pc
size_t count_memory()
{
	dword mem_end;
	dword *mem;
	dword mem_count, copia;
	word memkb;

	asm("cli");

	mem_count = 0;
	memkb = 0;

	do {
		memkb++;

		//calcolo il nuovo indirizzo
		mem_count += 1024 * 1024;

		//imposto il puntatore
		mem = (dword *) mem_count;

		//mi salvo il contenuto che c'è a quell'indirizzo
		copia = *mem;

		//PATTERN è un valore preso a caso
		*mem = PATTERN;

	      asm("": : :"memory");
		//controllo se l'indirzzo è ancora valido
		if (*mem != PATTERN)
			mem_count = 0;
		else {		//se è ancora valido riprovo per essere sicuro
			*mem = ANTIPATTERN;
		      asm("": : :"memory");
			if (*mem != ANTIPATTERN)
				mem_count = 0;
		}

	      asm("": : :"memory");
		*mem = copia;
	}
	while (memkb < 4096 && mem_count != 0);

	//calcolo la misura in byte                     (con le dovute approssimazioni ovviamente)
	mem_end = memkb << 20;

	asm("sti");

	return mem_end;
}


namespace {
	const size_t num_elem = 32768;
	const dword VUOTO = 0;
	const dword PIENO = 0xFFFFFFFF;
	dword page_bitmap[num_elem];	//1 bit 1 pagina   sono in totale 2^20 bit
	size_t num_pages;
	const size_t DIM_PAGE = 4096;

}

void init_spazio_fisico()
{
	num_pages = count_memory();
	kout << "Inizializzazione memoria fisica" << endl;
	kout << "Rilevati " << num_pages << "  byte ------->      " <<
		(num_pages / 1024) << " KB   ------>  " << (num_pages /
							    (1024 *
							     1024)) << " MB "
		<< endl;
	//determino il numero di pagine
	num_pages /= DIM_PAGE;
	for (size_t i = 0; i < num_elem; i++)
		page_bitmap[i] = 0;
}

//restituisce la base della pagina, se non ci sono + pagine restituisce NULL
void *prendi_pagina_fisica()
{
	size_t pagina_libera = find_first_zero_bit(page_bitmap, num_pages);
	if (pagina_libera > num_pages) {
		debug << "prendi_pagina_fisica: finito lo spazio fisico a disposizione" << endl;
		return NULL;
	}
	//debug << "prendi_pagina_fisica: trovata pagina fisica libera numero: " << pagina_libera << endl;
	set_bit(pagina_libera, page_bitmap);
	return (void *) (pagina_libera * DIM_PAGE);
}

//libera una pagina, se è una pagina valida viene liberata e viene tornato true, altrimenti viene tornato false
//viene tornato false se ad esempio l'indirizzo non è  un indirizzo restituito da prendi_pagina_fisica
bool rilascia_pagina(void *addr)
{
	dword indirizzo = (dword) addr;
	if (indirizzo % DIM_PAGE) {
		debug << "rilascia_pagina: pagina  fisica non valida" << endl;
		return false;
	}
	//debug << "rilascia_pagina: libero pagina fisica: " << (indirizzo/DIM_PAGE) << endl;
	clear_bit(indirizzo / DIM_PAGE, page_bitmap);
	return true;
}

//prende pagine che vanno da inizio a fine, restituisce il puntatore alla prima pagina
void *prendi_spazio_fisico(void *iinizio, void *ifine)
{
	dword inizio = (dword) iinizio;
	dword fine = (dword) ifine;
	size_t pinizio = inizio / DIM_PAGE;
	size_t pfine = fine / DIM_PAGE;
	if (fine % DIM_PAGE)
		pfine++;
	for (size_t i = pinizio; i < pfine; i++) {
		set_bit(i, page_bitmap);
		//debug << "prendi_spazio_fisico: setto la pagina fisica " << i << endl;
	}
	return (void *) (pinizio * 4096);
}

//libera pagine che vanno da inizio a fine, restituisce il puntatore alla prima pagina
void libera_spazio_fisico(void *iinizio, void *ifine)
{
	dword inizio = (dword) iinizio;
	dword fine = (dword) ifine;
	size_t pinizio = inizio / DIM_PAGE;
	size_t pfine = fine / DIM_PAGE;
	if (fine % DIM_PAGE)
		pfine++;
	for (size_t i = pinizio; i < pfine; i++) {
		clear_bit(i, page_bitmap);
		//debug << "libera_spazio_fisico: libero la pagina fisica  " << i << endl;
	}
}
