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
#define r_MAIN
#include "rete.h"
#include <stdio.h>

#ifndef VER
#define VER "------"
#endif

 extern void update_HW_cursor();
 u_short leds=0;
 u_int   ledsold=0;
 
 unsigned int porta=0,porta1=0x3f8;
 unsigned char RETE_=0;
 unsigned short reteDEBUG=0;
 u_char *videoMEM=(u_char *)0xB8000;
 u_char interruptRETE=4;
 extern u_short bufLOC1,bufLOC2;
 extern u_short bufTX1,bufTX2;
 extern u_short buffRX2,npunta,npunta2;
 u_char NumTEL[20],UserID[40],PassWD[40],CRLF=13;
 u_char asyncmapno=0;
 u_char NetX=0,NetY=0; 
 u_char Netcolore=0x17;
 extern int abs_y;
 u_short Errori[10];
 extern u_int PPPtimeout;
 char *barra=".\\|/-",nbarra=0,nbarra1=0;
 u_int tbarra=0;
 int adesso;
 extern int VecTempo;
 
 
void NetPrintf(char *strs) {
 char *dest;
 dest = (char *)0xb8000+2*NetX+(abs_y-24)*160;
 while (*strs) { 
  *dest++ = *strs++;
  *dest++ = Netcolore;
  NetX++;
 }
}
void NetPutc(char strs) {
 char *dest;
 dest = (char *)0xb8000+2*NetX+(abs_y-24)*160;
  *dest++ = strs;
  *dest++ = Netcolore;
  NetX++;
}

void NetPutInt(u_int strs) {
 char *dest;
 char nettmp[20];
 u_char tt;
 dest = (char *)0xb8000+2*NetX+(abs_y-24)*160;
 for (tt=0;(tt<20)&&(strs);tt++) {
   nettmp[tt]=48+(strs%10);strs/=10;
 };
 while (tt--) {
  *dest++ = nettmp[tt];
  *dest++ = Netcolore;
  NetX++;
 }
}

void reteRX() {
 int tRete;

void barraRETE() {

    if (!(reteDEBUG&2))
    if (adesso!=tbarra) { 
     tbarra=adesso;
     NetX=0;NetY=0;Netcolore=0x17;
     NetPrintf(" Net:Ip=");NetPrintf(NUMinIP(IPlocale)); NetX++;
     if ((stato&0xf)==0xf) NetPrintf("P"); else NetPrintf("p");
     if ((stato&0x30)==0x30) NetPrintf("P"); else NetPrintf("p");
     if ((stato&0x3c0)==0x3c0) NetPrintf("P"); else NetPrintf("p");
     NetPrintf("(");
     tRete=(int)inb(porta1+4);
     if (tRete&1) NetPrintf("8"); else NetPrintf("."); /*DTR */
     if (tRete&2) NetPrintf("5"); else NetPrintf(".");/* RTS */
     tRete=(int)inb(porta1+6);
     if (tRete&0x10) NetPrintf("6"); else NetPrintf(".");/* CTS */
     if (tRete&0x20) NetPrintf("7"); else NetPrintf(".");/* DSR */
     if (tRete&0x80) NetPrintf("9"); else NetPrintf(".");/* DCD */
     
     NetPrintf(")_");
     if (leds&2) Netcolore=0x1E; else Netcolore=0x17; NetPrintf("Tx");
     if (leds&1) Netcolore=0x1E; else Netcolore=0x17; NetPrintf("Rx");
     Netcolore=0x17;
     NetPrintf(" IP_");
     if (leds&16) Netcolore=0x1E; else Netcolore=0x17; NetPrintf("Tx");
     if (leds&8) Netcolore=0x1C; else 
     if (leds&4) Netcolore=0x1E; else
             Netcolore=0x17; NetPrintf("Rx");
     Netcolore=0x17;        
     NetPrintf(" Sok=");
     for (tRete=0;tRete<10;tRete++)
        if (Sok[tRete]!=NULL) {
          if (Sok[tRete]->buflenRX>2500)
             Netcolore=0x47; else Netcolore=0x17;
          NetPutc(Sok[tRete]->stato+48); 
/*          if ((Sok[tRete]->stato!=3)&&(Sok[tRete]->stato!=6)
            &&((adesso-Sok[tRete]->timeout)>15)) {
              Debug("timeoutReteRX\n");
              deregistraIP(tRete);
            }  */
        } else NetPutc('.');
     Netcolore=0x17;        
     NetX++;
     for (tRete=0;tRete<10;tRete++) {
      NetPutInt(Errori[tRete]);NetPrintf(",");
     } 
     NetPrintf(" ");
/*     NetPutInt(adesso); */
     NetPutc(*(barra+nbarra));
     NetPutc(*(barra+nbarra1));nbarra1=nbarra1+1-4*(nbarra1>3);
     ledsold=0;leds=0;
    } 

}


 tbarra=adesso;
 Init_tcp();
 for (tRete=0;tRete<10;tRete++) Errori[tRete]=0; 
 RETE_=1;  
 VecTempo=100000;
 while (RETE_)
  {
    adesso=Tempo();
    riceviPPP();    /* scansione dell' interfaccia ppp */
    riceviLOCALE(); /* scansione dell' interfaccia locale */
    GestioneSocket();/* gestione della trasmissione dei socket e timeout */
    barraRETE();
  }
 stato=0; 
 outb(inb(porta1+4)&0xfc,porta1+4); /* RTS e DTR OFF */
 IPlocale=0;IPremoto=0;IPDNS1=0;IPDNS2=0;
 suicide();
/*  if ((ch1=uart_getchar_interrupt(porta))!=0xf1) putchar(ch1); */
/*        uart_putchar_polling(0,ch); */
}


void statoRETE() {
  int t;
  printf("\nStato RETE:");
  if (RETE_) printf("Attiva"); else printf("non attiva");
  printf("\nint_ppp  IP:%s,",NUMinIP(IPlocale));
  printf("%s  ",NUMinIP(IPremoto));
  printf("DNS:%s,%s\n",NUMinIP(IPDNS1),NUMinIP(IPDNS2));
  printf("    TX:%u %u RX:%u (%u %u)  ",bufTX1,bufTX2,buffRX2,npunta,npunta2);
  printf("        stato=");
  if ((stato&0xf)==0xf) printf("LCP "); else printf("lcp ");
  if ((stato&0x30)==0x30) printf("PAP "); else printf("pap ");
  if ((stato&0x3c0)==0x3c0) printf("IPCP "); else printf("ipcp ");
  printf(" ");
  t=(int)inb(porta1+4);
  if (t&1) printf(" DTR");
  if (t&2) printf(" RTS");
  t=(int)inb(porta1+6);
  if (t&0x10) printf(" CTS");
  if (t&0x20) printf(" DSR");
  if (t&0x40) printf(" RI");
  if (t&0x80) printf(" DCD");

  printf("\nint_locale %u %u \n",bufLOC1,bufLOC2);
  printf(" Socket:\n");
  for (t=0;t<10;t++)
    if (Sok[t]!=NULL)
      printf("Sok%u: stato=%u proto=%u src=%s:%u dst=%s:%u\n",
      t,Sok[t]->stato,Sok[t]->proto,NUMinIP(Sok[t]->IPdst),Sok[t]->PRTsrc,
      NUMinIP(Sok[t]->IPsrc),Sok[t]->PRTdst);
}

void avviaPPP() {
     inizializzaPPP();
     add_task((void *)reteRX,"ReteRX");
/*     ogni(0,5000); */
     delay(2000);
     if (stato==0) A_OPEN_PPP;
     printf("\n");
}

void rete() {
 u_char buffRete[50];
 unsigned short velocita=38400;
 set_back_color(NERO);cls();set_text_color(VERDE);
 statoRETE();
 *buffRete=0;
 setmem(buffRete,0,50);
 while (strcmp(buffRete, "torna")) {
  printf("Help: PPP irc stato opzioni cancsk term ping debug(sfpPat) telnet connetti torna giu traduci\n"); 
 setmem(buffRete,0,50);
  printf("\nItaliOs_rete(%s) :> ",VER);getstring(buffRete);printf("\n");
  if (!strcmp(buffRete, "ping")) {
     		Ping();
	} else
/*  if (!strcmp(buffRete, "provamodem")) {
                provamodem();
	} else*/
  if (!strcmp(buffRete, "term")) {
     setmem(buffRete,0,50);
     printf("\nNumero Seriale da usare:[1..4] (%u)",porta);getstring(buffRete);
     if (*buffRete)
      if (!(porta=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nVelocita': (%u)",velocita);getstring(buffRete);
     if (*buffRete)
      if (!(velocita=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nInterrupt: (%u)",interruptRETE);getstring(buffRete);
     if (*buffRete)
      if (!(interruptRETE=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nCRLF= (%u)",CRLF);getstring(buffRete);
     if (*buffRete)
      if (!(CRLF=atou(buffRete))) return;
     switch(porta) {
      case 1: porta1=0x3f8;break;
      case 2: porta1=0x2f8;break;
      case 3: porta1=0x3e8;break;
      case 4: porta1=0x2e8;break;
     }
     printf("\nporta=seriale%u (0x%x) irq=%u vel=%u\n",porta,porta1,interruptRETE,velocita);
     if ((uart_open2(porta1, INTR, velocita,NO_PARITY,BITS_8_PER_WORD,STOP_BIT_1)) == NO_DEVICE) {
 		printf("\nERRORE: impossibile aprire COM1");
 		return;
     }
     setta_irq(interruptRETE);
     printf("rete: porta=%u\n",porta);
     		ParlaModem();
	} else
  if (str_str(buffRete, "opzioni ")==1) {
#define opz(za,zb) if (str_str(buffRete,za)) reteDEBUG=(reteDEBUG&zb?reteDEBUG&(!zb):reteDEBUG|zb)
            opz("crtscts",128);
	} else
  if (str_str(buffRete, "opzioni")==1) {
        printf("Opzioni disponibili: crtscts\n");
        if (reteDEBUG&1) printf(" sniff");
        if (reteDEBUG&2) printf(" funzioni");
        if (reteDEBUG&4) printf(" ppp_sniff");
        if (reteDEBUG&8) printf(" Ppp_NoSniffIniz");
        if (reteDEBUG&16) printf(" AsyncmapNonNegoziata");
        if (reteDEBUG&128) printf(" crtscts");
	} else
  if (!strcmp(buffRete, "stato")) {
               	statoRETE();
	} else
  if (str_str(buffRete, "irc")==1) {
/*                if (noPZ()==2) irc(IPinNUM(oPZ[0]),atou(oPZ[1])); else */
     		irc(0,0);
	} else
  if (!strcmp(buffRete, "cancsk")) {
     u_short t;
                for (t=0;t<10;t++) 
                 if (Sok[t]!=NULL) deregistraIP(t);
	} else
  if (!strcmp(buffRete, "giu")) {
                 stato=0; RETE_=0;
                 outb(inb(porta1+4)&0xfc,porta1+4); /* RTS e DTR OFF */
	} else
  if (str_str(buffRete,"debug")==1) {
#define opzione(za,zb) if (buffRete[t]==za) reteDEBUG=(reteDEBUG&zb?reteDEBUG&(!zb):reteDEBUG|zb)
     u_short t;
     	for (t=5;buffRete[t];t++) {
          opzione('s',1);
          opzione('f',2);
          opzione('p',4);
          opzione('P',8);
          opzione('a',16); 
        } 
        if (reteDEBUG&1) printf(" sniff");
        if (reteDEBUG&2) printf(" funzioni");
        if (reteDEBUG&4) printf(" ppp_sniff");
        if (reteDEBUG&8) printf(" Ppp_NoSniffIniz");
        if (reteDEBUG&16) printf(" AsyncmapNonNegoziata");
        if (reteDEBUG&128) printf(" crtscts");
        printf("\n");
     } else
  if (str_str(buffRete, "telnet")==1) {
               	telnet(0,0);
	} else
  if (str_str(buffRete, "seriali")==1) {
                seriali();
	} else
  if (!strcmp(buffRete, "tick")) {
               printf("%u PPPtimeout=%u\n",adesso,PPPtimeout);
	} else 
  if (!strcmp(buffRete, "ascolta")) {
               TelnetRX(80);
	} else 
  if (!strcmp(buffRete, "PPP")) {
               if (!RETE_) avviaPPP();
	} else
  if (!strcmp(buffRete, "traduci")) {
               setmem(buffRete,0,50);
               printf("\nHost: ");getstring(buffRete);printf("->%s\n",buffRete);
               printf("%s\n",NUMinIP(TraduciDNS(buffRete,IPDNS1)));
	} else
  if (!strcmp(buffRete, "connetti")) {
    if (RETE_) RETE_=0; else {
     printf("\nNumero Seriale da usare:[1..4] (%u)",porta);getstring(buffRete);
     if (*buffRete)
      if (!(porta=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nVelocita': (%u)",velocita);getstring(buffRete);
     if (*buffRete)
      if (!(velocita=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nInterrupt: (%u)",interruptRETE);getstring(buffRete);
     if (*buffRete)
      if (!(interruptRETE=atou(buffRete))) return;
     setmem(buffRete,0,50);

     setmem(buffRete,0,50);
     printf("\nNumero Telefonico: (%s)",NumTEL);getstring(buffRete);printf("\n");
     if (*buffRete) copymem(buffRete,NumTEL,strlen(buffRete)+1);
     setmem(buffRete,0,50);
     setmem(UserID,0,40);
     printf("\nUserID: (%s)",UserID);getstring(buffRete);printf("\n");
     if (*buffRete) copymem(buffRete,UserID,strlen(buffRete)+1);
     setmem(PassWD,0,40);
     printf("\nPassWD: ");getstring(PassWD);
     setmem(buffRete,0,50);
     printf("\nCRLF= (%u)",CRLF);getstring(buffRete);
     if (*buffRete)
      if (!(CRLF=atou(buffRete))) return;
     switch(porta) {
      case 1: porta1=0x3f8;break;
      case 2: porta1=0x2f8;break;
      case 3: porta1=0x3e8;break;
      case 4: porta1=0x2e8;break;
     }
     if ((porta = uart_open2(porta1, INTR, velocita,NO_PARITY,BITS_8_PER_WORD,STOP_BIT_1)) == NO_DEVICE) {
 		printf("\nERRORE: impossibile aprire COM1");
 		return;
     }
     setta_irq(interruptRETE);
     printf("\nporta=seriale%u (0x%x) irq=%u vel=%u\n",porta,porta1,interruptRETE,velocita);
     printf("rete: porta=%u\n",porta);
     if (connettiMDM(NumTEL)) printf("Connesso"); else 
       {printf("errore");goto errore;}
     avviaPPP();
 }
 }
} 


 errore:
}


