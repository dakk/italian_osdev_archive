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
#define r_TCP
#include "rete.h"
#define TCP_MTU 1452


#define Tcp(zz,Prt1,Prt2,seq1,seq2,flag,win) \
  *(u_short *)(zz+2)=scbyte(Prt1);*(u_short *)zz=scbyte(Prt2);\
  *(u_int *)(zz+4)=scbyte4(seq1); *(u_int *)(zz+8)=scbyte4(seq2); \
  *(zz+12)=0x50;*(zz+13)=flag;*(u_short *)(zz+14)=win;*(u_int *)(zz+16)=0;

#undef SKK
#define SKK Sok[*sk]

  u_short proxPrt2=1030;
  u_int seq_SIN=0x10a23abc;
  u_int seq_ACK=0;
  struct TSok *Skp; 
  u_char *bufTCP=NULL;
  u_char CC=0;
  

#undef SKK
#define SKK Sok[*sk]
#define SeqSIN scbyte4(*(u_int *)(buffTCP+8))
#define SeqACK scbyte4(*(u_int *)(buffTCP+4))
#define PrtSRC scbyte(*(u_short *)buffTCP)
#define PrtDST scbyte(*(u_short *)(buffTCP+2))
#define FLAg *(buffTCP+13)





u_char connettiTCP(u_char *sk,u_int Ip,u_short Porta) {
  Debug("connettiTCP");  
  ++proxPrt2; seq_SIN++; 
  *sk=registraIP(Porta,proxPrt2,Ip,0,6);/* binding */
  if (*sk==255) Errore(3,"connettiTCP: Sk=255");   /* se sk=0 ritorna 0 */
  SKK->seqSIN=seq_SIN;SKK->seqACK=0;
     	Debug("\nAperto Socket\n"); 
  SKK->stato=1;SKK->timeout=0;SKK->timeout2=adesso; SKK->flag=0;
  while ((SKK->stato!=3)&&(!kbhit())&&(SKK!=NULL)) ;
  if (SKK->stato==3) return 1;  /* se e' andato in stato connesso ritorna 1*/
errore:
  return 0;/* in tutti gli altri casi ritorna 0 */
}
#undef SKK
#define SKK Sok[sk]

u_char ascoltaTCP(u_short Porta) {
   u_char sk;
   if ((sk=registraIP(0,Porta,0,0,6))==255) return 0;
   SKK->stato=8;delay(50);
   while ((SKK!=NULL)&&(SKK->stato!=3)&&(!kbhit())) delay(50);
   if (reteDEBUG&2) printf("ascoltaTCP(): SKK=%x SKK->stato=%u kbhit=%u\n",SKK,SKK->stato,kbhit());
   if (SKK->stato==3) return 1;
   return 0;   
}

u_char disconnettiTCP(u_char sk) {
  Debug("disconnettiTCP\n");  
  if (SKK->stato!=3) return 0;
/*
  Alloca(20);
   Tcp(bufTCP,SKK->PRTsrc,SKK->PRTdst,SKK->seqSIN,SKK->seqACK,FIN|ACK,3000);
  if (!mandaIP(SKK->IPdst,6,bufTCP,20)) Errore(3,"disconnettiTCP: erroreMandaIP(syn)");
*/
  SKK->timeout=0;SKK->stato=4;SKK->timeout2=adesso;
  return 1;
}


u_char riceviTCP(u_char sk,u_int Ip,u_char *buffTCP,u_short lungh) {
   int ttt;
void MandaTcp(u_char flg) {
     setmem(bufTCP,0,60);
     Tcp(bufTCP,SKK->PRTsrc,SKK->PRTdst,SKK->seqSIN,SKK->seqACK,flg,3000);
     if (!mandaIP(SKK->IPdst,6,bufTCP,20)) Errore(6,"rxTCP:errore mandaIP");
     SKK->timeout=adesso;
  errore:     
}
  void Aggiorna() {
     SKK->seqSIN=SeqSIN;    SKK->seqACK=SeqACK;   SKK->timeout=adesso; 
  }

  if (SKK->stato==8) {
    if (reteDEBUG&2) printf("riceviTCP lungh=%u\n",lungh);
     if ((FLAg&(SYN|ACK))==SYN) { /* ricevuto syn */
        SKK->seqACK=SeqACK+1;   
        SKK->seqSIN=++seq_SIN; 
        SKK->IPdst=Ip; SKK->PRTsrc=PrtSRC;SKK->PRTdst=PrtDST;
        MandaTcp(SYN|ACK);
        SKK->stato=2;SKK->flag=0;
        SKK->buflenTX=0;SKK->buflenRX=0;
     } else {
        SKK->seqACK=SeqACK+1;   
        MandaTcp(ACK|RST);   
     }  
  }
  if (SKK->stato==1) {
     if ((FLAg&(SYN|ACK))==(SYN|ACK)) { /* ricevuto synack */
        if (SKK->stato!=1) Errore(4,"rxTCP:SynAck e socket!=1");
        if (SeqSIN!=(SKK->seqSIN+1)) Errore(5,"rxTCP:SynAck e seqSIN!=+1");
        SKK->seqACK=SeqACK+1;   
        SKK->seqSIN=SeqSIN; 
        MandaTcp(ACK);
        SKK->stato=3;
        SKK->buflenTX=0;SKK->buflenRX=0;
     } else {
        SKK->seqACK=SeqACK+1;   
        MandaTcp(ACK|RST);   
     }  
  } else {
    if (FLAg&ACK) {   /* ricevuto ack */
       if (SKK->stato==2)  
          if (((SKK->seqSIN+1)==SeqSIN)&&(SKK->seqACK==SeqACK)) {
             SKK->stato=3; Aggiorna(); goto errore; 
          }
 /*      while (CC&8) delay(50);
       CC=8;*/
       CC=0;Debug("Ricevuto ack  ");
       while (SKK->flag&3) delay(5);
       Debug("2\n");
       SKK->flag|=3;
       if (SeqSIN>SKK->seqSIN) { CC|=2;
/*          for (tTCP=SKK->buflenTX,tTCP1=0;(tTCP1<(SeqSIN-SKK->seqSIN))&&
                (tTCP!=SKK->buflenW_TX);tTCP=(tTCP+1)%3000,tTCP1++) ;
          if ((SKK->buflenTX+=tTCP1)>=3000) SKK->buflenTX-=3000;
          SKK->seqSIN+=tTCP1;*/
/*          printf("ricTCP1: seqSIN=%x skk->seqSIN buflenTX=%u lungh=%u\n",SeqSIN,SKK->seqSIN,SKK->buflenTX,lungh);*/           if (SKK->buflenTX<(SeqSIN-SKK->seqSIN)) { SKK->flag&=0xfc;  Errore(7,"rxTCP:Ack SeqSIN errato");}
          SKK->buflenTX-=(u_short)(SeqSIN-SKK->seqSIN);
          copymem(SKK->bufTX+(u_short)(SeqSIN-SKK->seqSIN),SKK->bufTX,SKK->buflenTX); /* OK */
          SKK->seqSIN=SeqSIN;
/*          printf("ricTCP2: seqSIN=%x skk->seqSIN buflenTX=%u lungh=%u\n",SeqSIN,SKK->seqSIN,SKK->buflenTX,lungh);*/           CC|=4;
       }
       if ((SeqACK<=SKK->seqACK)&&( (ttt=(SKK->seqACK-SeqACK)) <=(lungh-20))) {
/*       if (SeqACK==SKK->seqACK) { */
          if ((SKK->buflenRX+lungh-20-ttt)<3000) {
             CC|=1;
/*        printf("ricTCP3: seqACK=%x skk->seqACK buflenRX=%u lungh=%u\n",SeqACK,SKK->seqACK,SKK->buflenRX,lungh);*/
             copymem(buffTCP+20+ttt,SKK->bufRX+SKK->buflenRX,lungh-20-ttt);
             SKK->seqACK+=(u_int)lungh-20-ttt;
             SKK->buflenRX+=lungh-20-ttt;
/*    printf("ricTCP4: seqACK=%x skk->seqACK buflenRX=%u lungh=%u\n",SeqACK,SKK->seqACK,SKK->buflenRX,lungh);*/
          }  
       } else
       if (SeqACK<SKK->seqACK) CC|=1;
       SKK->flag&=0xfc;
       if ((SKK->stato==3)&&(CC&1)&&(lungh-20)&&(!(FLAg&FIN))) MandaTcp(ACK);
       CC=0;
    } 
    if (FLAg&RST) /* ricevuto rst */
       deregistraIP(sk);
    if (FLAg&FIN) { /* ricevuto fin */
       if ((SKK->stato!=4)&&(SKK->stato!=3))  Errore(4,"rxTCP:FinAck e socket!=3");
       SKK->seqACK++;
       MandaTcp(ACK);
       if (SKK->stato==3) {
          MandaTcp(FIN|ACK);
          SKK->stato=7;
       }
    }
  }
  errore:
  CC=0;
  return 1;
}

u_char nonsoTCP(u_int Ip,u_char *buffTCP,u_short lungh) {
#define lunghTcp ((bufTCP[12]&0xf0)>>2)
  Debug("nonsoTCP\n");  
  if (lungh>3000) return 0;
  setmem(bufTCP,0,lungh);
  copymem(buffTCP,bufTCP,lungh);
  Tcp(bufTCP,PrtSRC,PrtDST,SeqSIN,SeqACK,RST,3000);
  if (SeqACK) { *(u_int *)(bufTCP+8)=scbyte4(SeqACK+1); *(bufTCP+13)|=ACK; }
     if (!mandaIP(Ip,6,bufTCP,lungh))  Errore(6,"nonsoTCP:errore mandaIP RST");
  return 1;
 errore:

  return 0;
}


#undef SKK
#define SKK Sok[tTCP]

#define passato(a) ((SKK->timeout+a)<adesso)
#define passato2(a) ((SKK->timeout2+a)<adesso)
void TimeoutTCP(u_short tTCP) {
  u_short tTCP1;
      if (SKK->stato==1) {          /* se e' in fase di instaurazione */
          if passato(3) { /* se bisogna mandare il pacchetto SYN */
              Debug("MandaSYN");  
              SKK->timeout=adesso;
              setmem(bufTCP,0,20);
              Debug("TimeoutTCP stato=1 \n");  
              Tcp(bufTCP,SKK->PRTsrc,SKK->PRTdst,SKK->seqSIN,SKK->seqACK,SYN,3000);
              mandaIP(SKK->IPdst,6,bufTCP,20);
          } else {
/*          if (reteDEBUG&2) printf("T=%u t2=%u\n",adesso,SKK->timeout2); */
          if passato2(18) 
              {
                 Debug("timeout MandaSYN");  
/*                 if (reteDEBUG&2) printf("Tempo=%u timeout2=%u\n",adesso,SKK->timeout2); */
                 deregistraIP(tTCP);
              }
          }       
       } else
       if (SKK->stato==2) {          /* se e' in fase di listen e ha ricevuto un SIN */
          if passato(3) {    /* se bisogna mandare il pacchetto SINACK */
              SKK->timeout=adesso;
              setmem(bufTCP,0,20);
              Debug("TimeoutTCP stato=2 \n");  
              Tcp(bufTCP,SKK->PRTsrc,SKK->PRTdst,SKK->seqSIN,SKK->seqACK,SYN|ACK,3000);
              mandaIP(SKK->IPdst,6,bufTCP,20);
              SKK->flag=0;
          } else
          if passato2(10) {/* se va in timeout2 riporta il socket a 8 */
              setmem(bufTCP,0,20);
              Debug("Timeout2TCP stato=2 \n");  
              Tcp(bufTCP,SKK->PRTsrc,SKK->PRTdst,SKK->seqSIN,SKK->seqACK,RST|ACK,3000);
              mandaIP(SKK->IPdst,6,bufTCP,20);
              SKK->IPdst=0;SKK->PRTdst=0;SKK->timeout=0;SKK->timeout2=0;
              SKK->stato=8;
          }
       } else
       if (SKK->stato==4) {          /* se e' in fase di chiusura */
          if passato(3) { /* se bisogna mandare il pacchetto FINACK */
              SKK->timeout=adesso;
              setmem(bufTCP,0,20);
              Debug("TimeoutTCP stato=4 \n");  
              Tcp(bufTCP,SKK->PRTsrc,SKK->PRTdst,SKK->seqSIN,SKK->seqACK,FIN|ACK,3000);
              mandaIP(SKK->IPdst,6,bufTCP,20);
          } else
          if passato2(18) {
              setmem(bufTCP,0,20);
              Debug("Timeout2TCP stato=4 \n");  
              Tcp(bufTCP,SKK->PRTsrc,SKK->PRTdst,SKK->seqSIN,SKK->seqACK,RST|ACK,3000);
              mandaIP(SKK->IPdst,6,bufTCP,20);
              deregistraIP(tTCP);
          }
       } else
       if ((SKK->stato==3)&&(SKK->buflenTX)&&
          passato(5)) {
            Debug("TimeoutTCP stato=3");  
            while (SKK->flag&1) delay(5);
            SKK->flag|=1;
            Debug(" 1");  
            setmem(bufTCP,0,TCP_MTU+60);
            SKK->timeout=adesso;
            if (SKK->buflenTX<TCP_MTU) tTCP1=SKK->buflenTX; else tTCP1=TCP_MTU;
            copymem(SKK->bufTX,bufTCP+20,tTCP1);
            Tcp(bufTCP,SKK->PRTsrc,SKK->PRTdst,SKK->seqSIN,SKK->seqACK,PSH|ACK,3000);
            mandaIP(SKK->IPdst,6,bufTCP,tTCP1+20);
            SKK->flag&=0xfe;
            Debug(" 0\n");  
       } else  
       if (((SKK->stato==7)||(SKK->stato==4))&&(!SKK->buflenRX)&&passato2(13))
            deregistraIP(tTCP);
}

void Init_tcp() {
   if (bufTCP==NULL) bufTCP=malloc(3000);
}
