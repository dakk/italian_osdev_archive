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

/*         sx avanti dx
   Input: 1   2    3   nemico
   Input: 4   5    6   ostacolo
   Output 7   8    9

         +----------+(9)
        +----(8)    |
       +---(7)      |
       123         456     

      1     /\ O\ OO /O   vediDX->giraDX   3->7->15  4,48
     4*2    OO O/ \/ \O   vediSX->giraSX   1->9->13  12,34
      3                   vedi  ->avanti   2->8->14  8,41

            0     7,3,1,2,3,
   	    5	  8,3,1,2,3,
           10     9,3,1,2,3,
           15    10,3,4,5,6, 
           20    11,3,4,5,6, 
           25    12,3,4,5,6, 
           30    13,6,7,8,9,10,11,12,
           38    14,6,7,8,9,10,11,12,
           46    15,6,7,8,9,10,11,12

****************************************************************

       +++-+++-------(9)
       +++-+++----(8)
       +++-+++-(7)
       123 456     
u_char gene[]={ 7,6,1,2,3,4,5,6,
   		8,7,1,2,3,4,5,6,
                9,6,1,2,3,4,5,6,
              };
      1     /\ O\ OO /O   vediDX->giraDX   3->9 20
     4*2    OO O/ \/ \O   vediSX->giraSX   1->7 2
      3                   vedi  ->avanti   2->8 11     
                          muro  ->giraDX   

*/
#define GIRASX 7
#define GIRADX 9
#define AVANTI 8

#define PALLOSX 1
#define PALLOAV 2
#define PALLODX 3

#define MUROSX 4
#define MUROAV 5
#define MURODX 6

#define LunghGene 9
char gene2[LunghGene+1][3];

#include "io.h"
extern struct Tobg obg[100];
/* extern u_int FantAI; */
extern u_char xcoll[81][25];
/* variabili del fantasma 1 2 3 4 5 6 ingressi, 7 8 9 uscite */
extern char varF[NOBG][LunghVar];
u_char RandPnt=0,RandPnt2=0;

/* variabili del fantasma 1 2 3 4 5 6 ingressi, 7 8 9 uscite */
/* neuroni; idneurone,numeroing,iding,iding,etc */
/*    1     /\ O\ OO /O   vediDX->giraDX   3->9 
     4*2    OO O/ \/ \O   vediSX->giraSX   1->7 
      3                   vedi  ->avanti   2->8     
                          muro  ->giraDX   5->9
                          muro  ->giraDX   4->9 
                          muro  ->giraDX   6->7 */
/* funzione di inizializzazione */
#define InsGene(a,b,c,d) gene2[a][0]=b;gene2[a][1]=c;gene2[a][2]=d;
void initF() {
  int t;
  memset(gene2,0,LunghGene*3);
  InsGene(0,1,7,0);
  InsGene(1,2,8,0);
  InsGene(2,3,9,0);
  InsGene(3,8,10,3);
  InsGene(4,10,8,3);
  InsGene(5,5,9,6);
/*
  InsGene(6,5,7,5);
  InsGene(7,4,9,10);
  InsGene(8,6,7,5);
*/
/*
  InsGene(6,4,7,20);
  InsGene(7,6,9,20);
*/  
  for (t=0;t<NOBG;t++) {
   memset(varF[t]+1,0,LunghVar);
   varF[t][0]=1;
/*  cgene[4]=cgene[48]=cgene[12]=cgene[34]=cgene[8]=cgene[41]=30; */
/*    1     /\ O\ OO /O   vediDX->giraDX   3->9 20
     4*2    OO O/ \/ \O   vediSX->giraSX   1->7 2
      3                   vedi  ->avanti   2->8 11     
                          muro  ->giraDX   */
  }
}
/* funzione che valorizza gli ingressi del fantasma (Vista) */
#define RaggioVisP 50
#define RaggioVisM 10
#define aggiorna(a,b,c,d,e,f) {\
        if (obg[xcoll[a][b]-1].opz&0x0040) { p[d]=RaggioVisP-c;t=f;}\
        if (obg[xcoll[a][b]-1].opz&0x1020) { p[e]=RaggioVisM-c;t=f;}}

void leggiF(int t1) {
 int t;
 u_char x,y;
 char p[8];
    x=obg[t1].x; y=obg[t1].y;
    for (t=0;t<8;t++) p[t]=0;
    for (t=x;t<79;t++) {
     if ((xcoll[t][y]!=(t1+1))&&(xcoll[t][y]))
        aggiorna(t,y,t+x,1,5,79);
     if ((xcoll[t][y+1]!=(t1+1))&&(xcoll[t][y+1])&&(t<79))
        aggiorna(t,y+1,t+x,1,5,79)
    }
    for (t=x;t>=0;t--) {
     if ((xcoll[t][y]!=(t1+1))&&(xcoll[t][y]))
        aggiorna(t,y,x+t,3,7,0);
     if ((xcoll[t][y+1]!=(t1+1))&&(xcoll[t][y+1])&&(t>0))
        aggiorna(t,y+1,x+t,3,7,0); 
    }
    for (t=y;t<26;t++) {
     if ((xcoll[x][t]!=(t1+1))&&(xcoll[x][t]))
        aggiorna(x,t,t+y,2,6,26);
     if ((xcoll[x+1][t]!=(t1+1))&&(xcoll[x+1][t])&&(t<26))
        aggiorna(x+1,t,t+y,2,6,26);
    }
    for (t=y;t>=0;t--) {
     if ((xcoll[x][t]!=(t1+1))&&(xcoll[x][t]))
        aggiorna(x,t,y+t,0,4,0);
     if ((xcoll[x+1][t]!=(t1+1))&&(xcoll[x+1][t])&&(t>0))
        aggiorna(x+1,t,y+t,0,4,0);
    }
    for (t=0;t<8;t++) 
      if (p[t]<0) p[t]=0;
    switch (varF[t1][0]) {
      case 1: varF[t1][1]=p[3]; varF[t1][2]=p[0]; varF[t1][3]=p[1];
              varF[t1][4]=p[7]; varF[t1][5]=p[4]; varF[t1][6]=p[5];
                 break;  /*sopra*/
      case 2: varF[t1][1]=p[0]; varF[t1][2]=p[1]; varF[t1][3]=p[2];
              varF[t1][4]=p[4]; varF[t1][5]=p[5]; varF[t1][6]=p[6];
                 break; /*destra*/
      case 3: varF[t1][1]=p[1]; varF[t1][2]=p[2]; varF[t1][3]=p[3];
              varF[t1][4]=p[5]; varF[t1][5]=p[6]; varF[t1][6]=p[7];
                 break; /*basso*/
      case 4: varF[t1][1]=p[2]; varF[t1][2]=p[3]; varF[t1][3]=p[0];
              varF[t1][4]=p[6]; varF[t1][5]=p[7]; varF[t1][6]=p[4];
                 break;  /*sinistra*/
    }
}



/* funzione che "calcola" la rete neurale */
void calcolaF(int t1) {
  int t;
/*  u_char geneOUT,geneTOT,nRami;*/
/*  char piu[2]; */
/*  short TOT; */

/*
   for (t=0;t<LunghGene;t++)
      varF[t1][(u_char)gene2[t][1]]+=(varF[t1][(u_char)gene2[t][0]]*gene2[t][2]/(char)10);
   
   for (t=7;t<LunghVar;t++) {
      if ((--varF[t1][t])>40) varF[t1][t]=40;
      if (varF[t1][t]<-40) varF[t1][t]=-40;
  }
  for (t=0;t<LunghVar;t++) fprintf(stderr,"%i=%i ",t,varF[t1][t]);
*/

#define pnt(x) varF[t1][(u_char)gene2[t][x]]
/*   for (t=7;t<LunghVar;t++) varF[t1][t]=0; */

   for (t=0;t<LunghGene;t++)
    if (gene2[t][0]) 
     pnt(1)+=pnt(0)-gene2[t][2];

   for (t=7;t<LunghVar;t++) {
      if ((--varF[t1][t])>40) varF[t1][t]=40;
      if (varF[t1][t]<-10) varF[t1][t]=-10;
  }
#ifdef VARF
  for (t=0;t<LunghVar;t++) fprintf(stderr,"%i=%i ",t,varF[t1][t]); 
#endif

/*
   piu[0]=0;   piu[1]=0;
   for (t=GIRASX;t<(GIRADX+1);t++) {
     if (varF[t1][t]>piu[0]) {piu[1]=t; piu[0]=varF[t1][t];}
     varF[t1][t]=0;
   }
   if ((piu[1]!=0)&&(piu[0]>10)) varF[t1][(u_char)piu[1]]=1;   
*/
}

/* funzione che applica le uscite della rete neurale */
#define VERSO varF[t1][0]
void applicaF(int t1) {
u_char zp;  /* SX AV DX */
  zp=0;
  if (varF[t1][GIRASX]>varF[t1][GIRADX]) zp|=1;
  if (varF[t1][GIRADX]>varF[t1][AVANTI]) zp|=2;
  if (varF[t1][AVANTI]>varF[t1][GIRASX]) zp|=4;
/*
   sx (zp&5)==1   (zp&2)4
   av (zp&6)==4   2
   dx (zp&3)==2   1
*/

#ifdef LVL
/*  fprintf(stderr,"(zp1=%01x) ",zp); */
#endif
    
  if (((zp&5)==1)&&(varF[t1][GIRASX]>10)) { /* GIRASX */
      varF[t1][GIRASX]=0;
      obg[t1].vis=1; if ((--VERSO)<1) VERSO=4; 
  } else
  if (((zp&3)==2)&&(varF[t1][GIRADX]>10)) { /* GIRADX */
      varF[t1][GIRADX]=0;
      obg[t1].vis=1; if ((++VERSO)>4) VERSO=1; 
  } else
  if (((zp&6)==4)&&(varF[t1][AVANTI]>10)) {/* avanti */
    varF[t1][AVANTI]=0;
    switch (varF[t1][0]) {
      case 1: obg[t1].y-=(obg[t1].y>1);break;  /*sopra*/
      case 2: obg[t1].x+=(obg[t1].x<80);break; /*destra*/
      case 3: obg[t1].y+=(obg[t1].y<23);break; /*basso*/
      case 4: obg[t1].x-=(obg[t1].x>1);break;  /*sinistra*/
    }
  }
  if ((obg[t1].x==obg[t1].xai)&&(obg[t1].y==obg[t1].yai)) {
/*    if ((obg[t1].nai++)>10) modifica un parametro random */
  }
}
/* funzione che modifica i pesi dei neuroni vincitori */
void saldaF(int t1) {
/*
  int t;
  u_char geneOUT,nRami;
   for (t=0;gene[t];t++) {  scanna i neuroni 
      geneOUT=gene[t++];
      nRami=gene[t++];
      if (varF[t1][geneOUT]>0)  se un neurone e' vincitore 
       for (;nRami;nRami--) {  se un peso e' positivo viene incrementato 
         if (cgene[t]>0) cgene[t]+=(cgene[t]<50);
         t++;  
       }
   }
*/
}

/* funzione che modifica i pesi random */
void randomF() {
/*
 int t;
  if ((++RandPnt)>=LunghGene) RandPnt=0;
  fprintf(stderr,"Rnd=%i ",RandPnt);
  cgene[RandPnt]+=5*(cgene[RandPnt]<50); 
  for (t=0;t<5;t++) {
    if ((RandPnt2+=7)>=LunghGene) RandPnt2-=LunghGene;
    cgene[RandPnt2]-=(cgene[RandPnt2]>-10);
  }
*/

/*  Rand1=(u_char)(LunghGene* (rand()/(RAND_MAX+1)));  */
/*  for (t=0;t<LunghGene;t++) cgene[t]-=(cgene[t]>-10); */
}

