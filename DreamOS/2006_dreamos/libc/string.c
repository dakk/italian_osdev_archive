#include <string.h>
#include <stdlib.h>

/**
 *   _kstrncmp()
 *
 *   Compares the first (at most) n characters of s1 and s2
 *   Return: 0(equals), 1(s1>s2), -1(s1<s2)
 */
int _kstrncmp(const char *s1, const char *s2, int n)
{
  int sn=0;

  while (*s1 == *s2 && sn < n-1)
  {
    s1++;
    s2++;
    sn++;
  }

  if (*s1 > *s2) return 1;
  if (*s1 < *s2) return -1;
  return 0;
}

/**
 *   Converte un numero in una stringa
 */
void _kntos(char *buffer, int num, int base)
{
  int mod;
  char *p, *pbase;

  p = pbase = buffer;

  if (num == 0)
    *p++ = 48;

  while(num > 0)
  {
    mod = num % base;
    *p++ = mod + 48;
    num = num / base;
  }

  // termina la stringa
  *p-- = 0;

  // fa il reverse
  while(p > pbase)
  {
    char tmp;
    tmp = *p;
    *p = *pbase;
    *pbase = tmp;
    
    p--; pbase++;
  }
}

/*
	Autore: WindowsUninstall
	Determina la dimensione di una stringa
*/
size_t strlen(const char *string){
		int intint_tmp;

		for(intint_tmp=0;string[intint_tmp]!='\0';intint_tmp++){}

		return intint_tmp;
	}

/*
	Autore: WindowsUninstall
	Concatena un stringa
*/	
void strcat(char *strDestination, const char *strSource){
		//determina la dimensione della stringa di destinazione
		int strDestinationSize = strlen(strDestination);

		int intint_tmp;

		for(intint_tmp=0;strSource[intint_tmp]!='\0';intint_tmp++){
		  strDestination[strDestinationSize] = strSource[intint_tmp];
			strDestinationSize++;
		}

		strDestination[strDestinationSize] = '\0';
}

/*
 * Autrice: Lisa
 * copia n bytes da s2 ad s1, senza pero' eccedere i limiti di s1
 */
char *strncpy(char *s1, const char *s2, size_t n) {
  int i;

  for (i=0; i<n && i<strlen(s1); i++)
    *(s1+i)=*(s2+i);

  return s1;
}

void *memcpy(void *dest, const void *src, size_t n) { 
  // Copy a memory from src to dest with n length
  size_t i;
  for (i = 0; i < n; i++)
    *((char *) dest + i) = *((char *) src + i);
  return dest;
}

void *memset(void *dest, const int c, size_t n) {
	// Set an unique value from dest to dest+n
	size_t i;
	for (i = 0; i < n; i++)
		*((int *) dest + i) = c;
	return dest;
}
