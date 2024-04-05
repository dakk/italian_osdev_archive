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

#ifndef _LIBC_STRING_H_
#define _LIBC_STRING_H_

extern "C"{
	int strlen(const char *str); // Restituisce il numero di caratteri in una stringa

	char *strcat(char *dest, const char *src); // Attacca una stringa ad un'altra

	char *strcpy(char *dest, const char *src); // Incolla una stringa in un'altra

	int strcmp(const char *str1, const char *str2);// Confronta due stringhe e restituisce 0 se sono uguali, 1 se la prima è più grande e -1 se è più piccola

	char *strncat(char *dest, const char *src, int n); // Attacca n char di una stringa ad un'altra

	char *strncpy(char *dest, const char *src, int n); // Incolla n char di una stringa in un'altra

	int strncmp(const char *str1, const char *str2, int n); // Confronta n char di due stringhe

	char *strchr(char *str, int c);  // Cerca un carattere in una stringa
}

#endif
