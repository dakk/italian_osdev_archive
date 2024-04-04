#define r_IP
#include "rete.h"

/*
   mandaIP
   riceviIP
*/

  int t;
  u_int ll;
  u_char *buf1;
  u_char *bufIPrx;


  struct TSok Sok[10];

  unsigned short *l1IP;
  unsigned int *indiIP;
  u_char LSTN=0;
  u_short Errori[10];
  u_char SockTrovato=0;
  u_int z1;

u_short crc_ip() {
/* calcolo cksum IP */
    ll=0;
    for (t=0;t<10;t++) ll+=(unsigned int)(l1IP[t]);
    for (t=0;t<2;t++) ll=((ll>>16)&0xffff)+(ll&0xffff);
    return ((unsigned short)(ll&0xffff))^0xffff;
};

u_short crc_tcp(u_char *bufc) {
/* calcolo cksum TCP */
    ll=(u_int)scbyte(scbyte(l1IP[1])-20)+((u_int)bufc[9]<<8); 
    for (t=6;t<(scbyte(l1IP[1])/2);t++) ll+=(u_int)(l1IP[t]);
    if (scbyte(l1IP[1])%2) ll+=(u_int)(l1IP[t]&0xff);
    for (t=0;t<3;t++) ll=((ll>>16)&0xffff)+(ll&0xffff);
    return ((u_short)(ll&0xffff))^0xffff;
};
u_short crc_udp(u_char *bufc) {
/* calcolo cksum UDP */
    ll=(u_int)scbyte(scbyte(l1IP[1])-20)+((u_int)bufc[9]<<8); 
/*    ll=scbyte(((u_int)(bufc[32]>>2)&0x3c))+((u_int)bufc[9]<<8)+l1IP[1]; */
    for (t=6;t<(scbyte(l1IP[1])/2);t++) ll+=(u_int)(l1IP[t]);
    if (scbyte(l1IP[1])%2) ll+=(u_int)(l1IP[t]&0xff);
    for (t=0;t<3;t++) ll=((ll>>16)&0xffff)+(ll&0xffff);
    return ((unsigned short)(ll&0xffff))^0xffff;
};

u_short crc_icmp() {
/* calcolo cksum ICMP */
    ll=0;  for (t=10;t<(scbyte(l1IP[1])/2);t++) ll+=(unsigned int)(l1IP[t]);
    if (scbyte(l1IP[1])%2) ll+=(u_int)(l1IP[t]&0xff);
    return ((unsigned short)(ll&0xffff)+(unsigned short)((ll&0xffff0000)>>16))^0xffff;
};




u_char mandaIP(u_int Ip,u_char proto,u_char *buffIP,u_short lungh) {
  if (reteDEBUG&2) printf("mandaIP\n");
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
  l1IP[5]=crc_ip(buf1);
  if (proto==1) { l1IP[11]=0; l1IP[11]=crc_icmp();}
  if (proto==6) { l1IP[18]=0; l1IP[18]=crc_tcp(buf1);}
  if (proto==17) { l1IP[13]=0; l1IP[13]=crc_udp(buf1);}
  if (Ip==IPlocale) mandaLOCALE(buf1,lungh+20); else
  if (mandaPPP(buf1,lungh+20)==0) { Errori[0]++; printf("mandaIP: errore mandaPPP\n");goto errore;}
  free(buf1);  
  return 1;
errore:
  free(buf1);  
  return 0;
}


void InizSocket() {
  for (t=0;t<10;t++)
   setmem((u_char *)&(Sok[t]),0,sizeof(struct TSok));
}

u_short riceviIP(u_char *buf2,u_short lungh) {
  bufIPrx=malloc(lungh);
  copymem(buf2,bufIPrx,lungh);
  l1IP=(unsigned short *)bufIPrx;
  indiIP=(unsigned int *)bufIPrx;
  if (reteDEBUG&2) printf("riceviIP lungh=%u\n",lungh);
  if (crc_ip()) { printf("riceviIP: errore crc_ip %04x\n",crc_ip());goto errore;}
  if (bufIPrx[9]==1) 
     if (crc_icmp()) { printf("riceviIP: errore crc_icmp %04x\n",crc_icmp());goto errore;}
  if (bufIPrx[9]==6) 
     if (crc_tcp(bufIPrx)) { printf("riceviIP: errore crc_tcp %04x\n",crc_tcp(bufIPrx));goto errore;}
  if (bufIPrx[9]==17) 
     if (crc_udp(bufIPrx)) { printf("riceviIP: errore crc_udp %04x\n",crc_tcp(bufIPrx));goto errore;}
  SockTrovato=0;
  if ((bufIPrx[9]==1)&&(l1IP[10]==0x0008)) ServerEcho(indiIP[3],bufIPrx+20,lungh-20); else
  for (t=0;t<10;t++) {
    if (Sok[t].proto==1) { /* ICMP */
     if ((Sok[t].IPsrc==0)||(Sok[t].IPsrc==indiIP[3]))
      if ((Sok[t].IPdst==0)||(Sok[t].IPdst==indiIP[4]))
       if (Sok[t].PRTsrc==scbyte(l1IP[10])) {
/*        if (Sok[t].PRTdst==scbyte(l1IP[11])) */
           if ((Sok[t].buflenRX+lungh-20)<3000) {
                         copymem(bufIPrx+20,Sok[t].bufRX+Sok[t].buflenRX,lungh-20);
                         Sok[t].buflenRX+=lungh-20;
           }
       } 
    } else
    if (Sok[t].proto!=0) { /* gli altri */
      if ((Sok[t].proto==0)||(Sok[t].proto==bufIPrx[9]))
       if ((Sok[t].IPsrc==0)||(Sok[t].IPsrc==indiIP[3]))
        if ((Sok[t].IPdst==0)||(Sok[t].IPdst==indiIP[4]))
         if ((Sok[t].PRTsrc==0)||(Sok[t].PRTsrc==scbyte(l1IP[10])))
          if ((Sok[t].PRTdst==0)||(Sok[t].PRTdst==scbyte(l1IP[11]))) {
             SockTrovato=1;
/*
             if ((Sok[t].buflenRX+lungh-20)<3000) {
                         copymem(bufIPrx+20,Sok[t].bufRX+Sok[t].buflenRX,lungh-20);
                         Sok[t].buflenRX+=lungh-20;
             }
*/

             switch (Sok[t].proto) {

              case 6: riceviTCP((u_char)t,indiIP[3],bufIPrx+20,lungh-20);
/*
                      if ((Sok[t].b
                         copymem(bufIPrx+20,Sok[t].bufRX+Sok[t].buflenRX,lungh-20);
                         Sok[t].buflenRX+=lungh-20;
                      }
*/                      
                      break;
              case 17: 
                if (!Sok[t].IPsrc) {
                  LSTN=t+1;Sok[t].IPsrc=indiIP[3];Sok[t].PRTsrc=scbyte(l1IP[10]);
                }
                riceviUDP((u_char)t,bufIPrx+20,lungh-20);
                t=LSTN-1;
                if (LSTN) {
                  LSTN=0;Sok[t].IPsrc=0;Sok[t].PRTsrc=0;
                }
                break;
                default : 
                 if ((Sok[t].buflenRX+lungh-20)<3000)
                  copymem(bufIPrx+20,Sok[t].bufRX+Sok[t].buflenRX,lungh-20);
                  Sok[t].buflenRX+=lungh+20;
/*              default : Sok[t].ricez(t,bufIPrx+20,lungh-20); break; */
                 break;
             }

            } 
               
    }
  }    
  if (!SockTrovato)
    switch (Sok[t].proto) {
      case 6: nonsoTCP(indiIP[3],bufIPrx+20,lungh-20);break; /* TCP */
      case 17: break;
  }
  free(bufIPrx);
  return lungh-20;
errore:
  free(bufIPrx);
  return 0;
}

u_char registraIP(u_short porta1,u_short porta2,u_int ip1,u_int ip2,u_char proto) {
  if (reteDEBUG&2) printf("registraIP\n");
  for (t=0;((t<10)&&(Sok[t].proto));t++) ; 
  if (t==10) return 255;
  setmem((u_char *)&(Sok[t]),0,sizeof(struct TSok));
  Sok[t].IPsrc=ip1;Sok[t].IPdst=ip2;
  Sok[t].PRTsrc=porta1;Sok[t].PRTdst=porta2;
  Sok[t].proto=proto;
/*  printf("Registrasocket: Sok%u: stato=%u proto=%u src=%s:%u dst=%s:%u code:%u,%u\n", 
    t,Sok[t].stato,Sok[t].proto,NUMinIP(Sok[t].IPsrc),Sok[t].PRTsrc,
    NUMinIP(Sok[t].IPdst),Sok[t].PRTdst,Sok[t].buflen,Sok[t].buflenRX);*/
  return t;
}


u_char deregistraIP(u_char skt) {
  if (reteDEBUG&2) printf("deregistraIP\n");
  setmem((u_char *)&(Sok[skt]),0,sizeof(struct TSok));
  return 1;
}

