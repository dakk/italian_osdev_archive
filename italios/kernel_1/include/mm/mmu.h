#ifndef _MMU_H_
#define _MMU_H_

#include <cstddef>
#include <paging.h>

//indica l'inizio di una zona di memoria e per quante pagine si estende
struct vm_mem_desc{
    void *addr;		//indirizzo di partenza
	size_t size;	//numero di pagine
};

namespace{
	const size_t num_desc = 510;
}

//il tutto per funzionare bene sizeof(vm_mem_area) <= 4096
struct vm_mem_area{
	vm_mem_desc mem[num_desc];	
	size_t desc_liberi;		//indica il numero di descrittori liberi che ci sono ancora
	vm_mem_area *next;	//puntatore al prossimo descrittore dell'area
};

//descrittore della memoria
struct mem_desc{
    dword pagine_virtuali_libere[32768];  //bitmap per la gestione delle pagine virtuali libere
    page_dir_entry* pd;   //page directory base 
	vm_mem_area *tmem;	//tabella dell'uso della memoria
};

void init_mmu(mem_desc*  desc);

void init_kernel_mmu(mem_desc *desc);

void* alloca(size_t len, mem_desc*  desc);

void dealloca(void* addr, mem_desc*  desc);


#endif
