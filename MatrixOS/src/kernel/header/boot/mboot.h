/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: mboot.h			|
	| Date: 01/09/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _BOOT_MBOOT_H_
#define _BOOT_MBOOT_H_


/*
	**************************
	**	 Include	**
	**************************
*/

#include <kdef.h>
#include <boot/multiboot.h>
#include <hardware/driver/initconsole.h>

namespace boot{

	class mboot{
	public:
		static unsigned long long Init(unsigned long magic,multiboot_info * info); //Inizializza la classe e controlla se il kernel e' stato avviato da un bootloader multiboot

		static unsigned long getFlags(); //Legge le flags

		static unsigned long long getMagic(); //Legge il magic number del multiboot

		static unsigned long getMemLower(); //Legge la quantità di memoria bassa
		static unsigned long getMemUpper(); //Legge la quantita' di Memoria alta

		static unsigned long getBootDevice(); //Determina il device da cui è stato fatto il boot
		static unsigned long getVBEModeInfoBlock(); //Restituisce un puntatore all VBEModeInfoBlock
		static hardware::driver::video::console_type mboot::getGraphicMode(); //Determino il tipo di console.
	private:
		static unsigned long mboot_magic; //Multiboot magic number
		static multiboot_info *mboot_info; //Blocco informazioni multiboot
	};
}

#endif
