#define r_ICMP
#include "rete.h"

  u_char *buffer1;
  u_char *bufferPING;
  u_short *lICMP;
  int t,t1;
   
#define Icmp(ty,co,id,sn) { *(u_char *)lICMP=ty;*((u_char *)lICMP+1)=co;\
   lICMP[2]=scbyte(id); lICMP[3]=scbyte(sn); }

#define LUNGDATI 10
#define Errore(a) { printf("%s\n",a); goto errore;}

void ServerEcho(u_int Ip,u_char *buffer1,u_short lungh) {
if (reteDEBUG&2) printf("serverecho\n");
   buffer1[0]=0;/* echo reply */
   if (mandaIP(Ip,1,buffer1,lungh)==0) printf("ErroremandaIP\n"); 
}


u_char skPING=255;
u_char *bufferICMP;

u_char Ping() {
   u_int Ip;
   if (reteDEBUG&2) printf("Ping\n");
   bufferPING=malloc(8+LUNGDATI);
   printf("\nIP dest:");getstring(bufferICMP);printf("\n");
   Ip=IPinNUM(bufferICMP);
   setmem(bufferPING,0,8+LUNGDATI);
   for (t=0;t<LUNGDATI;t++) bufferPING[t+8]=t+48;
   lICMP=(u_short *)bufferPING;
   Icmp(8,0,2345,1);
   skPING=registraIP(0,0,Ip,0,1);
   if (!mandaIP(Ip,1,bufferPING,8+LUNGDATI))  Errore("Ping:errore mandaIP echoReq");
   while ((skPING!=255)&&!Sok[skPING].buflenRX&&!kbhit()) ;
      if (Sok[skPING].buflenRX)
                 printf("Ping OK\n");
            else printf("Ping fallito");    
  deregistraIP(skPING);          
  free(bufferPING);
  return 1;
 errore:
  free(bufferPING);
  return 0;
}


