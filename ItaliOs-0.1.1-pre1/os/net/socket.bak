#define r_SOCKET
#include "rete.h"

#define SKK Sok[sk]
/* ritardo per accedere al socket */
#define Rit1 50
/* ritardo di polling al socket */
#define Rit2 10

u_char trasmetti(u_char sk,u_char *bufferTX,u_short lungh) {
    if (SKK!=NULL) {             /* se il socket esiste */
     if (cur_task==SKK->task) {    /* se il task corrente ne e' proprietario */
       Debug("trasmetti ");
       delay(Rit1); 
       while (SKK->flag&1) {
          if (SKK==NULL) return 0;
          delay(Rit2);
       }   /* aspetta che si liberi il socket */
       Debug(" 4\n");
       SKK->flag|=1;                  /* prende il controllo del socket */
       if ((lungh+SKK->buflenTX)<MAX_LUN_SK) { /* se ci sta nel buffer in Tx */
         copymem(bufferTX,SKK->bufTX+SKK->buflenTX,lungh); /* SALVA SUL BUFF IN TX */
         SKK->buflenTX+=lungh; 
         SKK->timeout=0;
       } else return 0;         /* se non ci sta nel buffer */
       SKK->flag&=0xfe;         /* rilascia il controllo del socket */  
     } else return 0;           /* se non e' il task corrente */
    } else return 0;            /* se il socket non esiste */
  return 1;
}

u_char trasmetti_str(u_char sk,u_char *buff2) {
 u_short lungh2;
 lungh2=strlen(buff2);
 buff2[lungh2]=13;buff2[lungh2+1]=10;buff2[lungh2+2]=0;
 return trasmetti(sk,buff2,lungh2+2);
}



u_short ricevi(u_char sk,u_char *bufferRX,u_short lungh) {
    if (SKK!=NULL) {             /* se il socket esiste */
     if (cur_task==SKK->task) {    /* se il task corrente ne e' proprietario */
       delay(Rit1);
       while (SKK->flag&2) {
          if (SKK==NULL) return 0;
          delay(Rit2);
       }   /* aspetta che si liberi il socket */
       SKK->flag|=2;                  /* prende il controllo del socket */
       if (lungh>=SKK->buflenRX) {
          copymem(SKK->bufRX,bufferRX,SKK->buflenRX);
          lungh=SKK->buflenRX;SKK->buflenRX=0;
       } else {
          copymem(SKK->bufRX,bufferRX,lungh);
          copymem(SKK->bufRX+lungh,SKK->bufRX,SKK->buflenRX-lungh);
          SKK->buflenRX-=lungh;
       }
       SKK->flag&=0xfd;         /* rilascia il controllo del socket */  
     } else return 0;           /* se non e' il task corrente */
    } else return 0;            /* se il socket non esiste */
  if (lungh) Debug("Ricevi_str\n");
  return lungh;
}

u_short ricevi_str(u_char sk,u_char *buffTCP,u_short lungh) {
  u_short tTCP,tTCP1;
    if (SKK!=NULL) {             /* se il socket esiste */
     if (cur_task==SKK->task) {    /* se il task corrente ne e' proprietario */
       delay(Rit1); 
       while (SKK->flag&2) {
          if (SKK==NULL) return 0;
          delay(Rit2);
       }   /* aspetta che si liberi il socket */
       SKK->flag|=2;                 /* prende il controllo del socket */
       for (tTCP=0,tTCP1=0;(tTCP1<lungh)&&(SKK->bufRX[tTCP]!=10)&&(tTCP<SKK->buflenRX);tTCP++)
        if (SKK->bufRX[tTCP]>31)
         { buffTCP[tTCP1]=SKK->bufRX[tTCP];tTCP1++;} /* salva solo caratteri validi */
      
       if ((SKK->bufRX[tTCP]!=10)) {
        if (tTCP1<lungh) { tTCP1=0; goto fine2; } else {    
         copymem(SKK->bufRX+tTCP-1,SKK->bufRX,SKK->buflenRX-tTCP+1);
         SKK->buflenRX-=(tTCP-1);
         buffTCP[tTCP1]=0;tTCP1--;
         goto fine2;
        }
       }
       else {
        copymem(SKK->bufRX+tTCP+1,SKK->bufRX,SKK->buflenRX-tTCP-1);
        SKK->buflenRX-=(tTCP+1);
        buffTCP[tTCP1]=0;
        goto fine2;
       } 
  } else return 0;
 } else return 0;
  return 0;
fine2:
  SKK->flag&=0xfd;         /* rilascia il controllo del socket */  
  if (tTCP1) Debug("Ricevi_str\n");
  return tTCP1;  
}


#undef SKK
#define SKK Sok[sk]



#undef SKK
#define SKK Sok[tSOK]
void GestioneSocket() {/* gestione della trasmissione dei socket */
   u_int tSOK;
   for (tSOK=0;tSOK<10;tSOK++) {            /* scansione dei socket esistenti */
       if (SKK!=NULL)                       /* se il socket esiste */
         if (!(SKK->flag&4)) {              /* se il socket non e' stato appena creato */
          if (SKK->proto==6) TimeoutTCP(tSOK);  /* se e' TCP */
         }
   }
   TrasmettiIP();          /* svuota il buffer in Tx del socket IP (raw) */
}

