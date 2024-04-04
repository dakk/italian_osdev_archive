/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: keyboard.h			|
	| Date: 31/12/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_DRIVER_KEYBOARD_H_
#define _HARDWARE_DRIVER_KEYBOARD_H_

/*
	**************************
	**	 Include	**
	**************************
*/
#include <kdef.h>

#define BUF_SIZE 255

namespace hardware{

	namespace driver{
	
		namespace keyboard{
		
			class Keyboard{
			public:
				static void Init(); //Inizializzo i driver della tastiera
				
				static void BufWrite(int scanc); //Scrivo un scancode nel buffer
				static int BufRead(); //Leggo un scancode dal buffer
				static char getChar(); //Leggo un carattere
				static void getStr(char *str, size_t len); //Leggo una stringa
			private:
				static void KeyboardISR(); //Gestore dell'IRQ della tastiera (Interrupt Sevice Routine)

				static int buf[BUF_SIZE]; //Buffer
				static int pos; //Indice del carattere nel bufer
			};
		}
	}
}

#endif
