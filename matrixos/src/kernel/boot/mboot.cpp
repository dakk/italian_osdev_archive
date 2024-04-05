/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: mboot.cpp			|
	| Date: 01/09/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

/*
	**************************
	**	 Include	**
	**************************
*/

#include <boot/mboot.h>
#include <hardware/cpu/bitops.h>
#include <boot/multiboot.h>

#include <kdef.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace boot;

//Multiboot magic number
unsigned long mboot::mboot_magic;
//Blocco informazioni multiboot
multiboot_info *mboot::mboot_info;

/*
  Inizializza la classe e controlla se il kernel e' stato avviato da un bootloader multiboot
*/
unsigned long long mboot::Init(unsigned long magic,multiboot_info * info){
	
	//Controllo se il kernel è stato avviato da un bootloader multiboot
	mboot_magic = magic;
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) return magic;

	mboot_info = info;
	return 0;
}

/*
  Legge le flags
*/
unsigned long mboot::getFlags(){
	return mboot_info->flags;
}

/*
  Legge il magic number del multiboot
*/
unsigned long long mboot::getMagic(){
	return mboot_magic;
}

/*
  Legge la quantità di memoria bassa
*/
unsigned long mboot::getMemLower(){
	return mboot_info->mem_lower;
}

/*
  Legge la quantita' di Memoria alta
*/
unsigned long mboot::getMemUpper(){
	return mboot_info->mem_upper;
}

/*
  Determina il device da cui è stato fatto il boot
*/
unsigned long mboot::getBootDevice(){
	return mboot_info->boot_device;
}

/*
  Restituisce un puntatore all VBEModeInfoBlock
*/
unsigned long mboot::getVBEModeInfoBlock(){
	unsigned long *a = (unsigned long *)((char *)(mboot_info) + 76);
	return *a;
}

/*
  Determino il tipo di console.
*/
hardware::driver::video::console_type mboot::getGraphicMode(){
	if (test_bit(11, &mboot_info->flags) == 1){
		return hardware::driver::video::vbeconsole;
	}else{
		return hardware::driver::video::video;
	}
}
