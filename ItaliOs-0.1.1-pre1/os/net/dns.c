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

#define r_DNS
#include "rete.h"

#define Short(a) scbyte(*(u_short *)(bufDNS1+a))

u_char *bufDNS1;
int t,t1;
u_char sokDNS;
u_short TranID;

u_int TraduciDNS(u_char *dnsn,u_int Ip) {
  u_short buflun=10000;
/*  Ip=IPinNUM("192.168.0.1"); */
  if (!strcmp(dnsn,LOCALHOST)) return IPlocale;
  bufDNS1=malloc(buflun);
  setmem(bufDNS1,0,1000);TranID++;
  copymem((u_char *)&TranID,bufDNS1,2);
  copymem((u_char *)"\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00",bufDNS1+2,10);
  t=t1=0;
  while (dnsn[t1]) {
   while((dnsn[t1]!='.')&&(dnsn[t1])) t1++;
   bufDNS1[t+12]=(u_char)(t1-t);
   copymem(dnsn+t,bufDNS1+12+t+1,(u_short)(t1-t));
   t=(++t1);
  }
  copymem((u_char *)"\x00\x00\x01\x00\x01",bufDNS1+t1+12,5);
/*  printf("\nDNS="); 
  for (t=0;t<(t1+17);t++) printf("%x ",bufDNS1[t]);
  printf("\n");*/
  registraUDP(&sokDNS,53,Ip);
  if (!mandaUDP(255,Ip,53,bufDNS1,t1+17))  Errore(8,"nonsoTCP:errore mandaIP RST");

  while (((buflun=RxUDP(sokDNS,bufDNS1,buflun))==0)&&(Sok[sokDNS]!=NULL)&&((adesso-Sok[sokDNS]->timeout2)<5)&&
        (!kbhit()))  delay(50);

  if (Sok[sokDNS]==NULL) goto errore;
  if (buflun==0) goto errore2;
  deregistraUDP(sokDNS);
  if (sokDNS!=255) {
    if ((*(u_short *)bufDNS1)==TranID) {
      if (reteDEBUG&2) printf("TraduciDNS buflun=%u\n",buflun);
/*      printf("\nDNSrisposta=");
      for (t=0;t<buflun;t++) if (t<150) printf("%x ",bufDNS1[t]);
      printf("\n"); */
      if (Short(2)!=0x8180) Errore(8,"Risposta errata\n");
      if (Short(6)==0) Errore(8,"Risposta mancante\n");
      t=12;
      while (bufDNS1[t]&&(t<buflun)) t+=bufDNS1[t]+1;
      t+=5+2;
      while ((Short(t)!=0x0001)&&(t<buflun)) {
         t+=Short(t+8)+2+8+2;
      }
/*      printf("%s\n",NUMinIP(*(u_int *)(bufDNS1+10+t))); */
      sokDNS=255;
      if (t<buflun) return Ip=*(u_int *)(bufDNS1+10+t); else Ip=0;
      free(bufDNS1); return Ip;
    }
  }  
  
errore2:
  deregistraUDP(sokDNS);
errore:
  free(bufDNS1);
  return 0;
  
}

u_int host(u_char *shost) {
  u_int shost1;
  if ((shost1=IPinNUM(shost))==0) 
  if ((shost1=TraduciDNS(shost,IPDNS1))==0)
  if ((shost1=TraduciDNS(shost,IPDNS2))==0) return 0;
  return shost1;
}
