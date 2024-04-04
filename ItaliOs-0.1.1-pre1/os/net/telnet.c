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

#define r_TELNET
#include "rete.h"


void TelnetRX(u_short Porta) {
  u_char TelSK=0;
  u_char *buffTELNET;
  u_char ch;
  int tTEL=0;
  
  u_short opzTELNET=0;
  if (reteDEBUG&2) printf("telnet\n");  
  buffTELNET=malloc(200);
  if (!Porta) {
   printf("Porta dest:");getstring(buffTELNET);printf("\n");
   if (!(Porta=atou(buffTELNET))) goto errore;
  }
   opzTELNET=0;
  if (ascoltaTCP(Porta)==0) { printf("fallito\n"); return; }
  printf("Connesso\n");
  ch=0;
  while ((ch!='\033')&&(Sok[TelSK]!=0)) {
      if ((tTEL=ricevi_str(TelSK,buffTELNET,198))!=0) {
	printf("%s\n",buffTELNET);
      }
      if (kbhit()) {
        if (opzTELNET&1) {
           ch=buffTELNET[0]=getch();buffTELNET[1]=0;
           if (ch!='\033')
             if (!trasmetti(TelSK,buffTELNET,1)) Errore(8,"fine socket");
        } else {
           printf(">");getstring(buffTELNET);ch=*buffTELNET;printf("\n");
           if (ch!='\033')
             if (!trasmetti_str(TelSK,buffTELNET)) Errore(8,"fine socket");
        } 
        if (opzTELNET&2) printf("%s",buffTELNET);
      }
  }
  disconnettiTCP(TelSK);
  free(buffTELNET);
  return;
  errore:
  free(buffTELNET);
  return;
   
}



void telnet(u_int Ip,u_short Porta) {
  u_char TelSK=0;
  u_char *buffTELNET;
  u_char ch;
  int tTEL=0;
  u_char opzTELNET=0;
  
  if (reteDEBUG&2) printf("telnet\n");  
  buffTELNET=malloc(200);
  if (!Ip) {
   printf("\ndestinazione:");getstring(buffTELNET);
   if (!(Ip=host(buffTELNET))) goto errore;
   printf("  Ip=%s\n",NUMinIP(Ip));
  } 
  if (!Porta) {
   printf("Porta dest:");getstring(buffTELNET);printf("\n");
   if (!(Porta=atou(buffTELNET))) goto errore;
  }
   opzTELNET=0;
   printf("\nTipo carattere o linea? (c l):");getstring(buffTELNET);
   if (*buffTELNET=='c') {
     opzTELNET|=1;
     printf("\nEcho locale? (s n):");getstring(buffTELNET);printf("\n");
     if (*buffTELNET=='s') opzTELNET|=2;
   }  
  if (!connettiTCP(&TelSK,Ip,Porta)) Errore(8,"telnet: errore connessione");
  printf("Connesso\n");
  ch=0;
  while ((ch!='\033')&&(Sok[TelSK]!=0)) {
      if ((tTEL=ricevi_str(TelSK,buffTELNET,198))!=0) {
	printf("%s\n",buffTELNET);
      }
      if (kbhit()) {
        if (opzTELNET&1) {
           ch=buffTELNET[0]=getch();buffTELNET[1]=0;
           if (ch!='\033')
             if (!trasmetti(TelSK,buffTELNET,1)) Errore(8,"fine socket");
        } else {
           printf(">");getstring(buffTELNET);ch=*buffTELNET;printf("\n");
           if (ch!='\033')
             if (!trasmetti_str(TelSK,buffTELNET)) Errore(8,"fine socket");
        } 
        if (opzTELNET&2) printf("%s",buffTELNET);
      }
  }
  disconnettiTCP(TelSK);
  free(buffTELNET);
  return;
  errore:
  free(buffTELNET);
  return;
}
