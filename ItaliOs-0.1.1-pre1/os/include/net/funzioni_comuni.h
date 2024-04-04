#ifndef _FUNZCOMUNI_H_
#define _FUNZCOMUNI_H_

#include <rete.h>
void copymem(u_char *bsrc,u_char *bdst,u_short blen);/* copia una porzione di memoria su un altra */
void shiftDX(u_char *bsrc,u_short blenoff,u_short blen);
void shiftSX(u_char *bsrc,u_short blenoff,u_short blen);
void setmem(void *bsrc,u_char bst,u_short blen);/* setta una porzione di memoria con un dato */
u_short scbyte(u_short bsrc);/* su un short scambia i byte */
u_int scbyte4(u_int bsrc);/* su un intero (4byte) scambia i byte (4321->1234) */
u_int IPinNUM(char *ip); /* converte un IP dal formato stringa a numerico */
u_char *NUMinIP(u_int ip1); /* converte un IP dal formato numerico a stringa */
unsigned int str_str(char *s1,char *s2);/* cerca la stringa s2 in s1 */
unsigned int atou(char *nu); /* converte una stringa in un numero */
u_int Tempo(); /* restituisce i secondi dall'inizio della giornata */
#endif
