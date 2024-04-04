#include <kmmu.h>
#include <paging.h>
#include <fis_mmu.h>
#include <video.h>

page_dir_entry *kernel_dir;
/*
namespace{
     //bisogna ricordare che ogni directory occupa 4k (1 pagina) e ogni tabella anche
    page_dir_entry* kernel_dir;
//    page_table_entry* kernel_table;
}*/

void init_kmmu()
{
	page_table_entry *current_table;

	kout << "\ninizializzo la paginazione" << endl;

	//prendo lo spazio per la directory del ccs + kernel
	kernel_dir =
		(page_dir_entry *) prendi_spazio_fisico((void *) 0,
							(void
							 *) (sizeof
							     (page_dir_entry)
							     * 1024));

	kout << "la directory del kernel inizia a " << (dword) kernel_dir <<
		endl;

	//prendo la prima tabella del kernel, quella che mapperà i primi 4 mega virtuali
	current_table =
		(page_table_entry *) prendi_spazio_fisico((void *) 4096,
							  (void *) (4096 +
								    sizeof
								    (page_table_entry)
								    * 1024));

	kout << "la prima tabella del kernel inizia a " << (dword)
		current_table << endl;

	//imposta la prima entry nella tabella del kernel
	kernel_dir[0] =
		crea_page_dir(current_table, SCRITTURA_LETTURA | PRESENTE);

	//marco tutte le altre dir come non presenti
	for (int i = 1; i < 1024; i++)
		kernel_dir[i] = 0;

	//definiamo il primo mega come presente e lo mappiamo nello spazio del ccs
	dword addr =
		(dword) prendi_spazio_fisico((void *) 0, (void *) (1 << 20));
	for (int i = 0; i < 256; i++) {
		current_table[i] =
			crea_page_table((page_base *) addr,
					SCRITTURA_LETTURA | PRESENTE);
		addr += 4096;
	}

	//definisco i restanti 3 mega come non presenti
	for (int i = 256; i < 1024; i++) {
		current_table[i] = 0;
	}
	dword control = (dword) kernel_dir;
	control &= 0xFFFFF000;
	asm volatile ("mov %0, %%cr3"::"r" (control));
//          kout << "\nsettato cr3" << endl;
	abilita_hardware_paging();

}
