#define r_LOCALE
#include "rete.h"

u_char F_LOCALE=0;
u_char *buffLOCALE=0;
u_char *buffLOtx;
u_short bufLOC1=0,bufLOC2=0;
u_short lunghLO=0;

void mandaLOCC(u_char Bb) {
    buffLOCALE[bufLOC1]=Bb;
    bufLOC1=(bufLOC1+1)%10000;
    if (bufLOC1==bufLOC2) bufLOC1=(bufLOC1-1+10000)%10000;
}
u_char riceviLOCC() {
  u_char Bb; 
    if (bufLOC1==bufLOC2) return 0;
    Bb=buffLOCALE[bufLOC2];
    bufLOC2=(bufLOC2+1)%10000;
    return Bb;
}

u_char mandaLOCALE(u_char *bufLOC,u_short lungh) {
  u_int Mt;
  if (buffLOCALE==0) buffLOCALE=malloc(10000);
  while (F_LOCALE) ;
  F_LOCALE=1;  
  mandaLOCC((u_char)(lungh&0xff));mandaLOCC((u_char)((lungh&0xff00)>>8));
  for (Mt=0;Mt<lungh;Mt++) mandaLOCC(bufLOC[Mt]);
  F_LOCALE=0;  
  return 1;
}

u_char riceviLOCALE() {
  u_int Mt;
  if (bufLOC2!=bufLOC1) {   /* ci sono dati */
    lunghLO=(u_short)riceviLOCC()|((u_short)riceviLOCC()<<8);
    buffLOtx=malloc(lunghLO);
    for (Mt=0;Mt<lunghLO;Mt++) buffLOtx[Mt]=riceviLOCC();
    riceviIP(buffLOtx,lunghLO);
    free(buffLOtx);
    return 1;
  }  
  return 0;
}
