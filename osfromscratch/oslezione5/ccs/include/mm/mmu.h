#ifndef _MMU_H_
#define _MMU_H_

#include <stddef.h>
#include <paging.h>

struct mem_link{
        mem_link* succ;
	size_t size;
};

struct mem_desc{
        dword pagine_virtuali_libere[32768];
        page_dir_entry* pd;   //page directory base 
	mem_link* inizio;        //link per la gestione delle pagine libere
};

void init_mmu(mem_desc*  desc);

void* alloca(size_t len, mem_desc*  desc);

#endif
