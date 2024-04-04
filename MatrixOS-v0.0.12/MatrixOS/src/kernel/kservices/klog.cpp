/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: klog.cpp			|
	| Date: 09/11/2004			|
	| Authors: Davide B.			|
	| License: GNU General Public License	|
	+---------------------------------------+
*/

#include <kservices/klog.h>

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


void KLog::WriteLine(){
	Video::NewLine(); //Nuova riga
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

	Video::Print(c);
}

/*
Scrive una stringa o un carattere e poi torna a capo
*/
void KLog::WriteLine(const char *c){

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
void KLog::Write(int value){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value);
	Video::Print(tmpString);
}

/*
Scrive un numero intero e poi torna a capo
*/
void KLog::WriteLine(int value){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value);
	Video::Print(tmpString);
	
	Video::NewLine(); //Nuova riga
}

/*
Scrive un numero intero in una base diversa da 10
*/
void KLog::Write(int value, kframework::Base base){

	char tmpString[255];
	
	kframework::string::Util::toString(tmpString, 255, value, base);
	Video::Print(tmpString);
}

/*
Scrive un numero intero in una base diversa da 10 e poi torna a capo
*/
void KLog::WriteLine(int value, kframework::Base base){

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
void KLog::Write(bool value){

	if (value == true){
		Video::Print("True");
	}else{
		Video::Print("False");
	}
}

/*
Scrive un valore booleano (TRUE, FALSE) e poi torna a capo
*/
void KLog::WriteLine(bool value){

	if (value == true){
		Video::Print("True\n");
	}else{
		Video::Print("False\n");
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
	
	kframework::string::Util::toString(tmpString, 3, (int) value);
	Video::Print(tmpString);
}

/*
Scrive un byte e poi torna a capo
*/
void KLog::WriteLine(byte value){

	char tmpString[3];
	
	kframework::string::Util::toString(tmpString, 3, (int) value);
	Video::Print(tmpString);
	
	Video::NewLine(); //Nuova riga
}

/*
Scrive un byte in una base diversa da 10
*/
void KLog::Write(byte value, kframework::Base base){

	char tmpString[10];
	
	kframework::string::Util::toString(tmpString, 3, (int) value, base);
	Video::Print(tmpString);
}

/*
Scrive un byte in una base diversa da 10 e poi torna a capo
*/
void KLog::WriteLine(byte value, kframework::Base base){

	char tmpString[10];
	
	kframework::string::Util::toString(tmpString, 3, (int) value, base);
	Video::Print(tmpString);
	
	Video::NewLine(); //Nuova riga
}
