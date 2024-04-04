/*
ItaliOs
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

#include "rete.h"
#define IRC_adessoPING 20



u_char IrcSK=0;
u_char *buffIRC,*NickIRC,*ChIRC,*bufftempIRC,*buffIRCrx;
u_char ch;
int tIRC=0,tIRC1;
u_short nbuffIRC=0;

void i_aDD(u_char *aA) {
 copymem(aA,buffIRC+nbuffIRC,strlen(aA));nbuffIRC+=strlen(aA);
}

#define i_iNIZIO nbuffIRC=0;setmem(buffIRC,0,600);
/*#define i_aDD(aa) copymem(aa,buffIRC+nbuffIRC,strlen(aa));nbuffIRC+=strlen(aa); */
#define i_fINE buffIRC[nbuffIRC]=0;
#define MANDA if (!trasmetti_str(IrcSK,buffIRC)) Errore(8,"fine socket");/* printf("%s",buffIRC);*/

#define INIZ i_iNIZIO i_aDD( 
#define EE ); i_aDD(
#define FINE ); i_fINE;

/*
  u_int Ip;
  u_short Porta;
*/  
u_char *opzIRC[4];
u_char nopzIRC;
u_int tempoIRC;

void opzionizza(u_char *bufZ) {
  for (nopzIRC=0;nopzIRC<4;nopzIRC++) opzIRC[nopzIRC]=NULL;
  opzIRC[0]=bufZ;nopzIRC=1;bufZ++;
  if (opzIRC[0][0]==':') opzIRC[0]++;
  for (;((*bufZ)&&(nopzIRC<4));bufZ++) {
    if ((*bufZ=='!')&&(nopzIRC==1)) *bufZ=0;
    if (*bufZ==' ') { opzIRC[nopzIRC]=bufZ+1;*bufZ=0;nopzIRC++; } 
  }
}

void irc(u_int Ip,u_short Porta) {
  set_text_color(7);
  if (reteDEBUG&2) printf("irc\n");  
  bufftempIRC=malloc(600);
  buffIRC=malloc(600);
  buffIRCrx=malloc(600);
  NickIRC=malloc(40);
  ChIRC=malloc(40);
  if (!Ip) {
   printf("\ndestinazione:");getstring(buffIRC);
   if (!(Ip=host(buffIRC))) goto errore;
   printf("  Ip=%s\n",NUMinIP(Ip));
  } 
  if (!Porta) {
   printf("Porta dest:");getstring(buffIRC);printf("\n");
   if (!(Porta=atou(buffIRC))) goto errore;
  }
  if (!connettiTCP(&IrcSK,Ip,Porta)) Errore(8,"IRC: errore connessione");

  printf("Connesso\n");
  printf("\nNick:");getstring(NickIRC);printf("\n");
  INIZ "NICK " EE NickIRC FINE; MANDA;
  INIZ "USER " EE NickIRC EE " " EE NickIRC EE "2 " EE NickIRC EE "3 :" EE NickIRC EE "4" FINE;
  MANDA;
  tempoIRC=adesso;
  ch=0;
  printf("Fai 'J #canale' per join ,'P' per part dal canale\n");
  printf("Fai 'p nick' per query '/' per comandi diretti ,'esc+invio' per uscire\n");
  while ((ch!='\033')&&(SkEsiste(IrcSK))) {
/*      if (adesso>(tempoIRC+IRC_adessoPING)) {
         INIZ "PING :1231231" FINE; MANDA;
         tempoIRC=adesso+IRC_adessoPING;
      }*/
      if ((tIRC=ricevi_str(IrcSK,buffIRC,600))!=0) {
        buffIRC[tIRC]=0;
        if (str_str(buffIRC,"PING")>0) {
         buffIRC[1]='O';/*buffIRC[5]=' ';*/
         printf("Ping->"); 
         if (!trasmetti_str(IrcSK,buffIRC)) Errore(8,"fine socket");
         printf("Pong"); 
        } else {
         opzionizza(buffIRC);
         if (str_str(opzIRC[1],"PRIVMSG")>0) {
            set_text_color(14);
            printf("%s <%s> %s\n",opzIRC[2],opzIRC[0],opzIRC[3]); 
            set_text_color(11);
         } else
         if (str_str(opzIRC[1],"372")>0) 
            printf("--- Motd %s\n",opzIRC[3]); else
         if (str_str(opzIRC[1],"JOIN")>0) 
            printf("--- Entra <%s> %s\n",opzIRC[0],opzIRC[2]); else
         if (str_str(opzIRC[1],"PART")>0) 
            printf("--- Esce  <%s> %s\n",opzIRC[0],opzIRC[2]); else
         if (str_str(opzIRC[1],"QUIT")>0) 
            printf("--- Quit <%s> %s\n",opzIRC[0],opzIRC[2],opzIRC[3]); else
         if (str_str(opzIRC[1],"PONG")>0) ; else
         if (str_str(opzIRC[1],"KICK")>0) 
            printf("--- %s Kikka %s da %s\n",opzIRC[0],opzIRC[3],opzIRC[2]); else
         if (str_str(opzIRC[1],"MODE")>0) 
            printf("--- %s Modifica %s %s\n",opzIRC[0],opzIRC[2],opzIRC[3]); else
         if (str_str(opzIRC[1],"353")>0) 
            printf("--- Lista %s\n",opzIRC[3]); else 
         if ((str_str(opzIRC[1],"366")>0)||(str_str(opzIRC[1],"376")>0)) ;
            else {
            set_text_color(7);
            printf("%s %s %s %s\n",opzIRC[0],opzIRC[1],opzIRC[2],opzIRC[3]);
            set_text_color(11);
         }   
/*         printf("%s",buffIRC); 
         printf("\n");*/
        }
      }
      if (kbhit()) {
        set_text_color(15);
        printf("%s <%s> :",ChIRC,NickIRC);getstring(buffIRCrx);ch=*buffIRCrx;
        tIRC=strlen(buffIRCrx);printf("\n");
        set_text_color(11);
        if (buffIRCrx[0]=='/') {
         if (!trasmetti_str(IrcSK,buffIRCrx+1)) Errore(8,"fine socket");
        } else
        if (str_str(buffIRCrx,"L ")==1) {
           if (*ChIRC) {
              INIZ "LIST " EE ChIRC FINE; MANDA; 
              INIZ "NAMES " EE ChIRC FINE; MANDA; 
           }
        } else
        if (str_str(buffIRCrx,"J ")==1) {
           if (*ChIRC) { INIZ "PART " EE ChIRC FINE; MANDA; }  
           copymem(buffIRCrx+2,ChIRC,tIRC-1);
           INIZ "JOIN " EE ChIRC FINE; MANDA;
        } else
        if (str_str(buffIRCrx,"P ")==1) {
           if (*ChIRC) { INIZ "PART " EE ChIRC FINE; MANDA; *ChIRC=0; }  
        } else
        if (str_str(buffIRCrx,"p ")==1) {
           copymem(buffIRCrx+1,buffIRC+7,tIRC-1);
           copymem("PRIVMSG ",buffIRC,8);tIRC+=6;
           if (!trasmetti_str(IrcSK,buffIRC)) Errore(8,"fine socket");
        } else {
           if (*ChIRC) {
            copymem(buffIRCrx,bufftempIRC,tIRC+1);
            INIZ "PRIVMSG " EE ChIRC EE " :" EE bufftempIRC FINE; MANDA; 
           } else {
            if (!trasmetti_str(IrcSK,buffIRC)) Errore(8,"fine socket");
           } 
        }
        
        
        
      }
  }
  disconnettiTCP(IrcSK);
  free(buffIRCrx);
  free(bufftempIRC);
  free(ChIRC);
  free(NickIRC);
  free(buffIRC);
  return;
errore:
  free(buffIRCrx);
  free(bufftempIRC);
  free(ChIRC);
  free(NickIRC);
  free(buffIRC);
  return;
}
