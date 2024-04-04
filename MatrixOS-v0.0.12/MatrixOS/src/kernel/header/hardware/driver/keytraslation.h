/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: keytraslation.h		|
	| Date: 31/12/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

#ifndef _HARDWARE_DRIVER_KEYTRASLATION_H_
#define _HARDWARE_DRIVER_KEYTRASLATION_H_

/*
	**************************
	**	  define	**
	**************************
*/

#define FRECCIA_ALTA ('A' + ('[' << 8) )
#define FRECCIA_DESTRA ('C' + ('[' << 8) )
#define FRECCIA_SINISTRA ('D' + ('[' << 8))
#define FRECCIA_BASSA ('B' + ('[' << 8) )
#define BACKSPACE  (8)
#define NEWLINE ('\n')
#define NO_VALID_SCANDODE -1


/*
	**************************
	**	 Include	**
	**************************
*/
#include <kdef.h>


namespace hardware{

	namespace driver{
	
		namespace keyboard{
		
			class KeyTraslation{
			public:
				static void InitMap(void);
				static int Scan2Ascii(int car);
			};
		}
	}
}
#endif
