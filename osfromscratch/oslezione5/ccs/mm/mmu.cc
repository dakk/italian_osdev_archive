#include <mmu.h>
#include <pagine_libere.h>
#include <fis_mmu.h>
#include <pagine_virtuali.h>
#include <debug.h>

/*
struct mem_desc{
        dword pagine_virtuali_libere[32768];
        page_dir_entry* pd;   //page directory base 
	mem_link* inizio;        //link per la gestione delle pagine libere
};*/

void init_mmu(mem_desc*  desc){
        init_spazio_virtuale_libero(desc->pagine_virtuali_libere);
	init_spazio_virtuale(desc->pd);
	desc->inizio =  NULL;
}

void* alloca(size_t len, mem_desc*  desc){
        len +=sizeof(mem_link); //gli aggiungo la grandezza del link
        size_t pagine = len / 4096;
	if(len % 4096){
	        pagine++;
	}
	debug << "alloca: devo allocare " << len << " byte per una quantità di pagine pari a " << pagine << endl;
	void *addr = prendi_spazio_virtuale(pagine, desc->pagine_virtuali_libere);
	setta_spazio_virtuale(desc->pd, addr, pagine);
	mem_link* link = desc->inizio;
	if(!link){       //se è la prima volta
	        link = (mem_link*) addr;
		link->size = pagine*4096;
		link->succ = 0;
	}else{
                while(link->succ)       
		        link = link->succ;
		mem_link* link2 =(mem_link*) addr;
		link2->size = pagine*4096;
		link2->succ = 0;
		link->succ = link2;                
	}        
	return (void *)((char*)addr + sizeof(mem_link));
}
