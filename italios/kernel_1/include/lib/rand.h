#ifndef RAND_H
#define RAND_H

#ifdef __cplusplus
	extern "C"{
#endif
		#include <stddef.h>

//ritorna un numero casuale tra 0 e (2^32) - 1
dword casual();

// Ritorna un numero casuale da 0 ÷ n-1
dword casual_n( dword n);


#ifdef __cplusplus
	};
#endif

#endif
