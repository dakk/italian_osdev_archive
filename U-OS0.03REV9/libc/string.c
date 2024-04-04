#include <string.h>
#include <stdlib.h>

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

  *p-- = 0;

  while(p > pbase)
  {
    char tmp;
    tmp = *p;
    *p = *pbase;
    *pbase = tmp;
    
    p--; pbase++;
  }
}

size_t strlen(const char *string){
		int intint_tmp;

		for(intint_tmp=0;string[intint_tmp]!='\0';intint_tmp++){}

		return intint_tmp;
	}

void strcat(char *strDestination, const char *strSource){
		int strDestinationSize = strlen(strDestination);

		int intint_tmp;

		for(intint_tmp=0;strSource[intint_tmp]!='\0';intint_tmp++){
			strDestination[strDestinationSize] = strSource[intint_tmp];
			strDestinationSize++;
		}

		strDestination[strDestinationSize] = '\0';
}

