/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: kdbg.cpp			|
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

#include <kservices/kdbg.h>

#include <kdef.h>
#include <hardware/driver/bochsdebug.h>
#include <hardware/driver/vbeconsole.h>
#include <kframework/kframework.h>
#include <kframework/stringutil.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace kservices;
using namespace hardware::driver::video;
using namespace hardware::driver::vm;


void KDbg::WriteLine(){
	BochsDebug::OutputMsg('\n'); //Nuova riga
}

/*
	##########################
	##	  Char		##
	##########################
*/

/*
Scrive una stringa o un carattere
*/
void KDbg::Write(const char *c){
	BochsDebug::OutputMsg(c);
}

/*
Scrive una stringa o un carattere e poi torna a capo
*/
void KDbg::WriteLine(const char *c){

	BochsDebug::OutputMsg(c);
	BochsDebug::OutputMsg('\n'); //Nuova riga
}


/*
	##########################
	##	   Int		##
	##########################
*/

/*
Scrive un numero intero
*/
void KDbg::Write(unsigned int value){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value);
	BochsDebug::OutputMsg(tmpString);
}

/*
Scrive un numero intero e poi torna a capo
*/
void KDbg::WriteLine(unsigned int value){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value);
	BochsDebug::OutputMsg(tmpString);

	BochsDebug::OutputMsg('\n'); //Nuova riga
}

/*
Scrive un numero intero in una base diversa da 10
*/
void KDbg::Write(unsigned int value, kframework::Base base){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value, base);
	BochsDebug::OutputMsg(tmpString);
}

/*
Scrive un numero intero in una base diversa da 10 e poi torna a capo
*/
void KDbg::WriteLine(unsigned int value, kframework::Base base){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value, base);
	BochsDebug::OutputMsg(tmpString);
	
	BochsDebug::OutputMsg('\n'); //Nuova riga
}

/*
	##########################
	##	   bool		##
	##########################
*/

/*
Scrive un valore booleano (TRUE, FALSE)
*/
void KDbg::Write(bool value){

	if (value == true){
		BochsDebug::OutputMsg("True");
	}else{
		BochsDebug::OutputMsg("False");
	}
}

/*
Scrive un valore booleano (TRUE, FALSE) e poi torna a capo
*/
void KDbg::WriteLine(bool value){

	if (value == true){
		BochsDebug::OutputMsg("True\n");
	}else{
		BochsDebug::OutputMsg("False\n");
	}
}


/*
	##########################
	##	   byte		##
	##########################
*/

/*
Scrive un byte
*/
void KDbg::Write(byte value){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (unsigned int) value);
	BochsDebug::OutputMsg(tmpString);
}

/*
Scrive un byte e poi torna a capo
*/
void KDbg::WriteLine(byte value){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (unsigned int) value);
	BochsDebug::OutputMsg(tmpString);
	
	BochsDebug::OutputMsg('\n'); //Nuova riga
}

/*
Scrive un byte in una base diversa da 10
*/
void KDbg::Write(byte value, kframework::Base base){

	char tmpString[10];
	
	kframework::string::Util::toString(tmpString, 10, (unsigned int) value, base);
	BochsDebug::OutputMsg(tmpString);
}

/*
Scrive un byte in una base diversa da 10 e poi torna a capo
*/
void KDbg::WriteLine(byte value, kframework::Base base){

	char tmpString[10];
	
	kframework::string::Util::toString(tmpString, 10, (unsigned int) value, base);
	BochsDebug::OutputMsg(tmpString);
	
	BochsDebug::OutputMsg('\n'); //Nuova riga
}
