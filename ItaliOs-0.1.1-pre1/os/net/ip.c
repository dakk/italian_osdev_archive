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

#define r_IP
#include "rete.h"

/*
   mandaIP
   riceviIP
*/

  u_int ll;
  u_char *buf1;
  u_char *bufIPrx;


  struct TSok *Sok[10]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  struct TSok1 {
    u_char bufTX[3000];
    u_short buflenTX;
    u_char flag;
  } *SokIP=NULL; 
  
  unsigned short *l1IP;
  unsigned int *indiIP;
  u_char LSTN=0;
  extern u_short Errori[10];
  u_char SockTrovato=0;
  u_int z1;
  u_char F_IP=0; 

u_short Chk_ip() {
/* calcolo cksum IP */
    int t;
    ll=0;
    for (t=0;t<10;t++) ll+=(unsigned int)(l1IP[t]);
    for (t=0;t<2;t++) ll=((ll>>16)&0xffff)+(ll&0xffff);
    return ((unsigned short)(ll&0xffff))^0xffff;
};

u_short Chk_tcp(u_char *bufc) {
/* calcolo cksum TCP */
    int t;
    ll=(u_int)scbyte(scbyte(l1IP[1])-20)+((u_int)bufc[9]<<8); 
    for (t=6;t<(scbyte(l1IP[1])/2);t++) ll+=(u_int)(l1IP[t]);
    if (scbyte(l1IP[1])%2) ll+=(u_int)(l1IP[t]&0xff);
    for (t=0;t<3;t++) ll=((ll>>16)&0xffff)+(ll&0xffff);
    return ((u_short)(ll&0xffff))^0xffff;
};
u_short Chk_udp(u_char *bufc) {
/* calcolo cksum UDP */
    int t;
    ll=(u_int)scbyte(scbyte(l1IP[1])-20)+((u_int)bufc[9]<<8); 
/*    ll=scbyte(((u_int)(bufc[32]>>2)&0x3c))+((u_int)bufc[9]<<8)+l1IP[1]; */
    for (t=6;t<(scbyte(l1IP[1])/2);t++) ll+=(u_int)(l1IP[t]);
    if (scbyte(l1IP[1])%2) ll+=(u_int)(l1IP[t]&0xff);
    for (t=0;t<3;t++) ll=((ll>>16)&0xffff)+(ll&0xffff);
    return ((unsigned short)(ll&0xffff))^0xffff;
};

u_short Chk_icmp() {
/* calcolo cksum ICMP */
    int t;
    ll=0;  for (t=10;t<(scbyte(l1IP[1])/2);t++) ll+=(unsigned int)(l1IP[t]);
    if (scbyte(l1IP[1])%2) ll+=(u_int)(l1IP[t]&0xff);
    return ((unsigned short)(ll&0xffff)+(unsigned short)((ll&0xffff0000)>>16))^0xffff;
};




u_char mandaIP(u_int Ip,u_char proto,u_char *buffIP,u_short lungh) {
/*  while (F_IP) delay(50);
  F_IP=1;  */
  leds|=16;
  Debug("mandaIP\n");
  if (lungh>10000) return 0;
  buf1=malloc(lungh+30);
  l1IP=(unsigned short *)buf1;
  indiIP=(unsigned int *)buf1;
  setmem(buf1,0,lungh+30);
  copymem(buffIP,buf1+20,lungh);
  l1IP[0]=0x0045;l1IP[1]=scbyte(lungh+20);
  l1IP[2]=0xfe54;l1IP[3]=0x40;
  buf1[8]=64;buf1[9]=proto;
  indiIP[3]=IPlocale;
  indiIP[4]=Ip;
  l1IP[5]=Chk_ip(buf1);
  if (proto==1) { l1IP[11]=0; l1IP[11]=Chk_icmp();}
  if (proto==6) { l1IP[18]=0; l1IP[18]=Chk_tcp(buf1);}
  if (proto==17) { l1IP[13]=0; l1IP[13]=Chk_udp(buf1);}
  if (Ip==IPlocale) mandaLOCALE(buf1,lungh+20); else
  if (mandaPPP(buf1,lungh+20)==0) { Errore(0,"mandaIP: errore mandaPPP");}
  free(buf1);  
/*  F_IP=0;*/
  return 1;
errore:
  free(buf1);  
/*  F_IP=0; */
  return 0;
}


void InizSocket() {
  int t;
  
  for (t=0;t<10;t++) {
   if (Sok[t]==NULL) Sok[t]=malloc(sizeof(struct TSok));
   setmem((u_char *)Sok[t],0,sizeof(struct TSok));
  } 
}

u_short riceviIP(u_char *buf2,u_short lungh) {
  int t=0;
  bufIPrx=malloc(lungh);
  copymem(buf2,bufIPrx,lungh);
  l1IP=(unsigned short *)bufIPrx;
  indiIP=(unsigned int *)bufIPrx;
  leds|=4;
  if (reteDEBUG&2) printf("riceviIP lungh=%u\n",lungh);
  if (Chk_ip()) Errore(1,"riceviIP: errore CheckIP");
  if (bufIPrx[9]==1) if (Chk_icmp()) Errore(1,"riceviIP: errore CheckICMP");
  if (bufIPrx[9]==6) if (Chk_tcp(bufIPrx)) Errore(1,"riceviIP: errore CheckTCP");
  if (bufIPrx[9]==17) if (Chk_udp(bufIPrx)) Errore(1,"riceviIP: errore checkUDP");
  SockTrovato=0;
  if (reteDEBUG&2) printf("riceviIP %u %s:%s %u:%u\n",bufIPrx[9]
          ,NUMinIP(indiIP[3]),NUMinIP(indiIP[4]),scbyte(l1IP[10]),scbyte(l1IP[11]));
  if ((bufIPrx[9]==1)&&(l1IP[10]==0x0008)) 
      {
        SockTrovato=1;
        ServerEcho(indiIP[3],bufIPrx+20,lungh-20); 
      } else
  for (t=0;t<10;t++) {
   if (Sok[t]!=NULL) {
    if (Sok[t]->proto==1) { /* ICMP */
     if ((Sok[t]->IPdst==0)||(Sok[t]->IPdst==indiIP[3]))
      if ((Sok[t]->IPsrc==0)||(Sok[t]->IPsrc==indiIP[4]))
       if (Sok[t]->PRTsrc==scbyte(l1IP[10])) {
/*        if (Sok[t]->PRTdst==scbyte(l1IP[11])) */
           if ((Sok[t]->buflenRX+lungh-20)<3000) {
                         SockTrovato=1;
                         copymem(bufIPrx+20,Sok[t]->bufRX+Sok[t]->buflenRX,lungh-20);
                         Sok[t]->buflenRX+=lungh-20;
           }
       } 
    } else
    if (Sok[t]->proto!=0) { /* gli altri */
      if ((Sok[t]->proto==0)||(Sok[t]->proto==bufIPrx[9]))
       if ((Sok[t]->IPdst==0)||(Sok[t]->IPdst==indiIP[3]))
        if ((Sok[t]->IPsrc==0)||(Sok[t]->IPsrc==indiIP[4]))
         if ((Sok[t]->PRTsrc==0)||(Sok[t]->PRTsrc==scbyte(l1IP[10])))
          if ((Sok[t]->PRTdst==0)||(Sok[t]->PRTdst==scbyte(l1IP[11]))) {
             SockTrovato=1;
             switch (Sok[t]->proto) {
              case 6: riceviTCP((u_char)t,indiIP[3],bufIPrx+20,lungh-20);
                      break;
              case 17: 
                if (!Sok[t]->IPdst) {
                  LSTN=t+1;Sok[t]->IPdst=indiIP[3];Sok[t]->PRTsrc=scbyte(l1IP[10]);
                }
                riceviUDP((u_char)t,bufIPrx+20,lungh-20);
                t=LSTN-1;
                if (LSTN) {
                  LSTN=0;Sok[t]->IPdst=0;Sok[t]->PRTsrc=0;
                }
                break;
              default : 
/* da implementare insieme a RxIP e TxIP */

                 if ((Sok[t]->buflenRX+lungh-20)<3000)
                  copymem(bufIPrx+20,Sok[t]->bufRX+Sok[t]->buflenRX,lungh-20);
                  Sok[t]->buflenRX+=lungh+20;
/*              default : Sok[t]->ricez(t,bufIPrx+20,lungh-20); break; */
                 break;
             }

            } 
               
    }
   }
  }    
  if (!SockTrovato) {
    Errori[1]++;
    switch (bufIPrx[9]) {
      case 6: nonsoTCP(indiIP[3],bufIPrx+20,lungh-20);break; /* TCP */
      case 17: break;
  }}
  free(bufIPrx);
  return lungh-20;
errore:
  leds|=8;
  free(bufIPrx);
  return 0;
}

u_char registraIP(u_short porta1,u_short porta2,u_int ip1,u_int ip2,u_char proto) {
  int t;
  while (F_IP) delay(50);
  F_IP=1;
  Debug("registraIP\n");
  for (t=0;((t<10)&&(Sok[t]!=NULL));t++) ; 
  if (t==10) {F_IP=0;return 255;}
  Sok[t]=malloc(sizeof(struct TSok));
  setmem((u_char *)Sok[t],0,sizeof(struct TSok));
  Sok[t]->stato=9;Sok[t]->timeout2=Sok[t]->timeout=adesso;
  Sok[t]->task=cur_task;Sok[t]->flag=4;
  Sok[t]->IPdst=ip1;Sok[t]->IPsrc=ip2;
  Sok[t]->PRTsrc=porta1;Sok[t]->PRTdst=porta2;
  Sok[t]->proto=proto;
  if (reteDEBUG&2) printf("registraIP %u %s:%s %u:%u\n",proto,NUMinIP(ip1),NUMinIP(ip2),porta1,porta2);
/*  printf("Registrasocket: Sok%u: stato=%u proto=%u src=%s:%u dst=%s:%u code:%u,%u\n", 
    t,Sok[t]->stato,Sok[t]->proto,NUMinIP(Sok[t]->IPdst),Sok[t]->PRTsrc,
    NUMinIP(Sok[t]->IPsrc),Sok[t]->PRTdst,Sok[t]->buflen,Sok[t]->buflenRX);*/
  F_IP=0;
  return t;
}


u_char deregistraIP(u_char skt) {
  while (F_IP) delay(50);
  F_IP=1;
  Debug("deregistraIP\n");
  if (Sok[skt]==NULL) {F_IP=0;return 0;}
  free(Sok[skt]);Sok[skt]=NULL;
  F_IP=0;
  return 1;
}

#undef SKK
#define SKK SokIP
#define L_ip ((u_int *)puntaTX)
#define S_ip ((u_short *)puntaTX)

u_char TxIP(u_int Ip,u_char proto,u_char *buffIP,u_short lungh) {
  u_char *puntaTX;
  if (SokIP==NULL) {
    SokIP=malloc(sizeof(struct TSok1));
    setmem(SokIP,0,sizeof(struct TSok1));
  }  
  while (SKK->flag&1) delay(50);  
  SKK->flag|=1;
  Debug("TxIP\n");
  if (lungh>(3000-SKK->buflenTX-20)) return 0;
  puntaTX=SKK->bufTX+SKK->buflenTX;
  setmem(puntaTX,0,lungh+20);
  copymem(buffIP,SKK->bufTX+SKK->buflenTX+20,lungh);
  S_ip[0]=0x0045;S_ip[1]=scbyte(lungh+20);
  S_ip[2]=0xfe54;S_ip[3]=0x40;
  puntaTX[8]=64;puntaTX[9]=proto;
  L_ip[3]=IPlocale;
  L_ip[4]=Ip;
  SKK->buflenTX+=lungh+20;
  SKK->flag&=0xfe;
  return 1;
}

u_char TrasmettiIP() {
  u_short tIp;
  if (SokIP==NULL) {
    SokIP=malloc(sizeof(struct TSok1));
    setmem(SokIP,0,sizeof(struct TSok1));
  }  
  if (!SKK->buflenTX) return 0;
  while (SKK->flag&1) delay(50);
  SKK->flag|=1;
  Debug("TrasmettiIP\n");
  tIp=scbyte(*(u_short *)(SKK->bufTX+2));
  l1IP=(unsigned short *)SKK->bufTX;
  indiIP=(unsigned int *)SKK->bufTX;
  l1IP[5]=Chk_ip();
  if (SKK->bufTX[9]==1) { l1IP[11]=0; l1IP[11]=Chk_icmp();}
  if (SKK->bufTX[9]==6) { l1IP[18]=0; l1IP[18]=Chk_tcp(SKK->bufTX);}
  if (SKK->bufTX[9]==17) { l1IP[13]=0; l1IP[13]=Chk_udp(SKK->bufTX);}
  if (*(u_short *)(SKK->bufTX+16)==IPlocale) mandaLOCALE(SKK->bufTX,tIp); else
  if (mandaPPP(SKK->bufTX,tIp)==0) { Errore(0,"mandaIP: errore mandaPPP");}
  SKK->buflenTX-=tIp;
  SKK->flag&=0xfe;
  return 1;
errore:
  SKK->flag&=0xfe;
  return 0;  
}

