/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: stdlib.cpp			|
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

#include <kdef.h>
#include <stdlib.h>
#include <memory/virtualmem.h>

/*
	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////
*/

extern "C"{
	
	void *malloc(size_t dim){
		return memory::VirtualMem::kmalloc((unsigned int) dim, 0);
	}
	
	void free(void * addr){
		memory::VirtualMem::kfree(addr);
	}
}
