
#ifndef	_KMATH_
#define	_KMATH_

#include "config.h"

#include	 "time.h"

#define	PI		3.1415926535897932384626433		//NO Voi mette ora!!
#define	ABS(x)	((x) < 0 ? -(x) : (x))

#define  BCD2DEC(x)      ( (x >> 4) * 10 + (x & 0x0f) )
#define  DEC2BCD(x)      (( ((x/10) << 4) + (x % 10) ) & 0xff)

#define RAND_TABLE_SIZE	240//2000

unsigned long	kpow(unsigned long base,unsigned long esponente);		//Potenza
unsigned long	kfloor(double __x);									//Arrotonda per Difetto
unsigned long	kceil (double __x);									//Arrotonda per eccesso

int	krandseed(int seed);										//Pinat il seme della Random
int	krand();													//Restituisce un numero casuale(forse)

#endif
