#include <rand.h>
#include <time.h>
#include <stddef.h>


dword casual()			// Periodo ~2113 ~1034
{
	static dword z1, z2, z3, z4, INI = 1;
	dword b;
	if (INI) {
		INI = 0;
		do
			z1 = time(NULL);
		while (z1 < 2);
		do
			z2 = time(NULL);
		while (z2 < 8);
		do
			z3 = time(NULL);
		while (z3 < 16);
		do
			z4 = time(NULL);
		while (z4 < 128);
	}
	b = ((z1 << 6) ^ z1) >> 13;
	z1 = ((z1 & 4294967294U) << 18) ^ b;
	b = ((z2 << 2) ^ z2) >> 27;
	z2 = ((z2 & 4294967288U) << 2) ^ b;
	b = ((z3 << 13) ^ z3) >> 21;
	z3 = ((z3 & 4294967280U) << 7) ^ b;
	b = ((z4 << 3) ^ z4) >> 12;
	z4 = ((z4 & 4294967168U) << 13) ^ b;
	return z1 ^ z2 ^ z3 ^ z4;
}


// Ritorna un numero casuale da 0 ÷ n-1
dword casual_n(dword n)
{
	//dword S=4294967296/n;
	dword S = (~0) / n;
	dword X = S * n;
	dword x;
	if (X) {
		do
			x = casual();
		while (x >= X);
	} else
		x = casual();
	return x / S;		// Appena più veloce di x%n
}
