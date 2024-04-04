/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: klog.cpp			|
	| Date: 09/11/2004			|
	| Authors: Davide Bettio		|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

#include <kservices/klog.h>

#include <hardware/driver/initconsole.h>
#include <kframework/kframework.h>
#include <kframework/stringutil.h>

#include <kdef.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace kservices;
using namespace hardware::driver::video;


void KLog::WriteLine(){
	InitConsole::NewLine();
}

/*
	##########################
	##	  Char		##
	##########################
*/

/*
Scrive una stringa o un carattere
*/
void KLog::Write(const char *c){

	InitConsole::Print(c);
}

/*
Scrive una stringa o un carattere e poi torna a capo
*/
void KLog::WriteLine(const char *c){

	InitConsole::Print(c);

	InitConsole::NewLine();
}


/*
	##########################
	##	   Int		##
	##########################
*/

/*
Scrive un numero intero
*/
void KLog::Write(unsigned int value){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value);

	InitConsole::Print(tmpString);
}

/*
Scrive un numero intero e poi torna a capo
*/
void KLog::WriteLine(unsigned int value){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value);

	InitConsole::Print(tmpString);

	InitConsole::NewLine();
}

/*
Scrive un numero intero in una base diversa da 10
*/
void KLog::Write(unsigned int value, kframework::Base base){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value, base);

	InitConsole::Print(tmpString);
}

/*
Scrive un numero intero in una base diversa da 10 e poi torna a capo
*/
void KLog::WriteLine(unsigned int value, kframework::Base base){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value, base);
	InitConsole::Print(tmpString);	

	InitConsole::NewLine(); //Nuova riga
}

/*
	##########################
	##	   bool		##
	##########################
*/

/*
Scrive un valore booleano (TRUE, FALSE)
*/
void KLog::Write(bool value){

	if (value == true){
		InitConsole::Print("True");
	}else{
		InitConsole::Print("False");
	}
}

/*
Scrive un valore booleano (TRUE, FALSE) e poi torna a capo
*/
void KLog::WriteLine(bool value){

	if (value == true){
		InitConsole::Print("True\n");
	}else{
		InitConsole::Print("False\n");
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
void KLog::Write(byte value){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (unsigned int) value);

	InitConsole::Print(tmpString);
}

/*
Scrive un byte e poi torna a capo
*/
void KLog::WriteLine(byte value){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (unsigned int) value);

	InitConsole::Print(tmpString);	

	InitConsole::NewLine(); //Nuova riga
}

/*
Scrive un byte in una base diversa da 10
*/
void KLog::Write(byte value, kframework::Base base){

	char tmpString[10];
	
	kframework::string::Util::toString(tmpString, 10, (unsigned int) value, base);

	InitConsole::Print(tmpString);
}

/*
Scrive un byte in una base diversa da 10 e poi torna a capo
*/
void KLog::WriteLine(byte value, kframework::Base base){

	char tmpString[10];
	
	kframework::string::Util::toString(tmpString, 10, (unsigned int) value, base);

	InitConsole::Print(tmpString);	

	InitConsole::NewLine(); //Nuova riga
}
