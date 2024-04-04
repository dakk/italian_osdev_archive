#include <paging.h>
#include <debug.h>

page_dir_entry crea_page_dir(page_table_entry * pte, dword flag){
    dword addr = (dword) pte;
    page_dir_entry pde;
    addr &= 0xFFFFF000;
    flag &= 0xFFF;
    pde = addr + flag;
    return pde;
}


page_table_entry crea_page_table(page_base * page, dword flag){
    dword addr = (dword) page;
    page_table_entry pte;
    addr &= 0xFFFFF000;
    flag &= 0xFFF;
    pte = addr + flag;
    return pte;
}

page_table_entry* get_table_addr(page_dir_entry pde){
        return (page_table_entry*)(pde &  0xFFFFF000);
}

page_base* get_page_addr(page_table_entry pte){
        return (page_base*)(pte &  0xFFFFF000);
}

//vede se una pagina è presente oppure no
bool pagina_presente(page_table_entry pte){
    if (pte & PRESENTE)
	return true;
    return false;
}

//vede se una tabella è presente oppure no
bool table_presente(page_dir_entry pde){
    if (pde & PRESENTE)
	return true;
    return false;
}

void set_present(page_table_entry * pte){
    *pte |= PRESENTE;
}

//cerca la prima pagina libera nella tabella e la restituisce. Se non ci sono + pagine libere viene restituito <<1024>>
size_t prendi_prima_pagina_libera(page_table_entry * table){
    size_t i;
    for(i = 0; pagina_presente(table[i]) && (i < 1024); i++);
    if(i != 1024)
        set_present(&table[i]);
    return i;
}

void abilita_hardware_paging(){
            dword control;
	    asm volatile ("mov %%cr0, %0":"=r"(control));
	    control |= 0x80000000;
	    asm volatile ("mov %0, %%cr0": :"r"(control));
	    debug << "paging abilitato" << endl;
}


void disabilita_hardware_paging(){
            dword control;
	    asm volatile ("mov %%cr0, %0":"=r"(control));
	    control &= ~0x80000000;
	    asm volatile ("mov %0, %%cr0": :"r"(control));
	    debug << "paging disabilitato" << endl;
}
