/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: physicalmem.cpp		|
	| Date: 00/00/2005			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <memory/physicalmem.h>
#include <kdef.h>

#define BASE_MEM 0x200000

#define OFFSET_MEM 0x400   //L'offset e' di 1k
#define PATTERN 0xAA55AA55  //Il primo valore che provo a scrivere
#define ANTI_PATTERN ~0xAA55AA55 //Il secondo valore che provo a scrivere

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


unsigned long PhysicalMem::memsize;
unsigned int PhysicalMem::pagenum;

void PhysicalMem::Init(){
	//Calcolo la memoria disponibile
	MemSize();
	
	//Calcolo il numero di pagine
	pagenum = memsize / 4096;
}

/*
asm("": : :"memory");//Fa in modo che il gcc non fa previsioni sul contenuto della memoria
*/
void PhysicalMem::MemSize(){
	unsigned int *mempointer;
	
	mempointer = (unsigned int *) BASE_MEM;
	
	asm("cli");
    
	while(1){
		*mempointer = PATTERN;
		asm("": : :"memory");
		 
		if (*mempointer != PATTERN) break;
		asm("": : :"memory");
		
		*mempointer = ~(*mempointer);
		asm("": : :"memory");
		 
		if (*mempointer != ANTI_PATTERN) break;
		asm("": : :"memory");
		
		mempointer += OFFSET_MEM;
	}
	
	asm("sti");
	
	memsize = (unsigned long)mempointer;
}

unsigned long PhysicalMem::getMemSize(){
	return memsize;
}

unsigned int PhysicalMem::getPageNum(){
	return pagenum;
}

void EnablePagining(){

}

void DisablePagining(){

}
