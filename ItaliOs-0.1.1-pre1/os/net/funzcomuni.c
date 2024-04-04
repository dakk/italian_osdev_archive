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

#define r_FUNZCOMUNI
#include "rete.h"
#include "rtc.h"

 u_int ogni1[10]={0,0,0,0,0,0,0,0,0,0};
 TIME tempo;
 int DeltaTempo=0;
 int VecTempo=100000;


void copymem(u_char *bsrc,u_char *bdst,u_short blen) {
  if (bsrc>bdst) {
     for (;blen>0;blen--) *(bdst++)=*(bsrc++);
   } else {
     for (;blen>0;blen--) bdst[blen-1]=bsrc[blen-1];
   }
}
void shiftDX(u_char *bsrc,u_short blenoff,u_short blen) {
  for (;blen>0;blen--) bsrc[blen+blenoff-1]=bsrc[blen-1];
  for (;blenoff>0;blenoff--) bsrc[blenoff-1]=0;
}
void shiftSX(u_char *bsrc,u_short blenoff,u_short blen) {
  for (;blen>0;blen--) {*bsrc=bsrc[blenoff];bsrc++;}
  for (;blenoff>0;blenoff--) *(bsrc++)=0;
}
void setmem(void *bsrc,u_char bst,u_short blen) {
  for (;blen>0;blen--) *(((u_char *)bsrc)++)=bst;
}
u_short scbyte(u_short bsrc) {
  return ((bsrc&0xff00)>>8)|((bsrc&0xff)<<8);
}
u_int scbyte4(u_int bsrc) {
  return 
   ((bsrc&0xff000000)>>24)|
   ((bsrc&0xff0000)>>8)|
   ((bsrc&0xff00)<<8)|
   ((bsrc&0xff)<<24);
}


u_int IPinNUM(char *ip) {
#define NUMERO(ap) ((ap>47)&&(ap<58))
  u_int ip1=0;
  u_char ip2=0;
  while (*ip) {
    if (NUMERO(*ip)) {
      if (ip2<26) ip2=ip2*10+*ip-48; else return 0;
    } else
    if (*ip=='.') { ip1=(ip1|(u_int)ip2<<24)>>8;ip2=0;} else return 0;
    ip++;
  }  
  return ip1=(ip1|(u_int)ip2<<24);
/*  return (ip1<<8)|(u_int)ip2; */
}

u_char *NUMinIP(u_int ip1) {
  u_char *ip2;
  u_char ip3=0,ip4=0,ip5=0;

void calcola(u_char z) {
  if (ip3>99) { 
     ip2[ip4++]=(ip5=ip3/100)+48;ip3-=ip5*100;
     ip2[ip4++]=(ip5=ip3/10)+48;ip3-=ip5*10;
  } else   
  if (ip3>9) { 
     ip2[ip4++]=(ip5=ip3/10)+48;ip3-=ip5*10;
  }
  ip2[ip4++]=ip3+48;
  ip2[ip4++]=z;
}

  ip2=malloc(16);
  setmem(ip2,0,16);

  ip3=ip1&0xff;            calcola('.');
  ip3=(ip1&0xff00)>>8;     calcola('.');
  ip3=(ip1&0xff0000)>>16;  calcola('.');
  ip3=(ip1&0xff000000)>>24;calcola(0);
  return ip2;
}

unsigned int str_str(char *s1,char *s2) {
  char *s1a,*s2a;
  s1a=s1;s2a=s2;
  while ((*s2a)&&(*s1a)) {
     if (*s1a==*s2a) {s1a++;s2a++;} else 
        { s1a-=(s2a-s2)-1;s2a=s2;}
     
  }
  if (!*s2a) return (unsigned int)(s1a-s1-(s2a-s2)+1);
  return 0;
}

u_int atou(char *nu) {
  u_int atou1;
  for (atou1=0;NUMERO(*nu);) atou1=atou1*10+*(nu++)-48;
  return atou1;
}

int Tempo() {
  int n3;
  get_time(&tempo);
  n3=tempo.fields.hour*3600+tempo.fields.minute*60+tempo.fields.second;
  if (VecTempo==100000) VecTempo=n3; 
  if (((n3-VecTempo)>3)||((n3-VecTempo)<-3)) n3=VecTempo;
/*  n3&=0x1ffff; */
/*  printf("n3=%u VecTempo=%u\n",n3,VecTempo); */
/*  if (n3<VecTempo) {DeltaTempo+=86400;n3+=86400;}   */
  VecTempo=n3;
  return n3;
}
