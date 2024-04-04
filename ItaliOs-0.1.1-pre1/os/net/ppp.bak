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


/* ppp.c: by miki00


   0x7f,0xff,0x03,protocol[2]


  status: PAP,


#include "io.h"
*/
#define r_PPP
#include "rete.h"

#define ppp_flag (u_char)*ppp
#define ppp_addr (u_char)*(ppp+1)
#define ppp_control (u_char)*(ppp+2)
#define ppp_proto *(u_short *)(ppp+3)
#define ppp_code (u_char)*(ppp+5)
#define ppp_id (u_char)*(ppp+6)
#define ppp_len *(u_short *)(ppp+7)
#define ppp_dati (ppp+9)
#define ppp_fcs *(u_short *)(ppp+scbyte(ppp_len)+5)

#define ppplog_addr (u_char)*(ppplog+1)
#define ppplog_control (u_char)*(ppplog+2)
#define ppplog_proto *(u_short *)(ppplog+3)
#define ppplog_code (u_char)*(ppplog+5)
#define ppplog_id (u_char)*(ppplog+6)
#define ppplog_len *(u_short *)(ppplog+7)
#define ppplog_dati (ppplog+9)
#define ppplog_fcs *(u_short *)(ppplog+scbyte(ppplog_len)+5)

#define pppout_flag (u_char)*pppout
#define pppout_addr (u_char)*(pppout+1)
#define pppout_control (u_char)*(pppout+2)
#define pppout_proto *(u_short *)(pppout+3)
#define pppout_code (u_char)*(pppout+5)
#define pppout_id (u_char)*(pppout+6)
#define pppout_len *(u_short *)(pppout+7)
#define pppout_dati (pppout+9)
#define pppout_fcs *(u_short *)(pppout+scbyte(pppout_len)+5)

#define IP_LUNGH scbyte(*(u_short *)(pppout+7))

#ifndef stderr
#define stderr stdout 
#endif

/*
#define Log nlog+=snprintf(log+nlog,200,
#define Scrivilog {printf("%s\n",log);nlog=0;}
*/
#define Log printf(
#define Scrivilog printf("\n");

  int t;

  u_int PPPtimeout=0;
  
/*  u_char F_PPP=0; */
  u_char lastCH=0;
  u_char *ppp,*pppout,*ppplog;
  u_char *buff=0;
  u_char *buffRX=0;
  u_short buffRX1,buffRX2=0;
  int punta[]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},npunta=0,npunta2=0,puntaP=-1;
  char log[200],nlog=0,CLK=0;
  u_int stato=0;
  u_int IPlocale,IPremoto;
  u_int IPDNS1,IPDNS2;
  u_int asyncmap=0xffffffff,asyncmap2;
  u_char *bufTX=0;
  u_short bufTX1,bufTX2=0;
  u_int ritorno;  
  u_char ppp_confack,ppp_confnack,ppp_confrej;
  extern u_char asyncmapno;
  
  static unsigned short fcstab[256] = {
        0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
        0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
        0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
        0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
        0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
        0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
        0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
        0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
        0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
        0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
        0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
        0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
        0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
        0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
        0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
        0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
        0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
        0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
        0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
        0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
        0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
        0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
        0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
        0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
        0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
        0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
        0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
        0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
        0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
        0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
        0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
        0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
   };

u_char leggiPPP() {	/* funzione che legge e marca un frame in Rx */
  u_char ch;
  ch=((ritorno=leggibyteseriale())&0xff);/* leggibyte da seriale */
  if (ritorno!=NO_DATA) {	/* se e' stato letto un byte */
  if (ch==0x7e) {  		 /* se e' un flag */
   if (lastCH!=0x7e)              /* se il penultimo byte non e' un flag */
    if (puntaP>=0) {		   /* se e' un frame completo */
       punta[npunta]=puntaP;	   /* allora trasmetti */
       npunta=(npunta+1)%10;
    }
    if (buffRX2>(10000-3000)) buffRX2=0;
    puntaP=buffRX2;                    /* marca frame successivo */
  }
  if (ch==0x7d) {		/* se e' un codice di escape */
    while ((ritorno=leggibyteseriale())==NO_DATA);
      ch=(ritorno&0xff)^0x20;	/* leggi il prox byte e fai xor 0x20 */
  }
  buffRX[buffRX2]=ch;               /* salva carattere nel buffer */
  buffRX2++;if (buffRX2>10000) buffRX2=0;
  lastCH=ch;                          /* salva il carattere il lastCH */
  return 1;
 } else return 0;
}

   #define PPPINITFCS      0xffff  /* Initial FCS value */


   /*
    * Calculate a new fcs given the current fcs and the new data.
    */
   u_short pppfcs(fcs, cp, len)	/* funzione per il calcolo dell'FCS da RFC */
       register unsigned short fcs;
       register unsigned char *cp;
       register int len;
   {
       while (len--) {
           fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
       }
       fcs=~fcs;
       return fcs;
   }

void logga(u_char *ppplog,char *iniz) { /* log su stdout */
int t;

void leggiOPZ() {
 int t1;
  for (t=0;t<(scbyte(ppplog_len)-4);) { 
   for (t1=0;t1<ppplog_dati[t+1];t1++)
     Log "%x.",ppplog_dati[t+t1]);
   t+=t1;Log " ");
  }   
}

void CoNf() {
         switch (ppplog_code) {
           case 1:  Log "ConfReq ");break;
           case 2:  Log "ConfAck ");break;
           case 3:  Log "ConfNack ");break;
           case 4:  Log "ConfRej ");break;
           case 5:  Log "TermReq ");break;
           case 6:  Log "TermAck ");break;
         }
}

  if (reteDEBUG&4) {
   printf("(lun=%u) ",scbyte(ppplog_len)+8);
   for (t=0;( t<(scbyte(ppplog_len)+8) )&&(t<60);t++) 
    printf("%x ",ppplog[t]);printf("\n");
  }  

  if ((reteDEBUG&8)==0) return;
  if ( ((stato&0x03ff)==0x03ff)&(!(reteDEBUG&1))) return;
  if ((stato&0x03ff)<0x03ff) printf("stato=%x:  ",stato);   
  if (scbyte(ppplog_proto)==0x80fd) return;
  Log "%s: ",iniz);
  switch (scbyte(ppplog_proto)) {
    case 0xc021: /*LCP*/
         Log "LCP  ");
         Log "0x%x ",ppplog_id);
         CoNf();
         leggiOPZ();
         break;
    case 0xc023:Log "PAP  "); /*PAP*/
                Log "0x%x ",ppplog_id);
                CoNf();
                for (t=0;t<(scbyte(ppplog_len)-4);t++)
                     if (ppplog_dati[t]==0x06) Log " Us:" ); else
                     if (ppplog_dati[t]==0x07) Log " Pass:" ); else
                     if (ppplog_dati[t]==0x08) Log " OK:" ); else
                     if (ppplog_dati[t]>31) Log "%c",ppplog_dati[t]);
                Log " "); 
                break;    
    case 0x8021:Log "IPCP "); /*IPCP*/ 
                Log "0x%x ",ppplog_id);
                CoNf();
                leggiOPZ();break;
    case 0x0021:Log "IP (%u) ",scbyte(*(u_short *)(ppplog+7))); /*IP*/
                Log "src=%u.%u.%u.%u ", ppplog_dati[8], ppplog_dati[9],ppplog_dati[10],ppplog_dati[11]);
                Log "dst=%u.%u.%u.%u ",ppplog_dati[12],ppplog_dati[13],ppplog_dati[14],ppplog_dati[15]);
                Log "proto=%u ",ppplog_dati[5]);
                if ((ppplog_dati[5]==6)||(ppplog_dati[5]==17)) {
                  Log "%u:%u ",scbyte(*(u_short *)(ppplog_dati+16)),scbyte(*(u_short *)(ppplog_dati+18)));
                  if (ppplog_dati[5]==6) {
                     Log "(%x:%x) ",scbyte4(*(u_int *)(ppplog_dati+20)),scbyte4(*(u_int *)(ppplog_dati+24)));
                     if (ppplog_dati[29]&0x02) Log "S"); else Log "_");
                     if (ppplog_dati[29]&0x10) Log "A"); else Log "_");
                     if (ppplog_dati[29]&0x01) Log "F"); else Log "_");
                     if (ppplog_dati[29]&0x04) Log "R"); else Log "_");
                     if (ppplog_dati[29]&0x08) Log "P"); else Log "_");
                     if (ppplog_dati[29]&0x20) Log "U"); else Log "_");
                  }
                }
                break;
    
      default :
         Log "Proto=%x ",scbyte(ppplog_proto));
         break;
  }
 if (ppplog_fcs==pppfcs(PPPINITFCS,ppplog+1,scbyte(ppplog_len)+4))
        Log " fcsPPP_OK");
        else Log " fcsPPP_BAD");
 if (scbyte(ppplog_proto)!=0x80fd) { Scrivilog;} else nlog=0;
} 

u_int indirizzoPPP() {	/* ritorna l'IP locale (da cancellare perche' inutile)*/
  return IPlocale;
}

#define PPPout(a,b,c,d,e) pppout_flag=0x73;pppout_addr=0xff;\
                          pppout_code=a;pppout_id=b;pppout_control=c;\
                          pppout_proto=scbyte(d);pppout_len=scbyte(e);


void mandaser() { /* funzione che pesca dal buffer in trasmissione e invia */
 int ta;
 for (ta=0;ta<3000;ta++) /* trasmette non piu' di 3000 caratteri presenti nel buffer */
  if (bufTX2!=bufTX1) {
    scrivibyteseriale(bufTX[bufTX2]);
    bufTX2=(bufTX2+1)%100000;
  }
}


void mandabyte(u_char Ch1) { /* scrive sul buffer di trasmissione */
  bufTX[bufTX1]=Ch1;
  bufTX1=(bufTX1+1)%100000;
}


void mandappp() { /* costruisce un frame in trasmissione */
  int t;
  u_short lungh;
  u_char chu;
  if (reteDEBUG&2) printf("mandaPPP\n");  
/*if (stato<7) delay(100); */
  if (pppout_proto==0x2100) lungh=IP_LUNGH; else lungh=scbyte(pppout_len);
  leds|=2; 
  pppout_fcs=0;
  pppout_fcs=pppfcs(PPPINITFCS,pppout+1,lungh+4);
  mandabyte('\x7e');
  for (t=1;t<(lungh+7);t++)
    if ((pppout[t]==0x7d)||(pppout[t]==0x7e)||
    ((pppout[t]<32)&&((1<<pppout[t])&asyncmap))  ) { 
     chu=pppout[t]^0x20; 
     mandabyte('\x7d');
     mandabyte(chu);
    } else mandabyte(pppout[t]);
  pppout[0]=pppout[t]='\x7e';
  logga(pppout,"Out");
  mandabyte('\x7e');
}


u_char mandaPPP(u_char *bufen,u_short lungh) {
/*  while (F_PPP) delay(50);
  F_PPP=1;*/
  if (reteDEBUG&2) printf("mandaPPP\n");
  copymem(bufen,buff+5,lungh);
/*  printf("%0x %02x %0x %02x\n",bufen,*bufen,pppout,pppout[9]); */
  if (lungh!=IP_LUNGH) 
    {printf("mandaPPP:lungh!=pppout_len\n"); /* F_PPP=0 */;return 0;}
  pppout_flag=0x7e;
  pppout_addr=0xff;
  pppout_control=0x03;
  pppout_proto=scbyte(0x0021);
/*  *(u_int *)(pppout+25)=IPlocale;    :))))) */
  mandappp();
/*  for (t=1;pppout[t]!=0x7e;t++) printf("%02x ",pppout[t]); puts(""); */
  
/*  F_PPP=0; */
  return 1;
}

/* stato:
1.TxLcpConfReq
2.RxLcpConfAck
*/


u_short riceviPPP() {
int tPPP,ppp_ack,ppp_ack2;
#define TIMEOUT_PPP (stato&0x0800)
void TxLCPConfReq() {
/*  while (F_PPP);
  F_PPP=1;*/
  if (reteDEBUG&2) printf("mandaPPP1\n");  
  PPPout(1,1,0x03,0xc021,4+6)
  copymem("\x02\x06\xff\xff\xff\xff",pppout_dati,6);
  asyncmap2=0xffffffff;
  mandappp();
/*  F_PPP=0; */

}


void TxLCPConfAckNack() {
      ppp_ack2=ppp_ack=2;
      for (tPPP=t=0;t<(scbyte(ppp_len)-4);) {
         if (ppp_dati[t]==2) { 
/*           asyncmap2=scbyte4(*(u_int *)(ppp_dati+2)); */
           if (reteDEBUG&4) printf("Async=%x\n",asyncmap2);
         }  
         if ((ppp_dati[t]==3)&&(ppp_dati[t+1]==5))
         { 
           ppp_ack2=3; 
           if (ppp_ack!=4) { 
            copymem("\x03\x04\xc0\x23",pppout_dati+tPPP,4);
            tPPP+=4;
           } 
         }
           else /* nack */
/*         if ((ppp_dati[t]==2)&&(ppp_dati[t+2]!=0xff))
           ppp_confnack++; else 
         if ((ppp_dati[t]==2)&&asyncmapno)
             ppp_ack2=3; else */
         if (ppp_dati[t]==2)
             ppp_ack2=2; else 
         if ((ppp_dati[t]==3)&&(ppp_dati[t+1]==4))
           ppp_ack2=2; 
         else
           ppp_ack2=4;
/*
  ppp_ack2 ppp_ack
  2       2        copia ->2
  2       3        copia ->3
  2       4        noncopia ->4
  3       2        copia ->3
  3       3        copia ->3
  3       4        noncopia ->4
  4       2        tPPP=0 copia ->4
  4       3        tPPP=0 copia ->4
  4       4        copia ->4 
  
  
   
*/

         if ((ppp_ack2==4)&&(ppp_ack<4)) tPPP=0;
         if (((ppp_ack!=4)||(ppp_ack2==4))&&(ppp_ack2!=3)) {
           copymem(ppp_dati+t,pppout_dati+tPPP,ppp_dati[t+1]);
           tPPP+=pppout_dati[tPPP+1];
         }
         if (ppp_ack<ppp_ack2) ppp_ack=ppp_ack2;  
         t+=ppp_dati[t+1];
      }/* loop opzioni */
      PPPout(ppp_ack,ppp_id,0x03,0xc021,tPPP+11-7);
      mandappp();
}

u_char RxLCPConfReq() {
  return (stato&0x1000)&&(ppp_code==1);
}
u_char RxLCPConfAck() {
  return (stato&0x1000)&&(ppp_code==2);
}
u_char RxLCPConfNackRej() {
  return (stato&0x1000)&&(ppp_code>2);
}
void TxPAPConfReq() {
         pppout_dati[0]=strlen(UserID); /*userid */
         if (reteDEBUG&4) printf("sizeuserid=%u\n",strlen(UserID));
         copymem(UserID,pppout_dati+1,tPPP=strlen(UserID));
         pppout_dati[tPPP+1]=strlen(PassWD); /*password */
         if (reteDEBUG&4) printf("sizepasswd=%u\n",strlen(PassWD));
         copymem(PassWD,pppout_dati+tPPP+2,t=strlen(PassWD));
         PPPout(1,ppp_id+1,0x03,0xc023,tPPP+t+4+2) /*ConfReq userid password*/ 
         mandappp();
}
u_char RxPAPConfAck() {
  return (stato&0x2000)&&(ppp_code==2);
}
u_char RxPAPConfNackRej() {
  return (stato&0x2000)&&(ppp_code>2);
}
u_char RxIPCPConfReq() {
  return (stato&0x4000)&&(ppp_code==1);
}
void TxIPCPConfAckNack() {
      ppp_ack2=ppp_ack=2;
      for (tPPP=t=0;t<(scbyte(ppp_len)-4);) {
         if ((ppp_dati[t]==3)&&(ppp_dati[t+1]==6)) {
           ppp_ack2=2; /* ack */
           IPremoto=*(u_int *)(ppp_dati+t+2);
         } else {
           ppp_ack2=4;
         }
         if ((ppp_ack2==4)&&(ppp_ack<4)) tPPP=0;
         if ((ppp_ack!=4)||(ppp_ack2==4)) {
           copymem(ppp_dati+t,pppout_dati+tPPP,ppp_dati[t+1]);
           tPPP+=pppout_dati[tPPP+1];
         }
         if (ppp_ack<ppp_ack2) ppp_ack=ppp_ack2;  
         t+=ppp_dati[t+1];
      }/* loop opzioni */
      PPPout(ppp_ack,ppp_id,0x03,0x8021,tPPP+11-7);
      mandappp();
}

void TxIPCPConfReq() {
     PPPout(1,ppp_id+1,0x03,0x8021,4+18) /*ConfReq ip*/
     copymem("\x03\x06",pppout_dati,2);
     copymem((u_char *)(&IPlocale),pppout_dati+2,4);
     copymem("\x81\x06",pppout_dati+6,2);
     copymem((u_char *)(&IPDNS1),pppout_dati+8,4);
     copymem("\x83\x06",pppout_dati+12,2);
     copymem((u_char *)(&IPDNS2),pppout_dati+14,4);
     mandappp();
}

u_char RxIPCPConfNack() {
  if ((stato&0x4000)&&(ppp_code==3)) {
     for (tPPP=t=0;t<(scbyte(ppp_len)-4);) {
        if ((ppp_dati[t]==3)&&(ppp_dati[t+1]==6)) IPlocale=*(u_int *)(ppp_dati+t+2);
        if ((ppp_dati[t]==0x81)&&(ppp_dati[t+1]==6)) IPDNS1=*(u_int *)(ppp_dati+t+2);
        if ((ppp_dati[t]==0x83)&&(ppp_dati[t+1]==6)) IPDNS2=*(u_int *)(ppp_dati+t+2);
        t+=ppp_dati[t+1];
     }
     return 1;
  }
  return 0;
}
u_char RxIPCPConfRej() {
  return (stato&0x4000)&&(ppp_code==4);
}


/* da implementare */
u_char RxLCPTermReq() {
  return (stato&0x100)&&(ppp_code==5);
}
void   TxLCPTermAck() {
/*  while (F_PPP) ;
  F_PPP=1;*/
  PPPout(6,ppp_id,0x03,ppp_proto,scbyte(ppp_len));
  copymem(ppp_dati,pppout_dati,scbyte(ppp_len)-4);
  mandappp();
  stato=0;
/*  F_PPP=0; */
}
/* -- */


u_char RxIPCPConfAck() {
  if ((stato&0x4000)&&(ppp_code==2)) {
     for (tPPP=t=0;t<(scbyte(ppp_len)-4);) {
        if ((ppp_dati[t]==3)&&(ppp_dati[t+1]==6)) IPlocale=*(u_int *)(ppp_dati+t+2);
        if ((ppp_dati[t]==0x81)&&(ppp_dati[t+1]==6)) IPDNS1=*(u_int *)(ppp_dati+t+2);
        if ((ppp_dati[t]==0x83)&&(ppp_dati[t+1]==6)) IPDNS2=*(u_int *)(ppp_dati+t+2);
        t+=ppp_dati[t+1];
     }
     return 1;
  } 
  return 0;
}

  if (stato&0x0400) { TxLCPConfReq();stato=1;}
  leggiPPP();
  stato&=0x03ff;
  if (npunta!=npunta2) {
    if (reteDEBUG&2) printf("riceviPPP npunta!=npunta2\n");  
    ppp=(u_char *)(buffRX+punta[npunta2]);
    logga(ppp,"Inp");
    if ((stato&0x3ff)<=0x3f) delay(200);
    npunta2=(npunta2+1)%10;
    if (ppp_fcs==pppfcs(PPPINITFCS,ppp+1,scbyte(ppp_len)+4)) {/* fcs OK */
       if (ppp_proto==0x21c0) stato|=0x1000; /* lcp */ else
       if (ppp_proto==0x23c0) stato|=0x2000; /* pap */ else
       if (ppp_proto==0x2180) stato|=0x4000; /* ipcp */ else
       if (ppp_proto==0x2100) stato|=0x8000; /* ip */ else
             stato|=0x8000; /* gli altri */
       PPPtimeout=adesso;
    }
  } else 
  if ((stato&0x3ff)==0x3ff) goto fine; /* non ci sono dati e il link e' attivo */
    else 
  if ( (PPPtimeout+2)< adesso )
        { PPPtimeout=2+adesso;stato|=0x0800;} /* timeout */
  if (RxLCPTermReq()) { stato=0; TxLCPTermAck(); }
  if ((stato&0x7ff)<15) {
    if (TIMEOUT_PPP) { TxLCPConfReq(); stato=(stato&0xffc)|1; }
    if (RxLCPConfReq()) {
      if ((stato&0xc) == 0xc) { TxLCPConfReq(); stato = 1; } else
      {
         stato=(stato&0xfff3)|0x0004;
         ppp_ack=2;
         TxLCPConfAckNack();
         if (ppp_ack==2) stato|=8;
      }   
    } /* if RxLCPConfReq */ 
    if (RxLCPConfNackRej()) { stato=0; printf("opzioni invalide\n"); }
    if (RxLCPConfAck()) stato|=2;
    
  } else /* if stato<15 */
  if ((stato&0x7ff)==15) { /* LCP -> PAP */
    asyncmap=asyncmap2;
    TxPAPConfReq();
    stato|=16; 
  } else
  if ((stato&0x7ff)<63)  { /* PAP */
    if (TIMEOUT_PPP) TxPAPConfReq();
    if (RxPAPConfAck()) stato|=32;
    if (RxPAPConfNackRej()) { stato=0;printf("Autenticazione fallita\n");}
  } else /* se stato < 63 */
/*  if ((stato&0x7ff)==63) { PAP -> IPCP 
    TxIPCPConfReq(); stato|=64; 
  } else */
  if ((stato&0x7ff)<1023) { /* IPCP */
    if ((TIMEOUT_PPP)||((stato&0x03c0)==0x0300)) { TxIPCPConfReq(); stato=(stato&0xfbf)|64; }
    if (RxIPCPConfReq()) { stato=(stato&0xdff)|256; 
      TxIPCPConfAckNack();
      if (ppp_ack==2) stato|=512;
    }
    if (RxIPCPConfAck()) stato|=128;
    if (RxIPCPConfNack()) TxIPCPConfReq();
    if (RxIPCPConfRej()) { stato=0; printf("opzioni IP rigettate dal remoto\n"); }
  }
  if ((stato&0x03ff)==0x03ff) {
    if (stato&0x8000) {
    if (ppp_proto==scbyte(0x0021))
     if (scbyte(ppp_len)<=9900) {
      copymem(ppp+5,pppout,scbyte(ppp_len));
      pppout[scbyte(ppp_len)]=0;
      pppout[scbyte(ppp_len)+1]=0;
      riceviIP(pppout,scbyte(ppp_len));
     }
    if (ppp_proto==scbyte(0x80fd))
     if (ppp_code==1) {
/*      while (F_PPP);
      F_PPP=1;*/
      PPPout(2,ppp_id,0x03,0x80fd,scbyte(ppp_len))
      copymem(ppp_dati,pppout_dati,scbyte(ppp_len)-4);
      mandappp();
/*      F_PPP=0; */
     } 
    }
  } 
  if (stato&0xf000) leds|=1;
fine:  
  mandaser();
  return 0;
}


u_char inizializzaPPP() {
  asyncmap=0xffffffff;
  if (reteDEBUG&16) asyncmapno=1; else asyncmapno=0;
  if (reteDEBUG&2) printf("inizializzaPPP\n");  
   if (buff==0) buff=malloc(10000);
   if (buffRX==0) buff=malloc(10000);
   if (bufTX==0) bufTX=malloc(100000);
   pppout=(u_char *)buff;
   ppp=(u_char *)buffRX;
   setmem(buff,0,10000);
   setmem(buffRX,0,10000);
   punta[npunta2=0]=-1;npunta=0;
   stato=0;
  return 1;
}

u_char chiudiPPP() {
  RETE_=0;
  return 1;
}

