/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: virtualmem.cpp		|
	| Date: 00/00/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#include <memory/virtualmem.h> 
#include <kservices/kout.h>

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


/*
Iniziallizo il gestore della memoria
*/
void VirtualMem::Init(){

}

/*
Alloco la memoria. Priority indica la priorità.
*/
void *VirtualMem::kmalloc(unsigned int size, int priority){
static int mem = 0x300000;
mem += 0x200000;
return (void*)mem;
}

/*
Libero la memora. NON È STATA IMPLEMENTATA.
*/
void *VirtualMem::kfree(void *address){
	return (void *) 0;
}
