#include <mmu.h>
#include <desc_mem.h>
#include <debug.h>
#include <pagine_virtuali.h>
#include <pagine_libere.h>
#include <fis_mmu.h>
#include <video.h>
#include <cstddef>


void init_desc_mem(mem_desc * desc)
{
	desc->tmem =
		(vm_mem_area *) prendi_spazio_virtuale(1,
						       desc->
						       pagine_virtuali_libere);
	kout << "Inizializzazione descrittori di memoria" << endl;
	//debug << "init_desc_mem: alloco la tabella all'indirizzo " << (dword)desc->tmem<< endl;
	setta_spazio_virtuale(desc->pd, (void *) desc->tmem, 1);

	//al momento non è necessaria un altra tabella
	desc->tmem->next = NULL;

	desc->tmem->desc_liberi = num_desc;
}

void setta_desc_mem(mem_desc * desc, void *addr, size_t num_page)
{
	vm_mem_area *cmem = desc->tmem;
	int i = 0;
	//cerco spazo libero nei blocchi già allocati
	while (cmem) {
		if (cmem->desc_liberi) {	//se questa tabella non è ancora piena
			size_t num = num_desc - cmem->desc_liberi;
			//      debug << "setta_desc_mem:  tabella: " << i << " desc: " << num <<" Indirizzo=  " << ((dword)(&cmem->mem[num]))<<  endl;
			cmem->mem[num].addr = addr;
			cmem->mem[num].size = num_page;
			cmem->desc_liberi--;
			return;
		}
		i++;
		cmem = cmem->next;
	}
	//debug << "devo allocare una nuova tabella" << endl;
	cmem = desc->tmem;
	while (cmem->next)
		cmem = cmem->next;
	cmem->next =
		(vm_mem_area *) prendi_spazio_virtuale(1,
						       desc->
						       pagine_virtuali_libere);
	setta_spazio_virtuale(desc->pd, cmem->next, 1);

	//cmem->next = (vm_mem_area *) alloca(sizeof(vm_mem_area), desc);


	cmem = cmem->next;
	cmem->desc_liberi = num_desc;
	cmem->next = NULL;

	//setto lo spazio per la nuova tabella
	setta_desc_mem(desc, cmem, 1);

	setta_desc_mem(desc, addr, num_page);
	return;
}

size_t libera_desc_mem(mem_desc * desc, void *addr)
{
	vm_mem_area *cmem = desc->tmem;
	size_t i = 0;
	//debug << "libera_desc_mem: indirizzo: " << (dword)addr << "   pagine = ";

	//cerco in tutti i blocchi allocati
	while (cmem) {
		for (i = 0; i < (num_desc - cmem->desc_liberi); i++) {
			if (cmem->mem[i].addr == addr) {
				debug << cmem->mem[i].size << endl;
				return cmem->mem[i].size;
			}
		}
		cmem = cmem->next;
	}
	//debug << "Nessuna" << endl;
	return 0;
}
