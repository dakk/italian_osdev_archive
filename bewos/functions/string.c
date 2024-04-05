#include<string.h>

/********************************************************************************
********************************************************************************/
//Conta quanti caratteri ci sono in una stringa
int strlen(const char *str)
{
	int lunghezza;

	for(lunghezza = 0; *str != '\0'; str++)
		lunghezza++;
	return lunghezza;
}
/********************************************************************************
********************************************************************************/
//Compara due stringhe. Se sono uguali restituisce 0 altrimenti restituisce 1
int stringcmp(const char *str1, const char *str2, int n)
{
  int i=0;

  while (*str1 == *str2 && i < n)
  {
    str1++;
    str2++;
    i++;
  }

  if(*str1 != *str2)
   return 1;

  else //ho messo questo else per rendere il codice più chiaro ma si poteva anche omettere
   return 0;
}

