/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: stringutil.h		|
	| Date: 14/11/2004			|
	| Authors: Enrico S.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#ifndef _KFRAMEWORK_STRINGUTIL_H_
#define _KFRAMEWORK_STRINGUTIL_H_

#include <kdef.h>
#include <kframework/kframework.h>

namespace kframework{

	namespace string{
	
		class Util{
		public:
			static void toString(char string[], int size, int value); //Converte una numero in una stringa. size indica la dimensione della stringa.
			static void toString(char string[], int size, int value, kframework::Base base); //Converte una numero in una stringa. size indica la dimensione della stringa, e base indica la base del numero (BIN,OCT,DEC,HEX
			static void toString(char string[], int size, bool value); //Converte un bool in una stringa. size indica la dimensione della stringa.
		};
	}
}

#endif
