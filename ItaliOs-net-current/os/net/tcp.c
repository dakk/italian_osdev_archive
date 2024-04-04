#define r_TCP
#include "rete.h"

  int t;
  u_int ll;
  u_char *bufTCP;
  u_char *bufTCPsend;
  u_char *bufTCP2;
  unsigned short *TCPl1;
  unsigned int *TCPindi;
  unsigned short *TCPl12;
  unsigned int *TCPindi2;
  u_short proxPrt2=1030;
  u_int seqSIN=0x10a23abc;
  u_int seqACK=0;
  struct TSok *Skp; 

#define Tcp(Prt1,Prt2,seq1,seq2,flag,win) TCPl1[1]=scbyte(Prt1);TCPl1[0]=scbyte(Prt2);\
  TCPindi[1]=scbyte4(seq1); TCPindi[2]=scbyte4(seq2); \
  *(((u_char *)TCPl1)+12)=0x50;*(((u_char *)TCPl1)+13)=flag;TCPl1[7]=win;TCPindi[4]=0;

#define FLAg *(((u_char *)TCPl1)+13)
#define SKT(n,ippi,prt1,prt2,seq1,seq2) \
 Sok[n].IP=ippi;Sok[n].PRTsrc=prt1;Sok[n].PRTdst=prt2;\
 Sok[n].seqTX=seq1;Sok[n].seqRX=seq2; 



#define FIN 0x01
#define SYN 0x02
#define RST 0x04
#define PSH 0x08
#define ACK 0x10
#define URG 0x20

#define SKK Sok[sk]
#define Errore(a) { printf("%s\n",a); goto errore;}

u_char TxTCP(u_char sk,u_char *buff1,u_short lungh) {
  if (reteDEBUG&2) printf("TXTCP\n");  
  bufTCPsend=malloc(lungh+20);
  TCPl1=(unsigned short *)bufTCPsend;
  TCPindi=(unsigned int *)bufTCPsend;
  setmem(bufTCPsend,0,lungh+20);
  copymem(buff1,bufTCPsend+20,lungh);
  if ((SKK.buflen+lungh)>=3000) Errore("mandaTCP: SKK.buflen+lungh>3000");
  Tcp(SKK.PRTsrc,SKK.PRTdst,SKK.seqSIN+SKK.buflen,SKK.seqACK,ACK|PSH,3000);
  copymem(buff1,SKK.bufTX+SKK.buflen,lungh);SKK.buflen+=lungh;
  if (!mandaIP(SKK.IPsrc,6,bufTCPsend,20+lungh)) Errore("mandaTCP: erroreMandaIP(ack+psh)");
  free(bufTCPsend);
  return 1;
errore: 
  free(bufTCPsend);
  return 0;  
}



/* applicazione
   .connettiTCP(&sk,Ip,Porta,&applicazione)
     alloca bufTCP
     costruisci SYN
     manda pacchetto
     sk=registraIP(&applicazione,porta,proxprt2,Ip,0,6)
     se sk=0 ritorna 0
     SKK.stato=1 ritorna 1
*/

u_char connettiTCP(u_char *sk,u_int Ip,u_short Porta) {

  if (reteDEBUG&2) printf("connettiTCPn");  
  bufTCP=malloc(10000); 
  TCPl1=(unsigned short *)bufTCP;
  TCPindi=(unsigned int *)bufTCP;
  setmem(bufTCP,0,5000);
   Tcp(Porta,++proxPrt2,seqSIN=0xab232ba4,seqACK=0,SYN,3000);/* crea SYN */
  *sk=registraIP(Porta,proxPrt2,Ip,0,6);
  if (*sk==255) Errore("connettiTCP: Sk=255");   /*     se sk=0 ritorna 0 */
  if (reteDEBUG&2) printf("\nAperto Socket %u\n",*sk);
  Sok[*sk].seqSIN=seqSIN;  Sok[*sk].seqACK=seqACK;
  Sok[*sk].stato=1;          /*     SKK.stato=1 ritorna 1 */
/*     manda pacchetto */
  if (!mandaIP(Ip,6,bufTCP,20)) Errore("connettiTCP: erroreMandaIP(syn)");
  if (reteDEBUG&2) printf("Attesa connessione\n");
  while ((Sok[*sk].stato!=3)&&(Sok[*sk].stato!=0)&&(!kbhit())) ;
  if (reteDEBUG&2) printf("Connesso\n");
  if (Sok[*sk].stato!=3) goto errore;
  free(bufTCP);  
  return 1;
 errore:
  free(bufTCP);
  return 0;
}

u_char disconnettiTCP(u_char sk) {
  if (reteDEBUG&2) printf("disconnettiTCP\n");  
  bufTCP=malloc(5000); /* crea buffer */
  TCPl1=(unsigned short *)bufTCP;
  TCPindi=(unsigned int *)bufTCP;
  setmem(bufTCP,0,5000);
   Tcp(SKK.PRTsrc,SKK.PRTdst,SKK.seqSIN,SKK.seqACK,FIN|ACK,3000);
  Sok[sk].stato=4;
/*     manda pacchetto */
  if (!mandaIP(SKK.IPsrc,6,bufTCP,20)) Errore("disconnettiTCP: erroreMandaIP(syn)");
/*  while (Sok[sk].stato==4) ;            commentato causa fonte di blocco
  if (Sok[sk].stato!=0) goto errore;*/

  free(bufTCP);  
  return 1;
 errore:
  free(bufTCP);
  return 0;
}

u_short RxTCP(u_char sk,u_char *buffTCP,u_short lungh) {
  if (reteDEBUG&2) printf("RxTCP lungh=%u\n",lungh);  
  while ((!SKK.buflenRX)&&(RETE_)&&(SKK.stato==3)) ;
  if (SKK.buflenRX) {
     if (lungh>=SKK.buflenRX) {
        copymem(SKK.bufRX,buffTCP,SKK.buflenRX);
        lungh=SKK.buflenRX;SKK.buflenRX=0;
     } else {
        copymem(SKK.bufRX,buffTCP,lungh);
        copymem(SKK.bufRX+lungh,SKK.bufRX,SKK.buflenRX-lungh);
        lungh=SKK.buflenRX;SKK.buflenRX-=lungh;
     }
     return lungh;
     
  } else return 0;
  
}


u_char riceviTCP(u_char sk,u_int Ip,u_char *buffTCP,u_short lungh) {
  if (reteDEBUG&2) printf("riceviTCP lungh=%u\n",lungh);  
  bufTCP2=malloc(lungh); 
  copymem(buffTCP,bufTCP2,lungh);
/*  printf("Stato Sok=%u %u\n",sk,SKK.stato); */
  TCPl1=(unsigned short *)buffTCP;
  TCPindi=(unsigned int *)buffTCP;
  if ((FLAg&(SYN|ACK))==(SYN|ACK)) { /* ricevuto synack :)) */
     if (SKK.stato!=1) Errore("rxTCP:SynAck e socket!=1");
     if (scbyte4(TCPindi[2])!=(SKK.seqSIN+1)) Errore("rxTCP:SynAck e seqSIN!=+1");
     SKK.seqACK=scbyte4(TCPindi[1])+1;SKK.seqSIN=scbyte4(TCPindi[2]); 
     Tcp(SKK.PRTsrc,SKK.PRTdst,SKK.seqSIN,SKK.seqACK,ACK,3000);
     if (!mandaIP(SKK.IPsrc,6,buffTCP,20)) Errore("rxTCP:SynAck errore mandaIP");
     SKK.stato=3;
  } else
  if ((FLAg&(FIN|ACK))==(FIN|ACK)) { /* ricevuto finack :)) */
     if ((SKK.stato!=4)&&(SKK.stato!=3))  Errore("rxTCP:FinAck e socket!=3");
/*   commentati i controlli sui puntatori perche' bloccanti
     if (scbyte4(TCPindi[2])!=(SKK.seqSIN)) Errore("rxTCP:FinAck errore seqSIN");
     if (scbyte4(TCPindi[1])!=(SKK.seqACK)) Errore("rxTCP:FinAck errore seqACK");
*/     
     SKK.stato=0;
     Tcp(SKK.PRTsrc,SKK.PRTdst,SKK.seqSIN,++SKK.seqACK,ACK,3000);
     if (!mandaIP(SKK.IPsrc,6,buffTCP,20))  Errore("rxTCP:FinAck errore mandaIP ACK");
     Tcp(SKK.PRTsrc,SKK.PRTdst,SKK.seqSIN,SKK.seqACK,FIN|ACK,3000);
     if (!mandaIP(SKK.IPsrc,6,buffTCP,20))  Errore("rxTCP:FinAck errore mandaIP FinACK");
     deregistraIP(sk);
  } else
  if ((FLAg&(PSH|ACK))==(PSH|ACK)) { /* ricevuto pshack :)) */
/*     printf("Dati in RX pshack\n"); */
     if (SKK.stato!=3) goto errore;
     if (scbyte4(TCPindi[1])!=(SKK.seqACK)) Errore("rxTCP:PshAck errore seqACK");
     if (TCPindi[2]!=scbyte4(SKK.seqSIN)) {
        if ((scbyte4(TCPindi[2])-SKK.seqSIN)>(u_int)SKK.buflen) {
          Errore("rxTCP:PshAck errore seqACK fuorirange");
        }  
        SKK.buflen-=(u_short)(scbyte4(TCPindi[2])-SKK.seqSIN);
        SKK.seqSIN=scbyte4(TCPindi[2]);
/*        fprintf(stderr,"BufLen=%u\n",SKK.buflen); */
        copymem(SKK.bufTX+(u_short)(scbyte4(TCPindi[2])-SKK.seqSIN),SKK.bufTX,
               SKK.buflen); /* OK */
     }
     if ((SKK.buflenRX+lungh-20)<3000) {
       copymem(bufTCP2+20,SKK.bufRX+SKK.buflenRX,lungh-20);
       SKK.seqACK+=(u_int)lungh-20;
       SKK.buflenRX+=lungh-20;
       Tcp(SKK.PRTsrc,SKK.PRTdst,SKK.seqSIN,SKK.seqACK,ACK,3000);
       if (!mandaIP(SKK.IPsrc,6,buffTCP,20)) Errore("rxTCP:PshAck errore mandaIP");
/*       fprintf(stderr,"%s\n",bufTCP2+20); */
/*     SKK.ricez(sk,bufTCP2+20,lungh-20); */
     }
  } else
  if ((FLAg&ACK)==ACK) { /* ricevuto ack :)) */
     if (SKK.stato!=3)  Errore("rxTCP:Ack e socket!=3");
     if (TCPindi[2]!=scbyte4(SKK.seqSIN)) {
        if ((scbyte4(TCPindi[2])-SKK.seqSIN)>(u_int)SKK.buflen) {
          Errore("rxTCP:Ack errore seqACK fuorirange");
        }  
        SKK.buflen-=(u_short)(scbyte4(TCPindi[2])-SKK.seqSIN);
        copymem(SKK.bufTX+(u_short)(scbyte4(TCPindi[2])-SKK.seqSIN),SKK.bufTX,
               SKK.buflen); /* OK */
        SKK.seqSIN=scbyte4(TCPindi[2]);
/*        fprintf(stderr,"BufLen=%u\n",SKK.buflen); */
     }
     if (TCPindi[1]!=scbyte4(SKK.seqACK))
      if ((SKK.buflenRX+lungh-20)<3000) {
       copymem(bufTCP2+20,SKK.bufRX+SKK.buflenRX,lungh-20);
       SKK.seqACK+=(u_int)lungh-20;
       Tcp(SKK.PRTsrc,SKK.PRTdst,SKK.seqSIN,SKK.seqACK,ACK,3000);
       if (!mandaIP(SKK.IPsrc,6,buffTCP,20)) Errore("rxTCP:PshAck errore mandaIP");
/*       fprintf(stderr,"%s\n",bufTCP2+20);
       SKK.ricez(sk,bufTCP2+20,lungh-20);*/
      }
  }


  free(bufTCP2);
  return 1;
 errore:
/*  printf("%0x %0x SeqSIN=%x SeqACK=%x\n",scbyte4(TCPindi[2]),scbyte4(TCPindi[1]),SKK.seqSIN,SKK.seqACK); */
  free(bufTCP2);
  return 0;
}     

u_char nonsoTCP(u_int Ip,u_char *buffTCP,u_short lungh) {
  if (reteDEBUG&2) printf("nonsoTCP\n");  
  bufTCP2=malloc(lungh); 
  copymem(buffTCP,bufTCP2,lungh);
  TCPl12=(unsigned short *)buffTCP;
  TCPindi2=(unsigned int *)buffTCP;
  TCPl1=(unsigned short *)bufTCP2;
  TCPindi=(unsigned int *)bufTCP2;
  TCPl1[0]=TCPl12[1];  TCPl1[1]=TCPl12[0];
  TCPindi[1]=TCPindi2[2];  TCPindi[2]=TCPindi2[1];
  *(((u_char *)TCPl1)+13)=ACK|RST;TCPl1[7]=3000;

     if (!mandaIP(Ip,6,bufTCP2,20))  Errore("nonsoTCP:errore mandaIP RST");

  free(bufTCP2);
  return 1;
 errore:

  free(bufTCP2);
  return 0;
}

