/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: virtualmem.cpp		|
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

#include <memory/virtualmem.h> 

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


//BITMAP delle pagine occupate
dword VirtualMem::bitmap[32768];

/*
Iniziallizo il gestore della memoria
*/
void VirtualMem::Init(){

	for (unsigned int i = 0; i<16; i++){
		bitmap[i] = 0xFFFFFFFF;
	}
	
	for (unsigned int i = 16; i<32768; i++){
		bitmap[i] = 0x00000000;
	}
}

/*
Alloco la memoria. Priority indica la priorità.
*/
void *VirtualMem::kmalloc(unsigned int size, int priority){

	unsigned int pagenum = (size / 4096) + 1;
	unsigned int pagecontinue = 0;
	int bit = 0;
	int i = 0;
	int j = 0;
	unsigned long int address; 
	
	for (i = 16; i<32768; i++){
		
		while(j<32){
			
			bit = bitmap[i] & (1<<i);
			
			if (bit == 0){
			
				pagecontinue++;
				
				if (pagecontinue == pagenum) break;
				
				j++;
			}else{
				j++;
				pagecontinue = 0;
			}
		}
		
		if (pagecontinue == pagenum) break;
		
		j=0;
	}
	
	if (pagecontinue != pagenum) return NULL;

	address = ((i*32+j+1)-pagecontinue)*4096;
	
	i = (address/4096)/32;
	j = (address/4096)%32;
	
	pagecontinue = 0;
	
	while (pagecontinue < pagenum){
		bitmap[i] = (bitmap[i] | (1<<j));
		j++;
		pagecontinue++;
		
		if (j==32 && pagecontinue < pagenum){
			i++;
			j=0;
		}
	}
	
	return (void*)address;
}

/*
Libero la memora. NON È STATA IMPLEMENTATA.
*/
void *VirtualMem::kfree(void *address){
	return (void *) 0;
}
