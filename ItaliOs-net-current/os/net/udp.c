#define r_UDP
#include "rete.h"

#define SKK Sok[sk]
#define Udp(p1,p2,l1) { lUDP[0]=scbyte(p1); lUDP[1]=scbyte(p2); lUDP[2]=scbyte(l1); }

#define Errore(a) { printf("%s\n",a); goto errore;}

u_char *bufUDPtx;
u_char *bufUDPrx;
u_short proxPrt3=1024;

u_char mandaUDP(u_char sk,u_int Ip,u_short Porta,u_char *bufUDP,u_short lungh) {
  u_short *lUDP;
  bufUDPtx=malloc(lungh+8);  /* creazione pacchetto uscita */
  lUDP=(u_short *)bufUDPtx;
  copymem(bufUDP,bufUDPtx+8,lungh);
  if (sk==255) { 
        Udp(proxPrt3++,Porta,lungh+8);
        if (!mandaIP(Ip,17,bufUDPtx,8+lungh)) Errore("mandaUDP: erroreMandaIP");
      } else {
        Udp(SKK.PRTdst,SKK.PRTsrc,lungh+8);
        if (!mandaIP(SKK.IPsrc,17,bufUDPtx,8+lungh)) Errore("mandaUDP: erroreMandaIP");
      }  
  
  free(bufUDPtx);
  return 1;
errore: 
  free(bufUDPtx);
  return 0;  
  
}

#define SKK Sok[sk]
u_short RxUDP(u_char sk,u_char *buffUDP,u_short lungh) {
  while ((!SKK.buflenRX)&&(RETE_)&&((SKK.stato==5)||(SKK.stato==6))) ;
  if (reteDEBUG&2) printf("RxUDP buflenRX=%u\n",SKK.buflenRX);
  if (SKK.buflenRX) {
     if (lungh>=SKK.buflenRX) {
        copymem(SKK.bufRX,buffUDP,SKK.buflenRX);
        lungh=SKK.buflenRX;SKK.buflenRX=0;
     } else {
        copymem(SKK.bufRX,buffUDP,lungh);
        copymem(SKK.bufRX+lungh,SKK.bufRX,SKK.buflenRX-lungh);
        lungh=SKK.buflenRX;SKK.buflenRX-=lungh;
     }
     return lungh;
     
  } else return 0;
}

u_char riceviUDP(u_char sk,u_char *bufUDP,u_short lungh) {
  u_short *lUDP;
  if (reteDEBUG&2) printf("riceviUDP lungh=%u\n",lungh);
  bufUDPrx=malloc(lungh+8);  /* creazione pacchetto ingresso */
  lUDP=(u_short *)bufUDPrx;
  copymem(bufUDP,bufUDPrx,lungh);
/*  SKK.ricez(sk,bufUDPrx+8,lungh-8); */
  if ((SKK.buflenRX+lungh-8)<3000) {
       copymem(bufUDPrx+8,SKK.bufRX+SKK.buflenRX,lungh-8);
       SKK.buflenRX+=lungh-8;
  }  
  free(bufUDPrx);
  return 1;
}

u_char ascoltaUDP(u_char *sk,u_short Porta,u_int Ip) {
  if ((*sk=registraIP(0,Porta,Ip,0,17))!=255) {
    Sok[*sk].stato=6;
    return 1;
  }
  return 0;
}
u_char registraUDP(u_char *sk,u_short Porta,u_int Ip) {
  if ((*sk=registraIP(Porta,0,Ip,0,17))!=255) {
    Sok[*sk].stato=5;
    return 1;
  }
  return 0;
}

u_char deregistraUDP(u_char sk) {
 Sok[sk].stato=0;
 deregistraIP(sk);
 return 1;
}
