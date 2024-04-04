/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: virtualmem.h		|
	| Date: 00/00/2005			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _MEMORY_VIRTUALMEM_H_
#define _MEMORY_VIRTUALMEM_H_

#include <kdef.h>

namespace memory{

	class VirtualMem{
	public:
		static void Init(); //Iniziallizo il gestore della memoria

		static void *kmalloc(unsigned int size, int priority); //Alloco la memoria. Priority indica la priorità.
		static void *kfree(void *address); //Libero la memora. NON È STATA IMPLEMENTATA.
	private:
		static dword bitmap[32768]; //BITMAP delle pagine occupate
	};
}

#endif
