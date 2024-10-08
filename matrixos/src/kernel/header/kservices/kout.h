/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: kout.cpp			|
	| Date: 09/11/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

/*
	**************************
	**	 Include	**
	**************************
*/

#ifndef _KSERVICES_KOUT_H_
#define _KSERVICES_KOUT_H_

#include <kdef.h>
#include <kframework/kframework.h>

namespace kservices
{
    class KOut{
    public:
	static void WriteLine(); //Scrive una nuova linea
	
	static void Write(const char *c); //Scrive una stringa o un carattere
	static void WriteLine(const char *c); //Scrive una stringa o un carattere e poi torna a capo

	static void Write(unsigned int value); //Scrive un numero intero
	static void WriteLine(unsigned int value); //Scrive un numero intero e poi torna a capo
	static void Write(unsigned int value, kframework::Base base); //Scrive un numero intero in una base diversa da 10
	static void WriteLine(unsigned int value, kframework::Base base); //Scrive un numero intero in una base diversa da 10 e poi torna a capo 
		
	static void Write(bool value); //Scrive un valore booleano (TRUE, FALSE)
	static void WriteLine(bool value); //Scrive un valore booleano (TRUE, FALSE) e poi torna a capo
		
	static void Write(byte value); //Scrive un byte
	static void WriteLine(byte value); //Scrive un byte e poi torna a capo
	static void Write(byte value, kframework::Base base); //Scrive un byte in una base diversa da 10
	static void WriteLine(byte value, kframework::Base base); //Scrive un byte in una base diversa da 10 e poi torna a capo
    };
}

#endif
