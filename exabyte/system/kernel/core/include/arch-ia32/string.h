/* string.h -- ia32 strings manipulation functions definitions */
/******************************************************************************
* Exabyte Operating System                                                    *
*                                                                             *
* This string-include defines all string functions as inline                  *
* functions. Use gcc. It also assumes ds=es=data space, this should be        *
* normal. Most of the string-functions are rather heavily hand-optimized,     *
* see especially strsep,strstr,str[c]spn. They should work, but are not       *
* very easy to understand. Everything is done entirely within the register    *
* set, making the functions fast and clean. String instructions have been     *
* used through-out, making for "slightly" unclear code :-)                    *
*                                                                             *
*		NO Copyright (C) 1991, 1992 Linus Torvalds,                   *
*		consider these trivial functions to be PD.                    *
*******************************************************************************/

#ifndef ARCH_STRING_H
#define ARCH_STRING_H

#include <stddef.h>

static inline char * strcpy(char * dest,const char *src)
{
int d0, d1, d2;
__asm__ __volatile__(
	"1:\tlodsb\n\t"
	"stosb\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b"
	: "=&S" (d0), "=&D" (d1), "=&a" (d2)
	:"0" (src),"1" (dest) : "memory");
return dest;
}

static inline char * strncpy(char * dest,const char *src,size_t count)
{
int d0, d1, d2, d3;
__asm__ __volatile__(
	"1:\tdecl %2\n\t"
	"js 2f\n\t"
	"lodsb\n\t"
	"stosb\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n\t"
	"rep\n\t"
	"stosb\n"
	"2:"
	: "=&S" (d0), "=&D" (d1), "=&c" (d2), "=&a" (d3)
	:"0" (src),"1" (dest),"2" (count) : "memory");
return dest;
}

static inline char * strcat(char * dest,const char * src)
{
int d0, d1, d2, d3;
__asm__ __volatile__(
	"repne\n\t"
	"scasb\n\t"
	"decl %1\n"
	"1:\tlodsb\n\t"
	"stosb\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b"
	: "=&S" (d0), "=&D" (d1), "=&a" (d2), "=&c" (d3)
	: "0" (src), "1" (dest), "2" (0), "3" (0xffffffffu):"memory");
return dest;
}

static inline char * strncat(char * dest,const char * src,size_t count)
{
int d0, d1, d2, d3;
__asm__ __volatile__(
	"repne\n\t"
	"scasb\n\t"
	"decl %1\n\t"
	"movl %8,%3\n"
	"1:\tdecl %3\n\t"
	"js 2f\n\t"
	"lodsb\n\t"
	"stosb\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n"
	"2:\txorl %2,%2\n\t"
	"stosb"
	: "=&S" (d0), "=&D" (d1), "=&a" (d2), "=&c" (d3)
	: "0" (src),"1" (dest),"2" (0),"3" (0xffffffffu), "g" (count)
	: "memory");
return dest;
}

static inline int strcmp(const char * cs,const char * ct)
{
int d0, d1;
register int __res;
__asm__ __volatile__(
	"1:\tlodsb\n\t"
	"scasb\n\t"
	"jne 2f\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n\t"
	"xorl %%eax,%%eax\n\t"
	"jmp 3f\n"
	"2:\tsbbl %%eax,%%eax\n\t"
	"orb $1,%%al\n"
	"3:"
	:"=a" (__res), "=&S" (d0), "=&D" (d1)
		     :"1" (cs),"2" (ct));
return __res;
}

static inline int strncmp(const char * cs,const char * ct,size_t count)
{
register int __res;
int d0, d1, d2;
__asm__ __volatile__(
	"1:\tdecl %3\n\t"
	"js 2f\n\t"
	"lodsb\n\t"
	"scasb\n\t"
	"jne 3f\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n"
	"2:\txorl %%eax,%%eax\n\t"
	"jmp 4f\n"
	"3:\tsbbl %%eax,%%eax\n\t"
	"orb $1,%%al\n"
	"4:"
		     :"=a" (__res), "=&S" (d0), "=&D" (d1), "=&c" (d2)
		     :"1" (cs),"2" (ct),"3" (count));
return __res;
}

static inline char * strchr(const char * s, int c)
{
int d0;
register char * __res;
__asm__ __volatile__(
	"movb %%al,%%ah\n"
	"1:\tlodsb\n\t"
	"cmpb %%ah,%%al\n\t"
	"je 2f\n\t"
	"testb %%al,%%al\n\t"
	"jne 1b\n\t"
	"movl $1,%1\n"
	"2:\tmovl %1,%0\n\t"
	"decl %0"
	:"=a" (__res), "=&S" (d0) : "1" (s),"0" (c));
return __res;
}

static inline char * strrchr(const char * s, int c)
{
int d0, d1;
register char * __res;
__asm__ __volatile__(
	"movb %%al,%%ah\n"
	"1:\tlodsb\n\t"
	"cmpb %%ah,%%al\n\t"
	"jne 2f\n\t"
	"leal -1(%%esi),%0\n"
	"2:\ttestb %%al,%%al\n\t"
	"jne 1b"
	:"=g" (__res), "=&S" (d0), "=&a" (d1) :"0" (0),"1" (s),"2" (c));
return __res;
}

static inline size_t strlen(const char * s)
{
int d0;
register int __res;
__asm__ __volatile__(
	"repne\n\t"
	"scasb\n\t"
	"notl %0\n\t"
	"decl %0"
	:"=c" (__res), "=&D" (d0) :"1" (s),"a" (0), "0" (0xffffffffu));
return __res;
}

static inline void * memcpy(void * to, const void * from, size_t n)
{
int d0, d1, d2;
__asm__ __volatile__(
	"rep ; movsl\n\t"
	"testb $2,%b4\n\t"
	"je 1f\n\t"
	"movsw\n"
	"1:\ttestb $1,%b4\n\t"
	"je 2f\n\t"
	"movsb\n"
	"2:"
	: "=&c" (d0), "=&D" (d1), "=&S" (d2)
	:"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
	: "memory");
return (to);
}

static inline void * memmove(void * dest,const void * src, size_t n)
{
int d0, d1, d2;
if (dest<src)
__asm__ __volatile__(
	"rep\n\t"
	"movsb"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2)
	:"0" (n),"1" (src),"2" (dest)
	: "memory");
else
__asm__ __volatile__(
	"std\n\t"
	"rep\n\t"
	"movsb\n\t"
	"cld"
	: "=&c" (d0), "=&S" (d1), "=&D" (d2)
	:"0" (n),
	 "1" (n-1+(const char *)src),
	 "2" (n-1+(char *)dest)
	:"memory");
    return dest;
}

static inline int memcmp(const void *s1, const void *s2, size_t n)
{
        unsigned int i;
        for (i = 0; i < n; i++)
                if (*((char *) s1 + i) != *((char *) s2 + i))
                        return *((char *) s1 + i) - *((char *) s2 + i);
        return 0;
}

static inline void * memchr(const void * cs,int c,size_t count)
{
int d0;
register void * __res;
if (!count)
	return NULL;
__asm__ __volatile__(
	"repne\n\t"
	"scasb\n\t"
	"je 1f\n\t"
	"movl $1,%0\n"
	"1:\tdecl %0"
	:"=D" (__res), "=&c" (d0) : "a" (c),"0" (cs),"1" (count));
return __res;
}

static inline void * memset(void * s, char c,size_t count)
{
int d0, d1;
__asm__ __volatile__(
	"rep\n\t"
	"stosb"
	: "=&c" (d0), "=&D" (d1)
	:"a" (c),"1" (s),"0" (count)
	:"memory");
return s;
}

static inline size_t strnlen(const char * s, size_t count)
{
int d0;
register int __res;
__asm__ __volatile__(
	"movl %2,%0\n\t"
	"jmp 2f\n"
	"1:\tcmpb $0,(%0)\n\t"
	"je 3f\n\t"
	"incl %0\n"
	"2:\tdecl %1\n\t"
	"cmpl $-1,%1\n\t"
	"jne 1b\n"
	"3:\tsubl %2,%0"
	:"=a" (__res), "=&d" (d0)
	:"c" (s),"1" (count));
return __res;
}

/* grazie a non ricordo chi per questa funzione */
static inline char *strstr(const char *orig, const char *pattern)
{
	unsigned char trovato = 0;	//al momento non si �trovato gnente
	unsigned long lung = strlen(pattern);
	unsigned long i;
	if (lung > strlen(orig))
		return NULL;
	while (*orig) {		//finche la stringha contentinitore non �nulla
		if ((*orig) == (*pattern)) {
			trovato = 1;
			i = 1;
			while (*(pattern + i)) {
				if (*(orig + i) != *(pattern + i)) {
					trovato = 0;
					break;	//str non trovata
				}
				i++;
			}
			if (trovato)
				return (char *)orig;
		}
		orig++;
	}
	return (char *)NULL;
}

static inline void * memscan(void * addr, int c, size_t size)
{
	if (!size)
		return addr;
	__asm__("repnz; scasb\n\t"
		"jnz 1f\n\t"
		"dec %%edi\n"
		"1:"
		: "=D" (addr), "=c" (size)
		: "0" (addr), "1" (size), "a" (c));
	return addr;
}

#ifdef CONFIG_ARCH_SSE

// 1 para = 16 bytes = 128 bits

// Copia di 'paras' blocchi di memoria di 16 bytes
static inline void * arch_ia32_sse_pmemcpy(const void * dest , const void * orig , size_t paras) {
char xmm0[16];				// Qua salvero' il registro xmm0 per poi ripristinarlo alla fine
asm(					// La funzione di copia e' scritta in assembly
"movups %%xmm0,(%%edx)\n"		// Salvo il contenuto del registro xmm0
"jecxz sse_pmemcpy_2\n"			// Se ecx=0 salto a "sse_pmemcpy_2" (vuol dire che non ci sono paras da copiare)
"sse_pmemcpy_1:\n"			// Etichetta "sse_pmemcpy_1"
"sub $2,%%ecx\n"			// Decremento di 2 ecx
"movups (%%eax,%%ecx,8),%%xmm0\n"	// Copio il contenuto della locazione di memoria di origine in xmm0
"movups %%xmm0,(%%ebx,%%ecx,8)\n"	// Copio il contenuto di xmm0 nella locazione di memoria di destinazione
"jecxz sse_pmemcpy_2\n"			// Se ecx=0 salto a "sse_pmemcpy_2"
"jmp sse_pmemcpy_1\n"			// Salto a "sse_pmemcpy_1"
"sse_pmemcpy_2:\n"			// Etichetta "sse_pmemcpy_2"
"movups (%%edx),%%xmm0\n"		// Ripristino il registro xmm0

: // No output
: // Input:
	"a" (orig),		// Puntatore alla zona di memoria di origine
	"b" (dest),		// Puntatore alla zona di memoria di destinazione
	"c" (paras*2),		// Numero delle paras da copiare per 2
	"d" (xmm0)		// Indirizzo di 8 bytes disponibili dove salvare %%mm0
: "memory"
);
return (void*)dest;	// Ritorno un puntatore all'inizio della memoria di destinazione 
}

// Copia/sposta di 'paras' blocchi di memoria di 16 bytes
static inline void * arch_ia32_sse_pmemmove(const void * dest , const void * orig , size_t paras) {
char xmm0[16];				// Qua salvero' il registro xmm0 per poi ripristinarlo alla fine
if (dest==orig || paras==0);		/* Se origine e destinazione sono uguali oppure non devo copiare
					nulla, non faccio nulla */
else if (dest>orig || (dest+(paras*16))<orig) {
	// Questo e' l'algoritmo di copia piu' veloce e copia dall'ultimo para al primo
	// Lo uso se: la destinazione e' dopo l'origine
	// oppure se le zone di memoria da copiare non si sovrappongono
	asm(					// La funzione di copia e' scritta in assembly
	"movups %%xmm0,(%%edx)\n"		// Salvo il contenuto del registro xmm0
	"jecxz arch_ia32_sse_pmemmove_2\n"	// Se ecx=0 salto a "arch_ia32_sse_pmemmove_2" (vuol dire
						// che non ci sono paras da copiare)
	"arch_ia32_sse_pmemmove_1:\n"		// Etichetta "arch_ia32_sse_pmemmove_1"
	"sub $2,%%ecx\n"			// Decremento di 2 ecx
	"movups (%%eax,%%ecx,8),%%xmm0\n"	// Copio il contenuto della locazione di memoria di origine
						// in xmm0
	"movups %%xmm0,(%%ebx,%%ecx,8)\n"	// Copio il contenuto di xmm0 nella locazione di memoria di
						// destinazione
	"jecxz arch_ia32_sse_pmemmove_2\n"	// Se ecx=0 salto a "arch_ia32_sse_pmemmove_2"
	"jmp arch_ia32_sse_pmemmove_1\n"	// Salto a "arch_ia32_sse_pmemmove_1"
	"arch_ia32_sse_pmemmove_2:\n"		// Etichetta "arch_ia32_sse_pmemmove_2"
	"movups (%%edx),%%xmm0\n"		// Ripristino il registro xmm0
	
	: // No output
	: // Input:
		"a" (orig),		// Puntatore alla zona di memoria di origine
		"b" (dest),		// Puntatore alla zona di memoria di destinazione
		"c" (paras*2),		// Numero delle paras da copiare per 2
		"d" (xmm0)		// Indirizzo di 8 bytes disponibili dove salvare %%mm0
	: "memory"
	);
}
else { // dest<orig e le zone di memoria si sovrappongono
	asm(					// Il codice e' in assembly
	"movups %%xmm0,(%%edx)\n"		// Salvo il contenuto del registro xmm0
	"arch_ia32_sse_pmemmove_3:"		// Etichetta "arch_ia32_sse_pmemmove_1"
	"dec %%ecx\n"				// Decremento ecx
	"movups (%%eax),%%xmm0\n"		// Copio il contenuto della locazione di memoria di origine
						// in xmm0
	"movups %%xmm0,(%%ebx)\n"		// Copio il contenuto di xmm0 nella locazione di memoria
						// di destinazione
	"add $16,%%eax\n"			// Aggiungo 16 a eax
	"add $16,%%ebx\n"			// Aggiungo 16 a ebx
	"jecxz arch_ia32_sse_pmemmove_4\n"	// Se ecx=0 salto a "arch_ia32_sse_pmemmove_2"
	"jmp arch_ia32_sse_pmemmove_3\n"	// (altrimenti) salto a "arch_ia32_sse_pmemmove_1"
	"arch_ia32_sse_pmemmove_4:\n"		// Etichetta "arch_ia32_sse_pmemmove_2"
	"movups (%%edx),%%xmm0\n"		// Ripristino il registro xmm0
	: // No output
	: // Input:
		"a" (orig),		// Puntatore alla zona di memoria di origine
		"b" (dest),		// Puntatore alla zona di memoria di destinazione
		"c" (paras),		// Numero dei paras da copiare
		"d" (xmm0)		// Indirizzo di 8 bytes disponibili dove salvare %%mm0
	);
}
return (void*)dest;	// Ritorno un puntatore all'inizio della memoria di destinazione 
}

// Esegue la memcpy copiando tramite sse e copia normalmente quanto non appartiene ai blocchi di paras
static inline void * arch_ia32_sse_memcpy(const void * dest , const void * orig , size_t bytes) {
int paras=bytes/16;	// Inserisco in paras il numero dei paras da copiare
bytes%=16;		// Inserisco in bytes il numero di bytes che non sono inclusi nei paras
arch_ia32_sse_pmemcpy(dest,orig,paras);	// Copio i paras
if (bytes) {	// Eseguo questo codice solo se sono rimasti effettivamente bytes da copiare
paras*=16;	/* Moltiplico paras per 16 in modo da ottenere non piu' il numero dei paras da
copiare ma l'effettiva dimensione di tutti i paras copiati espressa in numero di bytes. Es.: se ho
copiato 2 paras, il nuovo valore sara' 2*16=32 (2para=32bytes) */
memcpy((void*)(dest+paras),(void*)(orig+paras),bytes); /* Copio i bytes non inclusi dai paras che
ho copiato */
}
return (void*)dest;	// Ritorno un puntatore all'inizio della memoria di destinazione
}

// Esegue la memmove copiando tramite sse e copia normalmente quanto non appartiene ai blocchi di paras
static inline void * arch_ia32_sse_memmove(const void * dest , const void * orig , size_t bytes) {
int paras=bytes/16;	// Inserisco in paras il numero delle paras da copiare
bytes%=16;		// Inserisco in bytes il numero di bytes che non sono inclusi nelle paras
arch_ia32_sse_pmemmove(dest,orig,paras);	// Copio le paras
if (bytes) {	// Eseguo questo codice solo se sono rimasti effettivamente bytes da copiare
paras*=16;	/* Moltiplico paras per 16 in modo da ottenere non piu' il numero delle paras da
copiare ma l'effettiva dimensione di tutte le paras copiate espressa in numero di bytes. Es.: se ho
copiato 2 paras, il nuovo valore sara' 2*16=32 (2para=32bytes) */
memmove((void*)(dest+paras),(void*)(orig+paras),bytes); /* Copio i bytes non inclusi dalle qwords che
ho copiato */
}
return (void*)dest;	// Ritorno un puntatore all'inizio della memoria di destinazione
}

#endif	/* CONFIG_ARCH_SSE */

#endif	/* ARCH_STRING_H */
