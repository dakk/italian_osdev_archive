#include <mmu.h>
#include <pagine_libere.h>
#include <fis_mmu.h>
#include <pagine_virtuali.h>
#include <debug.h>
#include <desc_mem.h>

#include <video.h>


void init_mmu(mem_desc * desc)
{
	debug << "inizializzazione di una mmu in corso...\n";
	disabilita_hardware_paging();
	init_spazio_virtuale_libero(desc->pagine_virtuali_libere);
	init_spazio_virtuale(desc);
	//desc->inizio =  NULL;
	alloca((1 << 20), desc);

	abilita_hardware_paging();
}

void init_kernel_mmu(mem_desc * desc)
{
	kout << "inizializzazione della mmu del kernel in corso...\n";
	disabilita_hardware_paging();
	init_spazio_virtuale_libero(desc->pagine_virtuali_libere);
	init_spazio_virtuale(desc);

	//prendo la pagina riservata alla tabella di allocazione
	init_desc_mem(desc);
	setta_desc_mem(desc, (void *) 0, ((1 << 20) + 0x1000) / 4096);


//      debug << "-----------------Sto per allocare il primo mega di ram---------" << endl;
	//alloco circa il primo mega di ram
	/*setta_pagina_virtuale(desc->pd, (void*)0, (void*)0);
	 * setta_pagina_virtuale(desc->pd, (void*)4096, (void*)4096); */

	void *addr = prendi_spazio_virtuale(((1 << 20) / 4096),
					    desc->pagine_virtuali_libere);
	setta_spazio_virtuale(desc->pd, addr, ((1 << 20) / 4096));




	dword control = (dword) desc->pd;
	control &= 0xFFFFF000;

	//debug << "\nil nuovo valore di cr3 è: " << control << endl;
	asm volatile ("mov %0, %%cr3"::"r" (control));
//      debug << "\nsettato cr3" << endl;


	page_dir_entry *mydir = (page_dir_entry *) 0;
	print_paging(mydir);

	abilita_hardware_paging();

}

void *alloca(size_t len, mem_desc * desc)
{
	size_t pagine = len / 4096;
	if (len % 4096) {
		pagine++;
	}
	//debug << "alloca: devo allocare " << len << " byte per una quantità di pagine pari a " << pagine << endl;
	void *addr =
		prendi_spazio_virtuale(pagine, desc->pagine_virtuali_libere);
	setta_spazio_virtuale(desc->pd, addr, pagine);
	setta_desc_mem(desc, addr, pagine);
	return addr;
}

void dealloca(void *addr, mem_desc * desc)
{
	size_t num_pag;
	//debug << "dealloca: devo deallocare la memoria a partire da " << (dword)addr<< endl;
	num_pag = libera_desc_mem(desc, addr);
	libera_spazio_virtuale(desc, addr, num_pag);

}
