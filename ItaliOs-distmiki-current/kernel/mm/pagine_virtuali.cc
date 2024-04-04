#include <pagine_virtuali.h>
#include <paging.h>
#include <debug.h>
#include <pagine_libere.h>
#include <fis_mmu.h>
#include <video.h>

void init_spazio_virtuale(mem_desc * desc)
{
	kout << "Inizializzazione spazione virtuale" << endl;
	dword *addr[11];

	//mi memorizzo il primo indirizzo preso
	addr[0] = (dword *) prendi_pagina_fisica();
	desc->pd = (page_dir_entry *) addr[0];

	//alloco le altre dir e mi memorizzo i relativi indirizzi
	for (size_t i = 0; i < 10; i++) {
		addr[i + 1] = (dword *) prendi_pagina_fisica();
		desc->pd[i] =
			crea_page_dir((page_dir_entry *) addr[i + 1],
				      SCRITTURA_LETTURA | PRESENTE);

		page_dir_entry *current_dir = (page_dir_entry *) addr[i + 1];
		for (int k = 0; k < 1024; k++)
			current_dir[k] = 0;
	}

	//prendo lo spazio per le 11 pagine prese
	for (size_t i = 0; i < 11; i++) {
		prendi_pagina_virtuale(i, desc->pagine_virtuali_libere);
		setta_pagina_virtuale(desc->pd, (void *) addr[i],
				      (void *) addr[i]);
	}


//   debug << "\ninit_spazio_virtuale: init prima dir " << (dword)desc->pd << endl;

	for (size_t i = 10; i < 1024; i++)
		desc->pd[i] = 0;
}
//TODO: aggiungere il controllo della fine
void setta_spazio_virtuale(page_dir_entry * dir, void *linear_address,
			   size_t pagine)
{
	byte *addr = (byte *) linear_address;

	//debug << "\n\nsetta_spazio_virtuale: indirizzo lineare " << (dword) addr << endl;
	//debug << "\nsetta_spazio_virtuale: indirizzo dir" << (dword) dir;

	for (size_t i = 0; i < pagine; i++, addr += 4096) {
		setta_pagina_virtuale(dir, addr, prendi_pagina_fisica());
		//debug << "setta_spazio_virtuale: settata pagina relativa " << i << " con indirizzo " << (dword)(addr) << endl;
	}
}

void setta_pagina_virtuale(page_dir_entry * dir, void *linear_address,
			   void *fis_address)
{
	dword pde, pte;
	pde = (dword) linear_address >> 22;
	pte = ((dword) linear_address >> 12) & 0x3FF;

	//debug << "setta_pagina_virtuale:dir[" << pde << "] e la table[" << pte << "] = " << ((dword)(fis_address)) << endl;

	//controllo se il dir entry è presente
	if (!table_presente(dir[pde])) {
		//debug << "\nalloco una dir " << pde << endl;
		page_dir_entry *current_dir =
			(page_dir_entry *) prendi_pagina_fisica();
		//   debug << "alloco dir[" << pde << "] = " << (dword)(current_dir) << endl;
		//non è presente quindi alloco una bella tabella
		dir[pde] =
			crea_page_dir(current_dir,
				      SCRITTURA_LETTURA | PRESENTE);

		//imposto in modo che non ci siano tabelle disponibili
		for (int k = 0; k < 1024; k++)
			current_dir[k] = 0;
	}

	page_table_entry *table =
		(page_table_entry *) get_table_addr(dir[pde]);

	page_base *current_page = (page_base *) fis_address;
	//debug << "l'indirizzo della tabella è: " << (dword)(table+pte) << endl;

	table[pte] =
		crea_page_table(current_page, SCRITTURA_LETTURA | PRESENTE);
	return;
}


//TODO: aggiungere il controllo della fine
void libera_spazio_virtuale(mem_desc * desc, void *linear_address,
			    size_t pagine)
{
	byte *addr = (byte *) linear_address;
	//debug << "\n\nlibera_spazio_virtuale: indirizzo lineare " << (dword) addr << endl;
	for (size_t i = 0; i < pagine; i++, addr += 4096) {
		libera_pagina_virtuale(desc, addr);
		//debug << "libera_spazio_virtuale: eliminata pagina relativa " << i << " con indirizzo " << (dword)(addr) << endl;
	}
}

void libera_pagina_virtuale(mem_desc * desc, void *linear_address)
{
	//debug << "libera_pagina_virtuale: " << (dword)linear_address << " = " << (((dword)linear_address)/4096) << endl;
	page_dir_entry *dir = desc->pd;
	dword pde, pte, pg;
	pde = (dword) linear_address >> 22;
	pte = ((dword) linear_address >> 12) & 0x3FF;
	page_table_entry *table =
		(page_table_entry *) get_table_addr(dir[pde]);
	page_base *fis_addr = get_page_addr(table[pte]);
	//debug << "libera_pagina_virtuale: valore del pte prima: " << table[pte] << endl;
	set_assent(&table[pte]);
	//debug << "libera_pagina_virtuale: valore del pte dopo: " << table[pte] << endl;
	pg = (dword) linear_address;
	pg /= 4096;
	libera_pagina_virtuale_libera(pg, desc->pagine_virtuali_libere);

	rilascia_pagina(fis_addr);

	return;
}
