/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: floppy.h			|
	| Date: 25/04/2005			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_DRIVER_FLOPPY_H_
#define _HARDWARE_DRIVER_FLOPPY_H_

#include <kdef.h>

namespace hardware{

	namespace driver{

		class Floppy{
		public:
			static void Init(); //Inizializzo il driver del Floppy

			static void PrintInfo(byte floppyInfo); //Scrivo le informazioni sul drive floppy
			static void Reset(); //Resetto il controller del floppy
			static void MotorOFF(); //Spengo il motore
			static void MotorON(); //Accendo il motore
			static bool Seek(int track); //Vado a una certa traccia.
			static bool BlockIO(int block, int blockn, byte *blockbuffer, bool read); //Fa un'operazione di input o di output di n blocchi
			static bool ReadBlock(int block, int blockn, byte *blockbuffer); //Leggo un blocco
			static bool WriteBlock(int block, int blockn, byte *blockbuffer); //Scrivo un blocco
		private:
			static void FloppyISR();
			static bool WaitFDC(bool sense_interrupt); //Attendo un IRQ 6 (ovvero che sia completata un'operazione)
			static void Recalibrate(); //Ricalibro il floppy
			static int SendByte(byte Byte); //Invio un byte al controller
			static int GetByte(); //Leggo un byte dal controller
			static bool done; //L'operazione di IO è stata completata
			static byte status[7]; //Stato
			static byte sr0; //Registro sr0
			static byte track; //Traccia corrente
			static void block2hts(int block, int *head, int *track, int *sector); //Converte il blocco nella tripletta head/track/sector

		};
	}
}

#endif
