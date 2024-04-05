/*
	+---------------------------------------+
	|		MatrixOS		|
	+---------------------------------------+
	| Filename: string.cpp			|
	| Date: 17/11/2004			|
	| Authors: Enrico S.			|
	| License: GNU General Public License	|
	+---------------------------------------+
	| Attenzione: Parte del codice fa parte	|
	|	del sistema operativo ItaliOS	|
	|	e del sistema operativo DreamOS	|
	+---------------------------------------+
*/



/*
	**************************
	**	 Include	**
	**************************
*/

#include <string.h>

/*
	//////////////////////////////////
	//	Inizio del Codice	//
	//////////////////////////////////
*/

extern "C"{

	/*
	Restituisce il numero di caratteri in una stringa
	*/
	int strlen(const char *str){
		int i=0;

		while(str[i] != '\0'){
			i++;
		}

		return i;
	}

	/*
	Concatena una stringa ad un'altra
	*/
	char *strcat(char *dest, const char *src){
		int i=0;
		int destSize = strlen(dest);

		while(src[i]!='\0'){
        		dest[destSize++] = src[i++];
		}

		dest[destSize] = '\0';

		return dest;
	}

	/*
	Incolla una stringa in un'altra
	*/
	char *strcpy(char *dest, const char *src){
		int i=0;

		while(src[i]!='\0'){
			dest[i++] = src[i];
	
		}
	
		dest[i] = '\0';

		return dest;
	}

	/*
	Confronta due stringhe e restituisce 0 se sono uguali,
	1 se la prima è più grande e -1 se è più piccola
	*/
	int strcmp(const char *str1, const char *str2){
		int i=0;

		while(str1[i]==str2[i]){
	
        		if(str1[i]=='\0'){ //entrambe valgono NULL
				return 0;
			}

			i++;
		}   

		if(i>0){
			--i;
		}

		if(str1[i]<str2[i]){
			return -1;
		}else{
			return 1;
		}
	}

	/*
	Attacca n char di una stringa ad un'altra
	*/
	char *strncat(char *dest, const char *src, int n){
		int i=0;
		int destSize = strlen(dest);

		while((src[i]!='\0') && (i<n)){
        		dest[destSize++] = src[i++];
		}

		dest[destSize] = '\0';

		return dest;
	}

	/*
	Incolla n char di una stringa in un'altra
	*/
	char *strncpy(char *dest, const char *src, int n){
		int i=0;

		while((src[i]!='\0') && (i<n)){
			dest[i++] = src[i];
		}

		dest[i] = '\0';

		return dest;
	}

	/*
	Confronta n char di due stringhe
	*/
	int strncmp(const char *str1, const char *str2, int n){
		int i=0;

		while((str1[i]==str2[i]) && (i<n)){
			if((i+1)==n){
				return 0;
			}
			
			i++;
		}

		if(i>0){
			--i;
		}

		if(str1[i]<str2[i]){
			return -1;
		}else{
        		return 1;
		}
	}


	/*
	Cerca un carattere in una stringa
	*/
	char *strchr(char *str, int c){
		int i=0;
    
		while(str[i]!='\0'){

			if(str[i]==c){
				return str+i;
			}
			
			i++;
		}
		
		return '\0';    
	}

}
