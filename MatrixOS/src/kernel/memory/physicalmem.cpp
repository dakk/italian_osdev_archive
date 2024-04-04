/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: physicalmem.cpp		|
	| Date: 18/09/2005			|
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
#include <boot/mboot.h>
#include <boot/multiboot.h>
#include <kservices/kout.h>
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

dword PhysicalMem::pageframebitmap[32768];
dword PhysicalMem::pagenumber;
dword PhysicalMem::freepagenumber;

void PhysicalMem::Init(){
	//Setto a 0 tutti i bit della bitmap
	for (dword i = 0; i < 32768; i++) pageframebitmap[i] = 0;

	//Calcolo il numero di pagine
	pagenumber = ((boot::mboot::getMemUpper() + 1024) * 1024) / 4096;
	freepagenumber = pagenumber;

	kservices::KOut::Write("Page number: ");
	kservices::KOut::WriteLine(pagenumber);

	//Alloco il primo megabyte di memoria (ovvero 256 pagine) occupato dal bios e da altre cose..
	for(int i = 0; i < 256*4; i++) set_bit(i, pageframebitmap);
	freepagenumber -= 256*4;

	//TODO: Alloco la memoria occupa dal kernel
	
	//TODO: Alloco le mappe di memoria passate da GRUB

	kservices::KOut::WriteLine("Physical memory manager enabled");
}

void *PhysicalMem::PageFrameAlloc(){
	size_t page = find_first_zero_bit(pageframebitmap, pagenumber);
	set_bit(page, pageframebitmap);
	freepagenumber--;

	return (void *)(page*4096);
}

void *PhysicalMem::PageFrameFree(void *address){
	size_t page = (size_t) address / 4096;
	clear_bit(page, pageframebitmap);
	freepagenumber++;

	return (void *) 0;
}

dword PhysicalMem::getPageNumber(){
	return pagenumber;
}

dword PhysicalMem::getFreePageNumber(){
	return freepagenumber;
}
