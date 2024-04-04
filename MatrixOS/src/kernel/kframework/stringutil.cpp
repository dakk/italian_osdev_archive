 /*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: stringutil.cpp		|
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

#include <kframework/stringutil.h>
#include <kframework/kframework.h>

#include <kdef.h>
#include <string.h>


/*
	**************************
	**	 Namespace	**
	**************************
*/

using namespace kframework::string;


	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////



/*
Converte una numero in una stringa. size indica la dimensione della stringa.
*/
void Util::toString(char string[], unsigned int size, unsigned int value){

	unsigned int i = 0;
	unsigned int div = 1;

	//conta il numero di cifre presenti nel numero
	do{
		i++;
		div *= 10;        
	}while( (value / div) != 0 );
    
	//se il numero di cifre è minore o uguale a quelle che possono essere memorizzate
	if(i <= size){
		string[i--] = '\0';
		
        	do{
			string[i--] = (value % 10) +'0';
			value /= 10;
		}while( value != 0 && i >= 0 );
	}else{
        	string[0] = '\0';
	}
}


/*
Converte una numero in una stringa. size indica la dimensione della stringa, e base indica la base del numero (BIN,OCT,DEC,HEX)
*/

void Util::toString(char string[], unsigned int size, unsigned int value, kframework::Base base){

	unsigned int i = 1;
	unsigned int div = 1;
    
	//conta il numero di cifre presenti nel numero
	do{
		i++;
		div *= base;        
	}while( (value / div) != 0);

    
	//se il numero di cifre è minore o uguale a quelle che possono essere memorizzate
	if(i <= size){
		string[i--] = '\0';
		string[i--] = (base == kframework::HEX)? 'h' : (base == kframework::DEC)? 'd' : (base == kframework::OCT)? 'o' : 'b';
		
        	do{
			if( (base != kframework::HEX) || ( (base == kframework::HEX) && ((value % base) < 10) ) ){
				string[i--] = (value % base) + '0';
			}else{
                		string[i--] = (value % base) - 10 + 'A';
			}

			value /= base;
			
		}while(value!=0 && i>=0);

	}else{
		string[0]= '\0';
	}
}

/*
Converte una numero in una stringa. size indica la dimensione della stringa.
*/
//TODO: Implementare
void Util::toString_(char string[], unsigned int size, unsigned long long value){
/*
	unsigned long long  li = 0;
	unsigned long long ldiv = 1;
	unsigned int lvalue = (unsigned int)(value >> 32) 
	unsigned int lvalue2 = (unsigned int)(value >> 32)

	//conta il numero di cifre presenti nel numero
	do{
		li++;
		ldiv *= 10;        
	}while( (value / ldiv) != 0 );
    
	//se il numero di cifre è minore o uguale a quelle che possono essere memorizzate
	if(li <= size){
		string[li--] = '\0';
		
        	do{
			string[li--] = (value % 10) +'0';
			value /= 10;
		}while( value != 0 && li >= 0 );
	}else{
        	string[0] = '\0';
	} */
}


/*
Converte una numero in una stringa. size indica la dimensione della stringa, e base indica la base del numero (BIN,OCT,DEC,HEX)
*/
//TODO: Implementare
void Util::toString_(char string[], unsigned int size, unsigned long long value, kframework::Base base){
/*
	unsigned long long i = 1;
	unsigned long long div = 1;
    
	//conta il numero di cifre presenti nel numero
	do{
		i++;
		div *= base;        
	}while( (value / div) != 0);

    
	//se il numero di cifre è minore o uguale a quelle che possono essere memorizzate
	if(i <= size){
		string[i--] = '\0';
		string[i--] = (base == kframework::HEX)? 'h' : (base == kframework::DEC)? 'd' : (base == kframework::OCT)? 'o' : 'b';
		
        	do{
			if( (base != kframework::HEX) || ( (base == kframework::HEX) && ((value % base) < 10) ) ){
				string[i--] = (value % base) + '0';
			}else{
                		string[i--] = (value % base) - 10 + 'A';
			}

			value /= base;
			
		}while(value!=0 && i>=0);

	}else{
		string[0]= '\0';
	}
*/
}

/*
Converte un bool in una stringa. size indica la dimensione della stringa.
*/
void Util::toString(char string[], unsigned int size, bool value){

	string[0] = '\0';

	if (value == true){
		strncat(string, "True", size);
	}else{
		strncat(string, "False", size);
	}

}

