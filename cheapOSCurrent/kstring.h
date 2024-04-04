
#ifndef	_KSTRING_
#define	_KSTRING_

#include	"config.h"

#define	KSTRNG_LUNG		128
#define	KNUM_LUNG			16		//Lunghezza massima dei numeri in cifre

//Gestione delle stringhe a livello kernel
unsigned int	ksprint(char* buffer,const char* stringa,...);		//Espande stringa per ora solo \n %d %x %s
unsigned int	kstrlen(const char* stringa);					//Lunbghezza della stringa '\0' escluso
char* 		kstrcpy(char *dest,const char *src);			//Copia src in dest
bool			kstrcmp(const char* s1,const char* s2);			//Ritorna TRUE se uguali
bool			kstrncmp(const char *s1,const char *s2,int  n);	//Ritorna TRUE se uguali i primi (o gli ultimi) (-)n caratteri
int			katoi(char* buffer);							//Da stringa ad intero

#endif
