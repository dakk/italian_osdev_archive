/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: paging.cpp			|
	| Date: 23/09/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <memory/physicalmem.h>
#include <hardware/cpu/bitops.h>
#include <memory/paging.h>
#include <kservices/kout.h>
#include <hardware/cpu/idt.h>
#include <kservices/kdbg.h>

#include <kdef.h>


/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace memory;


/*
	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////
*/

extern "C"{
	extern void Int14Handler(void);
	extern void PageFaultISR(dword fault_address, dword error_code);
}

dword *page_directory;
dword *first_mb_pagetable; 

void Paging::Init(){
	//Aggiungo l'handler alla IDT
	hardware::cpu::IDT::Add(Int14Handler, 14);
	
	//Alloco le pagine fisiche della pagedir e della page table
	page_directory = (dword *)PhysicalMem::PageFrameAlloc();
	first_mb_pagetable = (dword *)PhysicalMem::PageFrameAlloc();

	//Mappo i primi 4 megabyte di memoria
	for (int i = 0; i < 1024; i++) first_mb_pagetable[i] = NewPTE((void *) (i*4096), KERNEL_STD_PAGE);

	//Riempio la pagedirectory
	for(int i = 0; i < 1024; i++) page_directory[i] = NewPDE((dword *) 0x00, KERNEL_NOPRESENT_STD_PAGE);

	//Aggiungo le entry della page table dei primi 4 mb
	page_directory[0] = NewPDE(first_mb_pagetable, KERNEL_STD_PAGE);
	//Aggiungo la pagedirectory. In questo modo ottengo la paginazione ticorsiva.
	page_directory[1023] = NewPDE(page_directory, KERNEL_STD_PAGE);

	//Scrivo in cr3 l'indirizzo della pagedir
	setCR3((dword) page_directory);
	//Abilito il paging
	Enable();

	//Faccio puntare a page_directory l'indirizzo 0xFFFFF000 che corrisponde
	//a quello della page directory nella paginazione ricorsiva.
	page_directory = (dword *) 0xFFFFF000;
}

/*
  Abilito il paging
*/
void Paging::Enable(){
	volatile dword cr0reg;
	//Leggo Il valore di CR0
	asm volatile("movl %%cr0,%0": "=r"(cr0reg));
	//Setto il bit 31 (paging)
	cr0reg |= 0x80000000;
	//Scrivo in cr0 e abilito il paging
	asm volatile("movl %0, %%cr0\n": :"r"(cr0reg));
}

/*
  Disabilito il paging
*/
void Paging::Disable(){	
	volatile dword cr0reg;
	//Leggo Il valore di CR0
	asm volatile("movl %%cr0,%0": "=r"(cr0reg));
	//Setto il bit 31 (paging)
	cr0reg |= 0x80000000;
	//Scrivo in cr0 e abilito il paging
	asm volatile("movl %0, %%cr0\n": :"r"(cr0reg));
}

/*
  Scrivo in cr3 l'indirizzo della pagedir
*/
void Paging::setCR3(dword cr3reg){
	asm volatile("movl %0, %%cr3\n" : : "r"(cr3reg));
}

/*
  Crea una nuova Page Table Entry a partire dal indirizzo di un page frame
*/
dword Paging::NewPTE(void *address, dword flags){
	dword tmp_pte = (dword) address;
	tmp_pte |= flags;

	return tmp_pte;
}

/*
  Crea una nuova Page Directory Entry a partire dal indirizzo di una page table
*/
dword Paging::NewPDE(dword *pagetable, dword flags){
	dword tmp_pde = (dword) pagetable;
	tmp_pde |= flags;

	return tmp_pde;
}

void Paging::PhyscalAlloc(dword addr, dword size){
	dword address = addr;
	for(dword i = 0; i < size; i += 1024){
		dword pde = address >> 22;
		dword pte = (address >> 12) & 0x3FF;

		if (test_bit(0, &page_directory[pde]) == 0){
			page_directory[pde] = Paging::NewPDE((dword *)PhysicalMem::PageFrameAlloc(), KERNEL_STD_PAGE);
		}
		dword *tab = (dword *) ((0x3FF << 22) + (pde << 12));
		if (test_bit(0, &tab[pte]) == 0 | address == (address & 0xFFFFF000)){
			tab[pte] = Paging::NewPTE((void *)(address & 0xFFFFF000), KERNEL_STD_PAGE);
		}
	address += 1024;
	}
}

/*bool Paging::PageIsPresent(dword page){

}
*/

extern "C"{
	/*
	  Funzione scritta in C per la gestione dei pagefault.
	  Deve essere chiamata da un handler scritto in ASM.
	*/
	void PageFaultISR(dword fault_address, dword error_code){
		dword pde = fault_address >> 22;
		dword pte = (fault_address >> 12) & 0x3FF;	

		//Stampo un msg di debug
		//kservices::KOut::Write("Error code:");
		//kservices::KOut::WriteLine(error_code);
	
		if (test_bit(0, &page_directory[pde]) == 0){
			page_directory[pde] = Paging::NewPDE((dword *)PhysicalMem::PageFrameAlloc(), KERNEL_STD_PAGE);
		}
		dword *tab = (dword *) ((0x3FF << 22) + (pde << 12));
		tab[pte] = Paging::NewPTE(PhysicalMem::PageFrameAlloc(), KERNEL_STD_PAGE);
	}

	/*
	  Handler scritto in ASM per l'interrupt 14.
	  Salva i registri e poi chiama la funzione scritta in C (PageFaultISR).
	*/
	asm(".globl Int14Handler	\n"
	    "Int14Handler:		\n"
	    "nop			\n"
	    "xchgl %eax,(%esp)		\n"
	    "pushl %ecx			\n"
	    "pushl %edx			\n"
	    "push %ds			\n"
	    "push %es			\n"
	    "push %fs			\n"
	    "movl $0x10,%edx		\n"
	    "mov %dx,%ds		\n"
	    "mov %dx,%es		\n"
	    "mov %dx,%fs		\n"
	    "movl %cr2,%edx		\n"
	    "pushl %eax			\n"
	    "pushl %edx			\n"
	    "call PageFaultISR		\n"
	    "addl $8,%esp		\n"
	    "pop %fs			\n"
	    "pop %es			\n"
	    "pop %ds			\n"
	    "popl %edx			\n"
	    "popl %ecx			\n"
	    "popl %eax			\n"
	    "iret			\n"
	);
}
