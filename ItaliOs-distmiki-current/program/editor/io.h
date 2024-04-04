#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>


#ifndef BSD
 #ifndef TIOCGWINSZ
  #define TIOCGWINSZ (int)0x5413
 #endif
#endif

#ifndef Disabilita
 #define Disabilita disab_ab(0)
 #define Abilita disab_ab(1)
 #define LunghVar 11
#endif

#ifndef u_char
 #define NOBG 100
 #define ASPETTA 0
 #define NON_ASPETTA 1
 #define u_char unsigned char
#endif
#ifndef SU
 #define SU 193
 #define GIU 194
 #define DX 195
 #define SX 196
 #define PF1 208
 #define PF2 209
 #define PF3 210
 #define PF4 211
 #define DEL 127
 #define HOME 177
 #define INS 178
 #define CANC 179
 #define FINE 180
 #define PGSU 181
 #define PGGIU 182
#endif

#ifdef MAIN
extern unsigned long prima[10],adesso;
extern void delay(int mt,long msec);
extern void disab_ab(int echo1);
extern unsigned char leggichar();
extern void svuota_buff();
extern u_char ogni(int mt,long msec);
#endif

