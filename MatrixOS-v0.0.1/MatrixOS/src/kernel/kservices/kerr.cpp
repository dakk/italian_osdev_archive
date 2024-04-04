/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: kerr.cpp			|
	| Date: 09/11/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/ 

/*
	**************************
	**	 Include	**
	**************************
*/

#include <kservices/kerr.h>

#include <kdef.h>
#include <hardware/driver/video.h>
#include <kframework/kframework.h>
#include <kframework/stringutil.h>

/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace kservices;
using namespace hardware::driver::video;

/*
	##########################
	##	  Char		##
	##########################
*/

/*
Scrive una stringa o un carattere
*/
void KErr::Write(const char *c){

	Video::Print(c);
}

/*
Scrive una stringa o un carattere e poi torna a capo
*/
void KErr::WriteLine(const char *c){

	Video::Print(c);
	Video::NewLine(); //Nuova riga
}


/*
	##########################
	##	   Int		##
	##########################
*/

/*
Scrive un numero intero
*/
void KErr::Write(int value){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value);
	Video::Print(tmpString);
}

/*
Scrive un numero intero e poi torna a capo
*/
void KErr::WriteLine(int value){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value);
	Video::Print(tmpString);
	
	Video::NewLine(); //Nuova riga
}

/*
Scrive un numero intero in una base diversa da 10
*/
void KErr::Write(int value, kframework::Base base){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value, base);
	Video::Print(tmpString);
}

/*
Scrive un numero intero in una base diversa da 10 e poi torna a capo
*/
void KErr::WriteLine(int value, kframework::Base base){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value, base);
	Video::Print(tmpString);
	
	Video::NewLine(); //Nuova riga
}

/*
	##########################
	##	   bool		##
	##########################
*/

/*
Scrive un valore booleano (TRUE, FALSE)
*/
void KErr::Write(bool value){

	if (value == true){
		Video::Print("True");
	}else{
		Video::Print("False");
	}
}

/*
Scrive un valore booleano (TRUE, FALSE) e poi torna a capo
*/
void KErr::WriteLine(bool value){

	if (value == true){
		Video::Print("True\n");
	}else{
		Video::Print("False\n");
	}
	
	Video::NewLine(); //Nuova riga
}


/*
	##########################
	##	   byte		##
	##########################
*/

/*
Scrive un byte
*/
void KErr::Write(byte value){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (int) value);
	Video::Print(tmpString);
}

/*
Scrive un byte e poi torna a capo
*/
void KErr::WriteLine(byte value){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (int) value);
	Video::Print(tmpString);
	
	Video::NewLine(); //Nuova riga
}

/*
Scrive un byte in una base diversa da 10
*/
void KErr::Write(byte value, kframework::Base base){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (int) value, base);
	Video::Print(tmpString);
}

/*
Scrive un byte in una base diversa da 10 e poi torna a capo
*/
void KErr::WriteLine(byte value, kframework::Base base){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (int) value, base);
	Video::Print(tmpString);
	
	Video::NewLine(); //Nuova riga
}
