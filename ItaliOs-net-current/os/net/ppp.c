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

  u_char F_PPP=0;
  u_char lastCH=0;
  u_char *ppp,*pppout,*ppplog;
  u_char *buff=0;
  u_char *buffRX=0;
  u_short buffRX1,buffRX2=0;
  int punta[]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},npunta=0,npunta2=0,puntaP=-1;
  char log[200],nlog=0,CLK=0;
  u_char stato=0;
  u_int IPlocale,IPremoto;
  u_int IPDNS1,IPDNS2;

  u_char *bufTX=0;
  u_short bufTX1,bufTX2=0;
  u_int ritorno;  
  u_char ppp_confack,ppp_confnack,ppp_confrej;

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

u_char leggiPPP() {
  u_char ch;
  ch=((ritorno=leggibyteseriale())&0xff);
  if (ritorno!=NO_DATA) {
  if (ch==0x7e) {  /* se flag */
   if (lastCH!=0x7e)                   /* se frame non nullo */
    if (puntaP>=0) {                        /* allora trasmetti */
       punta[npunta]=puntaP;
       npunta=(npunta+1)%10;
    }
/*    printf("\nMarcaFrame lastCH=%02x buffRX2=%u puntaP=%u\n",lastCH,buffRX2,puntaP); */
    if (buffRX2>(10000-3000)) buffRX2=0;
    puntaP=buffRX2;                    /* marca frame successivo */
  }
  if (ch==0x7d) {
    while ((ritorno=leggibyteseriale())==NO_DATA);
      ch=(ritorno&0xff)^0x20;
  }
/*  if (!ch) return 0; */
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
   u_short pppfcs(fcs, cp, len)
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

void logga(u_char *ppplog,char *iniz) {
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

/*  for (t=0;t<(scbyte(ppplog_len)+8);t++) 
    for (t=0;t<50;t++) 

  for (t=0;t<(scbyte(ppplog_len)+8);t++) 
   printf("%x ",ppplog[t]);printf("\n");
*/
  if ((stato>7)&&(!(reteDEBUG&1))) return;
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

u_int indirizzoPPP() {
  return IPlocale;
}

#define PPPout(a,b,c,d,e) pppout_flag=0x73;pppout_addr=0xff;\
                          pppout_code=a;pppout_id=b;pppout_control=c;\
                          pppout_proto=scbyte(d);pppout_len=scbyte(e);


void mandaser() {
 int ta;
 for (ta=0;ta<3000;ta++) 
  if (bufTX2!=bufTX1) {
    scrivibyteseriale(bufTX[bufTX2]);
    bufTX2=(bufTX2+1)%100000;
  }
}


void mandabyte(u_char Ch1) {
  bufTX[bufTX1]=Ch1;
  bufTX1=(bufTX1+1)%100000;
}


void mandappp() {
  int t;
  u_short lungh;
  u_char chu;
  if (reteDEBUG&2) printf("mandaPPP\n");  
  if (stato<7) delay(300);
  if (pppout_proto==0x2100) lungh=IP_LUNGH; else lungh=scbyte(pppout_len);
  pppout_fcs=0;
  pppout_fcs=pppfcs(PPPINITFCS,pppout+1,lungh+4);
  mandabyte('\x7e');
/*  for (t=0;t<(lungh+7);t++) printf("%02x ",pppout[t]); */
  for (t=1;t<(lungh+7);t++)
    if ((pppout[t]<32)||(pppout[t]==0x7d)||(pppout[t]==0x7e)) {
     chu=pppout[t]^0x20; 
     mandabyte('\x7d');
     mandabyte(chu);
    } else mandabyte(pppout[t]);
  pppout[0]=pppout[t]='\x7e';
  logga(pppout,"Out");
  mandabyte('\x7e');
/*  printf(" Lungh=%u\n",lungh); */
}

u_char traduciPPP() {
  u_char oldstato;
  int t,t1;
/* costruisce stringa di log */
/* controllo di quello che serve */ 
  if (reteDEBUG&2) printf("traduciPPP\n");  
 oldstato=stato;
 switch (stato) {
  case 82:/* accetta solo autenticazione */
   if (ppp_proto==scbyte(0xc021)) {
       stato=1;
/*     leggiopz(); */
/*     pppout_id=1; */
     for (t1=t=0;t<(scbyte(ppp_len)-4);) {
       if ((ppp_dati[t]!=2)&&(ppp_dati[t]!=3)) {
         copymem(ppp_dati+t,pppout_dati+t1,ppp_dati[t+1]);
         t1+=pppout_dati[t1+1];
       }
       t+=ppp_dati[t+1];
     }
     /* header trama ppp + LCP */ 
     PPPout(4,ppp_id,0x03,0xc021,t1+11-7) /*ConfRej tutto tranne auth e async */
     pppout_dati[t1+0]=0;
     pppout_dati[t1+1]=0;
     pppout_dati[t1+2]=0x7e;
     mandappp();
   }
   break;
 case 0: /* se chap rifiuta se pap accetta e manda character map*/
   if (ppp_proto==scbyte(0xc023)) {
       if (ppp_code==2) /* accetta */
         stato=4 /* PAP ok */;
   } else    
   if (ppp_proto==scbyte(0xc021)) {
       ppp_confnack=ppp_confrej=ppp_confack=0; 
       for (t=0;t<(scbyte(ppp_len)-4);) {
         if ((ppp_dati[t]==3)&&(ppp_dati[t+1]==5))
           ppp_confnack++; else
         if ((ppp_dati[t]==2)&&(ppp_dati[t+2]!=0xff))
           ppp_confnack++; else
         if ((ppp_dati[t]==3)&&(ppp_dati[t+1]==4))
           ppp_confack++; else ppp_confrej++;
         t+=ppp_dati[t+1];
       }
       if (reteDEBUG&2) printf("ACK=%u NACK=%u REJ=%U\n",ppp_confack,ppp_confnack,ppp_confrej);
       if (ppp_confrej) {
        for (t1=t=0;t<(scbyte(ppp_len)-4);) {
         if ((ppp_dati[t]!=2)&&(ppp_dati[t]!=3)) {
           copymem(ppp_dati+t,pppout_dati+t1,ppp_dati[t+1]);
           t1+=pppout_dati[t1+1];
         }
         t+=ppp_dati[t+1];
        }
        PPPout(4,ppp_id,0x03,0xc021,t1+11-7) /*ConfRej tutto tranne auth e async */
       } else
       if (ppp_confnack) {
        for (t1=t=0;t<(scbyte(ppp_len)-4);) {
         if (ppp_dati[t]==2) {
           copymem("\x02\x06\xff\xff\xff\xff",pppout_dati+t1,6);
           t1+=6;
         } else             
         if (ppp_dati[t]==3) {
           copymem("\x03\x04\xc0\x23",pppout_dati+t1,4);
           t1+=4;
         } else {
           copymem(ppp_dati+t,pppout_dati+t1,ppp_dati[t+1]);
           t1+=pppout_dati[t1+1];
         }
         t+=ppp_dati[t+1];
        }
        PPPout(3,ppp_id,0x03,0xc021,t1+11-7) /*ConfNack auth chap*//*prova confrej */
       } else { /* confack */
        PPPout(2,ppp_id,0x03,0xc021,scbyte(ppp_len))  /*ConfReq IPrem*/
        copymem(ppp_dati,pppout_dati,scbyte(ppp_len)-4);
        mandappp();
        pppout_dati[0]=sizeof(UserID)-1; /*userid */
        copymem(UserID,pppout_dati+1,t1=sizeof(UserID)-1);
        pppout_dati[t1+1]=sizeof(PassWD)-1; /*password */
        copymem(PassWD,pppout_dati+t1+2,t=sizeof(PassWD)-1);
        PPPout(1,1,0x03,0xc023,t+t1+4+2) /*ConfReq userid password*/
       }
       mandappp();
   } 
   break;
 case 3: /* controlla se l'isp Ack la PAP user e pass  */
   if (ppp_proto==scbyte(0xc021)) {
       if ((ppp_dati[0]==3)&&(ppp_dati[1]==5)) {
         stato=1;
         PPPout(4,ppp_id,0x03,0xc021,4+5) /*ConfNack auth chap*//*prova confrej */
         copymem(ppp_dati,pppout_dati,5);
         mandappp();
         
       }
   } 
   if (ppp_proto==scbyte(0xc023)) {
       if (ppp_code==2) /* accetta */
         stato=4 /* PAP ok */;
   }    
   break;
 case 4: /* se e' IPCP nak tutte tranne la 3 */
   if (ppp_proto==scbyte(0x8021))
     if (ppp_code==1) {/* ConfReq IPCP */
         stato=5;
         for (t1=t=0;t<(scbyte(ppp_len)-4);) {
          if (ppp_dati[t]!=3) {
            copymem(ppp_dati+t,pppout_dati+t1,ppp_dati[t+1]);
            t1+=pppout_dati[t1+1];
          }
          t+=ppp_dati[t+1];
         }
     /* header trama ppp + IPCP */ 
     PPPout(4,ppp_id,0x03,0x8021,t1+11-7) /*ConfRej tutto tranne ip*/
     pppout_dati[t1+0]=0;
     pppout_dati[t1+1]=0;
     pppout_dati[t1+2]=0x7e;
     mandappp();
     }
   break;
 case 5: /* conferma IP */
   if (ppp_proto==scbyte(0x8021))
     if (ppp_code==1) {/* ConfReq IPCP */
         stato=6;
     PPPout(2,ppp_id,0x03,0x8021,4+6) /*ConfAck ip*/
     copymem(ppp_dati,pppout_dati,6);
     mandappp();IPremoto=*(u_int *)(ppp_dati+2);  
     Log "->IPremoto=%u.%u.%u.%u",ppp_dati[2],ppp_dati[3],ppp_dati[4],ppp_dati[5]);Scrivilog;
     printf("IPremoto=%u.%u.%u.%u\n",ppp_dati[2],ppp_dati[3],ppp_dati[4],ppp_dati[5]);
     PPPout(1,ppp_id+1,0x03,0x8021,4+6) /*ConfReq un nuovo ip per completare il three way*/
     copymem("\x03\x06\x00\x00\x00\x00",pppout_dati,6);
     mandappp();
         
     }    break;

 case 6: /* conferma IP */
   if (ppp_proto==scbyte(0x8021))
     if (ppp_code==3) {/* ConNack IPCP */
         stato=7;IPlocale=*(u_int *)(ppp_dati+2);  
         Log "->IPlocale=%u.%u.%u.%u",ppp_dati[2],ppp_dati[3],ppp_dati[4],ppp_dati[5]);Scrivilog;
         printf("IPlocale=%u.%u.%u.%u\n",ppp_dati[2],ppp_dati[3],ppp_dati[4],ppp_dati[5]);

     PPPout(2,ppp_id,0x03,0x8021,4+6)  /*ConfAck ip*/
     copymem(ppp_dati,pppout_dati,6);
     mandappp();
     PPPout(1,ppp_id+1,0x03,0x8021,4+18) /*ConfReq ip*/
     copymem(ppp_dati,pppout_dati,6);
     copymem("\x81\x06\x00\x00\x00\x00\x83\x06\x00\x00\x00\x00",pppout_dati+6,12);
     mandappp();
   } break;
 case 7: /* conferma IPremoto e richiesta DNS */
   if (ppp_proto==scbyte(0x8021))
     if (ppp_code==3) {/* ConfNAck IPCP */
       stato=8;
       PPPout(1,ppp_id,0x03,0x8021,4+18)  /*ConfReq DNS*/
       *(u_int *)(pppout_dati+2)=IPlocale;
       pppout_dati[0]=0x03;pppout_dati[1]=0x06;
       copymem(ppp_dati,pppout_dati+6,12);
       mandappp();
     }     

     break;
 case 8: /* conferma DNS */

   if (ppp_proto==scbyte(0x8021))
     if (ppp_code==2) {/* ConfAck IPCP */
       stato=9;
       Log "->DNS1=%u.%u.%u.%u",ppp_dati[8],ppp_dati[9],ppp_dati[10],ppp_dati[11]);Scrivilog;
       Log "->DNS2=%u.%u.%u.%u",ppp_dati[14],ppp_dati[15],ppp_dati[16],ppp_dati[17]);Scrivilog;
       printf("DNS1=%u.%u.%u.%u\n",ppp_dati[8],ppp_dati[9],ppp_dati[10],ppp_dati[11]);
       printf("DNS2=%u.%u.%u.%u\n",ppp_dati[14],ppp_dati[15],ppp_dati[16],ppp_dati[17]);
       IPDNS1=*(u_int *)(ppp_dati+8);
       IPDNS2=*(u_int *)(ppp_dati+14);
     }     

     break;
 default :
   if (ppp_proto==scbyte(0x8021))
     if (ppp_code==1) {/* ConfReq IPCP */
       PPPout(2,ppp_id,0x03,0x8021,scbyte(ppp_len))  /*ConfReq IPrem*/
       copymem(ppp_dati,pppout_dati,scbyte(ppp_len)-4);
       mandappp();
     }     
   if (ppp_proto==scbyte(0x80fd))
     if (ppp_code==1) {
     PPPout(2,ppp_id,0x03,0x80fd,scbyte(ppp_len))
     copymem(ppp_dati,pppout_dati,scbyte(ppp_len)-4);
     mandappp();
   } 
   
   if (ppp_proto==scbyte(0x0021))
    if (scbyte(ppp_len)<=9900) {
      copymem(ppp+5,pppout,scbyte(ppp_len));
      pppout[scbyte(ppp_len)]=0;
      pppout[scbyte(ppp_len)+1]=0;
      riceviIP(pppout,scbyte(ppp_len));
    }
   break;
 
 }
/* if (oldstato!=stato) logga(ppp,"Inp"); */

 return 0;
}


u_char mandaPPP(u_char *bufen,u_short lungh) {
  while (F_PPP);
  F_PPP=1;
  if (reteDEBUG&2) printf("mandaPPP\n");
  copymem(bufen,buff+5,lungh);
/*  printf("%0x %02x %0x %02x\n",bufen,*bufen,pppout,pppout[9]); */
  if (lungh!=IP_LUNGH) 
    {printf("mandaPPP:lungh!=pppout_len\n");  F_PPP=0;return 0;}
  pppout_flag=0x7e;
  pppout_addr=0xff;
  pppout_control=0x03;
  pppout_proto=scbyte(0x0021);
/*  *(u_int *)(pppout+25)=IPlocale;    :))))) */
  mandappp();
/*  for (t=1;pppout[t]!=0x7e;t++) printf("%02x ",pppout[t]); puts(""); */
  
  F_PPP=0;
  return 1;
}

u_short riceviPPP() {
  leggiPPP();
/*  if (ritorno==NO_DATA) */
  if (npunta!=npunta2) {
    if (reteDEBUG&2) printf("riceviPPP npunta!=npunta2\n");  
    ppp=(u_char *)(buffRX+punta[npunta2]);
    npunta2=(npunta2+1)%10;
    logga(ppp,"Inp");
    traduciPPP();
    mandaser();
    return 1; 
  }
  mandaser();
  return 0;
}


u_char inizializzaPPP() {
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

void mandaPPP1() {
  while (F_PPP);
  F_PPP=1;
  if (reteDEBUG&2) printf("mandaPPP1\n");  
  PPPout(1,1,0x03,0xc021,4+6) /*ConfAck auth pap*/
  copymem("\x02\x06\xff\xff\xff\xff",pppout_dati,6);
  mandappp();
  F_PPP=0;

}
