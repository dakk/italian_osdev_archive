#include <paging.h>
#include <debug.h>
#include <video.h>

page_dir_entry crea_page_dir(page_table_entry * pte, dword flag)
{
	dword addr = (dword) pte;
	page_dir_entry pde;
	addr &= 0xFFFFF000;
	flag &= 0xFFF;
	pde = addr + flag;
	return pde;
}


page_table_entry crea_page_table(page_base * page, dword flag)
{
	dword addr = (dword) page;
	page_table_entry pte;
	addr &= 0xFFFFF000;
	flag &= 0xFFF;
	pte = addr + flag;
	return pte;
}

page_table_entry *get_table_addr(page_dir_entry pde)
{
	return (page_table_entry *) (pde & 0xFFFFF000);
}

page_base *get_page_addr(page_table_entry pte)
{
	return (page_base *) (pte & 0xFFFFF000);
}

//vede se una pagina è presente oppure no
bool pagina_presente(page_table_entry pte)
{
	if (pte & PRESENTE)
		return true;
	return false;
}

//vede se una tabella è presente oppure no
bool table_presente(page_dir_entry pde)
{
	if (pde & PRESENTE)
		return true;
	return false;
}

void set_present(page_table_entry * pte)
{
	*pte |= PRESENTE;
}

#define flush_tlb_one(addr) \
	__asm__ __volatile__("invlpg %0": :"m" (*(char *) addr))

void set_assent(page_table_entry * pte)
{
	debug << "set_assent: l'indirizzo dell'elemento che sto modificando è: " << (dword) pte << endl;
	*pte &= ~PRESENTE;
	flush_tlb_one(get_page_addr(*pte));
	/*asm("pushl %eax \n"
	 * "mov %cr3, %eax      \n"
	 * "mov %eax, %cr3      \n"
	 * "popl %eax   \n"); */
}

void print_paging(page_dir_entry * pde)
{
	int i, j;
	page_dir_entry *dir = pde;	//get_table_addr(pde);
	page_table_entry *pte;
	debug << "\nLa dir è: " << (dword) dir;
	for (i = 0; i < 1024; i++) {
		if (table_presente(dir[i])) {
			debug << "\ndir[" << i << "] =  " << (dword)
				get_table_addr(dir[i]);
			pte = get_table_addr(dir[i]);
			for (j = 0; j < 1024; j++) {
				if (pagina_presente(pte[j]))
					debug << "\n" << ((dword) (pte + j))
						<< "  table[" << j << "] =  "
						<< (dword)
						get_page_addr(pte[j]);
			}
		}
	}
}

//cerca la prima pagina libera nella tabella e la restituisce. Se non ci sono + pagine libere viene restituito <<1024>>
size_t prendi_prima_pagina_libera(page_table_entry * table)
{
	size_t i;
	for (i = 0; pagina_presente(table[i]) && (i < 1024); i++);
	if (i != 1024)
		set_present(&table[i]);
	return i;
}

void abilita_hardware_paging()
{
	dword control;
	debug << "paging abilitato....";
	asm volatile ("mov %%cr0, %0":"=r" (control));
	control |= 0x80000000;
	asm volatile ("mov %0, %%cr0"::"r" (control));
	debug << "OK" << endl;
}


void disabilita_hardware_paging()
{
	dword control;
	debug << "paging disabilitato....";
	asm volatile ("mov %%cr0, %0":"=r" (control));
	control &= ~0x80000000;
	asm volatile ("mov %0, %%cr0"::"r" (control));
	debug << "OK" << endl;
}
