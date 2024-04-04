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

#define r_ICMP
#include "rete.h"

  u_char *buffer1;
  u_char *bufferPING;
  u_short *lICMP;
  int t,t1;
   
#define Icmp(ty,co,id,sn) { *(u_char *)lICMP=ty;*((u_char *)lICMP+1)=co;\
   lICMP[2]=scbyte(id); lICMP[3]=scbyte(sn); }

#define LUNGDATI 10

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
   printf("\ndestinazione:");getstring(bufferICMP);
   Ip=host(bufferICMP);printf("  Ip=%s\n",NUMinIP(Ip));
   setmem(bufferPING,0,8+LUNGDATI);
   for (t=0;t<LUNGDATI;t++) bufferPING[t+8]=t+48;
   lICMP=(u_short *)bufferPING;
   Icmp(8,0,2345,1);
   skPING=registraIP(0,0,Ip,0,1);
   if (!TxIP(Ip,1,bufferPING,8+LUNGDATI))  Errore(8,"Ping:errore mandaIP echoReq");
   while ((skPING!=255)&&!Sok[skPING]->buflenRX&&!kbhit()&&(Sok[skPING]!=NULL)) ;
   if (reteDEBUG&2) printf("Ping (fine)\n");
   if (Sok[skPING]->buflenRX)
                 printf("Ping OK\n");
            else printf("Ping fallito\n");    
  deregistraIP(skPING);          
  free(bufferPING);
  return 1;
 errore:
  deregistraIP(skPING);          
  free(bufferPING);
  return 0;
}


