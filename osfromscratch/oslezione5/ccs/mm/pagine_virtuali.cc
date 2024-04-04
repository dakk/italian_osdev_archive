#include <pagine_virtuali.h>
#include <paging.h>
#include <debug.h>
#include <fis_mmu.h>

void init_spazio_virtuale(page_dir_entry* dir){
        dir =(page_dir_entry*) prendi_pagina_fisica();
	debug << "ho trovato la pagine fisica  " << (dword)dir << endl;
	//debo mappare la directoruy in qualkè pagina. altrimenti non riesco a settarla
	//disabilito il paging

        disabilita_hardware_paging();
	for(size_t i=0; i < 1024; i++)
	        dir[i] = 0;
	abilita_hardware_paging();
	//lo riabilito
}

void setta_spazio_virtuale(page_dir_entry* dir, void* linear_address, size_t pagine){
        byte *addr = (byte *)linear_address;   
        dword pde, pte, pg;	
	
	disabilita_hardware_paging();
	for(size_t i = 0; i < pagine; i++, addr+=4096){
	      pde = (dword)addr >> 22;
	      pte = ((dword)addr >> 12) & 0x3FF;  
	      
	      debug << "setta_spazio_virtuale:   Sto per impostare la dir " << pde << " e la table " << pte << endl;
              
	      //controllo se il dir entry è presente
	      if(!table_presente(dir[pde])){
	           page_dir_entry*  current_dir =(page_dir_entry*) prendi_pagina_fisica();
	           //non è presente quindi alloco una bella tabella
		   dir[pde] =    crea_page_dir(current_dir, SCRITTURA_LETTURA | PRESENTE);
		   
		   //imposto in modo che non ci siano tabelle disponibili
		   for(int k=0; k < 1024; k++)
		        current_dir[k] = 0;
	      }
	      
	      page_table_entry*  table =(page_table_entry*) get_table_addr(dir[pde]);
	      
	      //controlo se il pte esiste
      	      if(!pagina_presente(table[pte])){
	           page_table_entry*  current_table =(page_table_entry*) prendi_pagina_fisica();
		   
	           //non è presente quindi alloco le pagine
		   table[pte] =    crea_page_table(current_table, SCRITTURA_LETTURA | PRESENTE);
	      }
	}	
	abilita_hardware_paging();
}
