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

/*
    pallo.c    by miki00
    Gioco tipo pacman in modalita' testo
     
main
 +->livello()          scelta livello
 |   +->insobg()       costruisci oggetto
 |   +->gligl()        costruisci percorso
 |
 +->motore()           gioco
     +->io.h           i/o da tastiera
     +->video()        visualizza schermo e gesione collisioni 
         +->video_1()  visualizza oggetto

opz:
  84218421 84218421  
  xxxxxxxx xxxxxxxx
  |||||||| ||||||||  
  |||||||| ||||++++-->000X Mossa tipo: Rimbalzo
  |||||||| |||+------>0010 trasparente
  |||||||| ||+------->0020 Nemico
  |||||||| |+-------->0040 Pallo
  |||||||| +--------->0080 diamante
  |||||||+----------->0100 Sparo
  ||||||+------------>0200 Uscita
  |||||+------------->0400 Mossa tipo: Serpente
  ||||+-------------->0800 Mossa tipo: Insegue
  |||+--------------->1000 Griglia
  ||+---------------->2000 Mossa tipo: AI
  |+----------------->4000 Tarma
  +------------------>8000 DiamanteMorto

 griglia  0x1000
 fantasma 0x0020
 diamante 0x0090
 pallo    0x0040
 uscita   0x0200
 masso    0x0021
 serpente 0x0420
 sparo    0x0170


 31=rosso 32=verde 34=blu
*/



#define MAXIMMAG 10
#define MAIN 1
#include "io.h"

#define ccprint printf(

 u_char *navicella = "\1\4\4\\\3\3\3\3/\2\2\2\4\4\4\4\4\4/\3\3\3\3\\\4\4\4\4\1\1\5,170/\\\2\4\4\\/";
 u_char *incostruzione = "\5,173   In Costruzione!!!  ";
 u_char *fantaAI[] = { "/\\\2\4\4OO","O\\\2\4\4O/",
                       "OO\2\4\4\\/","/O\2\4\4\\O" };
  
 u_char *fanta[] = { "/oO\\\2\4\4\4\4|/\\|" , 
                    "/Oo\\\2\4\4\4\4\\||/" };
 u_char *mostro[] = { 
   "\5,110/\5,147(\6)\5,110\\\2\4\4\4\4\4|\5.170VVV\5.110|\2\4\4\4\4\4|   |\
\2\4\4\4\4\4|\5.170AAA\5.110|\2\4\4\4\4\4\\___/" , 
   "\5,110/\5,147(\6)\5,110\\\2\4\4\4\4\4|\5.170VVV\5.110|\2\4\4\4\4\4|\5.170AAA\
\5.110|\2\4\4\4\4\4\\___/\2\4\4\4\4\4\7\7\7\7\7" };
 u_char *pac="\5,170/@@\\\2\4\4\4\4Pllo";
 u_char *diamante[]= { "/\\\2\4\4\\/", "*\\\2\4\4\\/" };
 u_char *serpente="O";
 u_char *sparo[]={ "\5,174*", "\5,174+"};
 u_char *ostacolo="X";
 u_char *tarma[]={"x","+"};
 u_char *masso="/\\\2\4\4\\/";
 u_char *vuoto="";
 u_char *uscita[]= { "\5,170esci_","\5,130ESCI_" };
 u_char *croce="\1\4\1\4\\\2\\\2\4\4\2\2/\1/\1><\1/\1/\2\2\2\4\4\\\2\\";
 u_char xcoll[81][25];
 u_char gri1[10000];
 u_char fls;
 int ngri=0,tfantasm=300,pntTARMA=2;
 int nobg=0,fsm=0,ndiam=0,tgra=100,ngra=0;
 int sparo1=1,sparo2=0,numvite=1;
 u_short nsparo1 = 0;
 int t,pnt=0,ndia=0,pl;
 char varF[NOBG][LunghVar];
 u_char congela = 0;
 int vitemax = 3;
#define NOMIMAX 40 
 u_int nnomi,tz;
 char nomi[NOMIMAX][2][20];
 u_char ch;
 int livell=1,nvite=0;
 int csposta,tsposta=0;
 char *Msg=NULL,PartVinta[]="Partita Vinta",PartPersa[]="Partita Persa";
 char riff=-1;
 u_char *immag;
 u_short nimmag[MAXIMMAG],timmag=0;

 extern int sk,sk1;

#define colorebase ccprint "\033[40;37m")

#define GRIGLIA    0x1000
#define SERPENTE   0x0C20
#define SERPENTENO 0x1C00
#define TARMA      0x4800 
#define TARMAAI    0x6000 
#define FANTASMA   0x0820
#define FANTASMAAI 0x2020
#define DIAMANTE   0x0090
#define PALLO      0x0040
#define USCITA     0x1200
#define MASSO      0x0021

struct Tobg obg[NOBG];
/*u_int FantAI; */

#include "motore.c"
#include "livello.c"

extern void termina();
extern void terminale();

int main() {
/*
  unsigned char chh ;
  Abilita; 
  chh = leggichar();
  while(1) {
   chh = leggichar();
   if (chh != 35) printf("%03x ",chh);
  }
*/
  terminale(); 
  immag= malloc(4094);
  memset(immag,0,4094);
  timmag = 0;
#ifdef LVL
  livell=LVL;
#else
  livell=0;
#endif
  livello(livell);
  nvite = vitemax;
  if (livell>LIVELLMAX) livell=LIVELLMAX;
  livell--;  
  while (((++livell)<=LIVELLMAX)&&(ch!='q')&&(nvite)) {
    messaggio("Livello:",livell);
    livello(livell);motore();
    if (Msg==PartPersa) { 
      if ((--nvite) == 0) {
        messaggio("Hai Perso!!",-1);
/*      
        #ifndef DEBUG
         ccprint "\033[2J"); 
        #else
         ccprint "\033[22;1H\n");
        #endif
        svuota_buff(1);
        exit(0);
*/
      } else livell--;
      } else nvite++;

  }
  if ((ch!='q')&&(livell>LIVELLMAX)&&(nvite>0)) messaggio("Hai Vintoooooo!!!!!!!! :)))",-1);
#ifndef DEBUG
  ccprint "\033[2J"); 
#else
  ccprint "\033[22;1H\n");
#endif
  svuota_buff(1);
  ccprint "\033[m\033[2J"); 
  ccprint "\033[1;1H");
  termina();
  free(immag);
  return 0;

}
