/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: physicalmem.h		|
	| Date: 00/00/2005			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _MEMORY_PHYSICALMEM_H_
#define _MEMORY_PHYSICALMEM_H_

#include <kdef.h>

namespace memory{

	class PhysicalMem{
	public:
		static void Init(); //Inizializzo il gestore della memoria fisica
		static unsigned long getMemSize(); //Leggo la dimensione della memoria
		static unsigned int getPageNum(); //Leggo il numero di pagine
	private:
		static void MemSize(); //Calcolo la memoria fisica disponibile
		static unsigned long memsize; //Dimensione della memoria in byte. Sostituire con qword per le architetture a 64 bit.
		static unsigned int pagenum; //Numero di pagine disponibili. Utilizzando unsigned int sono supportati 16 Tb.
	};
}

#endif
