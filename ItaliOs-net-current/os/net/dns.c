#define r_DNS
#include "rete.h"

#define Errore(a)  { printf("%s\n",a); goto errore;}
#define Short(a) scbyte(*(u_short *)(bufDNS1+a))

u_char *bufDNS1;
int t,t1;
u_char sokDNS;
u_short TranID;

u_int TraduciDNS(u_char *dnsn) {
  u_short buflun=10000;
  u_int Ip;
/*  Ip=IPinNUM("192.168.0.1"); */
  if (!strcmp(dnsn,LOCALHOST)) { printf("\nDNSRisposta=%s\n",NUMinIP(IPlocale));return 1;}
  Ip=IPDNS1;
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
  printf("\nDNS=");
  for (t=0;t<(t1+17);t++) printf("%x ",bufDNS1[t]);
  printf("\n");
  registraUDP(&sokDNS,53,Ip);
  if (!mandaUDP(255,Ip,53,bufDNS1,t1+17))  Errore("nonsoTCP:errore mandaIP RST");

  buflun=RxUDP(sokDNS,bufDNS1,buflun);
  deregistraUDP(sokDNS);
  if (sokDNS!=255) {
    if ((*(u_short *)bufDNS1)==TranID) {
      if (reteDEBUG&2) printf("TraduciDNS buflun=%u\n",buflun);
      printf("\nDNSrisposta=");
      for (t=0;t<buflun;t++) if (t<150) printf("%x ",bufDNS1[t]);
      printf("\n");
      if (Short(2)!=0x8180) Errore("Risposta errata\n");
      if (Short(6)==0) Errore("Risposta mancante\n");
      t=12;printf("\n");
      while (bufDNS1[t]&&(t<buflun)) t+=bufDNS1[t]+1;
      t+=5+2;
      while ((Short(t)!=0x0001)&&(t<buflun)) {
         t+=Short(t+8)+2+8+2;
      }
      printf("%s\n",NUMinIP(*(u_int *)(bufDNS1+10+t)));
      sokDNS=255;
      if (t<buflun) return *(u_int *)(bufDNS1+10+t); else return 0;
    }
  }  
  
  free(bufDNS1);
  return 1;
errore:
  free(bufDNS1);
  return 0;
  
}
