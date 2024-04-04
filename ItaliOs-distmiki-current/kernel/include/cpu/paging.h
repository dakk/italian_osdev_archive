#ifndef _PAGING_H_
#define _PAGING_H_

#include <cstddef>


enum{
        PRESENTE = 1,
        NON_PRESENTE =  0,
        KERNEL = 0,
        USER  = 4,
        SOLA_LETTURA = 0,
	SCRITTURA_LETTURA = 2
};

typedef dword page_dir_entry;
typedef dword page_table_entry;
typedef dword page_base;//sono solo 20 bit  gli altri vengono ignorati

//crea la exntry da inserire nella directory per la tabella pte
page_dir_entry crea_page_dir(page_table_entry*  pte, dword flag);

//crea una entry da inserire nella tabella per descrivere una pagina
page_table_entry crea_page_table(page_base*  pte, dword flag);

bool pagina_presente(page_table_entry pte);
bool table_presente(page_dir_entry pde);
void set_present(page_table_entry *pte); 
void set_assent(page_table_entry * pte);
size_t get_page(page_table_entry* table);
page_table_entry* get_table_addr(page_dir_entry pde);
page_base* get_page_addr(page_table_entry pte);

void abilita_hardware_paging();
void disabilita_hardware_paging();

void  print_paging(page_dir_entry *pde);


#endif
