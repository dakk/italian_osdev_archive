/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: dma.h			|
	| Date: 25/04/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_DRIVER_DMA_H_
#define _HARDWARE_DRIVER_DMA_H_

#include <kdef.h>

namespace hardware{

	namespace driver{

		class DMA{
		public:
			static void Init(); //Inizializzo il driver DMA
			static void StartDMA(byte DMAchannel, byte page, unsigned int offset, unsigned int lenght, byte mode); //Inizio un'operazione del DMA
			static void DMA::StartDMA(byte channel, unsigned long address, unsigned int length, bool read); //Inizio un'operazione del DMA
		private:
			//Dati registri
			static byte MaskReg[8];
			static byte ModeReg[8];
			static byte ClearReg[8];
			
			//Dati porte
			static byte PagePort[8];
			static byte AddrPort[8];
			static byte CountPort[8];
		};
	}
}

#endif
