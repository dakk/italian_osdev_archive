#define r_FUNZCOMUNI
#include "rete.h"

u_ll ogni1[10]={0,0,0,0,0,0,0,0,0,0};

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
  ip2=malloc(16);
  setmem(ip2,0,16);

  ip3=ip1&0xff;
  if (ip3>99) { ip2[ip4++]=(ip5=ip3/100)+48;ip3-=ip5*100;}
  if (ip3>9) { ip2[ip4++]=(ip5=ip3/10)+48;ip3-=ip5*10;}
  ip2[ip4++]=ip3+48;
  ip2[ip4++]='.';

  ip3=(ip1&0xff00)>>8;
  if (ip3>99) { ip2[ip4++]=(ip5=ip3/100)+48;ip3-=ip5*100;}
  if (ip3>9) { ip2[ip4++]=(ip5=ip3/10)+48;ip3-=ip5*10;}
  ip2[ip4++]=ip3+48;
  ip2[ip4++]='.';

  ip3=(ip1&0xff0000)>>16;
  if (ip3>99) { ip2[ip4++]=(ip5=ip3/100)+48;ip3-=ip5*100;}
  if (ip3>9) { ip2[ip4++]=(ip5=ip3/10)+48;ip3-=ip5*10;}
  ip2[ip4++]=ip3+48;
  ip2[ip4++]='.';

  ip3=(ip1&0xff000000)>>24;
  if (ip3>99) { ip2[ip4++]=(ip5=ip3/100)+48;ip3-=ip5*100;}
  if (ip3>9) { ip2[ip4++]=(ip5=ip3/10)+48;ip3-=ip5*10;}
  ip2[ip4++]=ip3+48;
  ip2[ip4]=0;
     

  return ip2;
/*  return (ip1<<8)|(u_int)ip2; */
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

u_char ogni(u_char n1,u_ll n2) {
  if (ogni1[n1]<get_ticks()) {
   ogni1[n1]=get_ticks()+n2; return 1;
  } else return 0; 
}

