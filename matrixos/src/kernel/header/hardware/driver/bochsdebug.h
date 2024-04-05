/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: bochsdebug.h		|
	| Date: 23/11/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_DRIVER_BOCHSDEBUG_H_
#define _HARDWARE_DRIVER_BOCHSDEBUG_H_

/*
	**************************
	**	 Include	**
	**************************
*/

#include <kdef.h>

namespace hardware{

	namespace driver{

		namespace vm{

			enum TraceStatus{
				tsOFF, //Disabilita il trace
				tsON //Abilita il trace
			};
		
			class BochsDebug{
			public:
				static void Init(); //Inizializza il debug di bochs
				
				static void OutputMsg(const char c); //Scrive un carattere nel output di bochs
				static void OutputMsg(const char *c); //Scrive una stringa nel output di bochs
				
				static void IstructionTrace(TraceStatus status); //Setta lo stato del trace delle istruzioni
				static void RegisterTrace(TraceStatus status); //Setta lo stato del trace dei registri
				
				static void Stop(); //Ferma il debug
			};
		
		}
	}
}

#endif
