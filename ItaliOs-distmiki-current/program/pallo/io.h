/*
     Pallo

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef u_char
 #define NOBG 100
 #define u_char unsigned char
 #define u_short unsigned short
 #define u_int unsigned int
#endif

struct Tobg {
   u_char x,y,xold,yold,xai,yai,nai;
   u_char *img;
/* char liv; */
   char nick[6];
   char vis;
   u_int opz;
   u_char nvite;
   u_char cnt; /* contatore per labirinto */
};


#ifdef MAIN
extern unsigned long prima[10],adesso;
extern void delay(int mt,long msec);
extern void disab_ab(int echo1);
extern unsigned char leggichar();
extern void svuota_buff();
extern u_char ogni(int mt,long msec);
extern void init_socket();
extern void close_socket();
extern void initF(); /* funzione di inizializzazione */
extern void leggiF(int t1); /* funzione che valorizza gli ingressi del fantasma (Vista) */
extern void calcolaF(int t1);/* funzione che "calcola" la rete neurale */
extern void applicaF(int t1);/* funzione che applica le uscite della rete neurale */
extern void saldaF(int t1);/* funzione che modifica i pesi dei neuroni vincitori */
extern void randomF();/* funzione che modifica i pesi random */

/* extern char *varF; */
#endif

