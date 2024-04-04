#define r_MODEM
#include "rete.h"

int t;
/*
#define SERIALE0 0x3f8
#define SERIALE1 0x2f8
#define SERIALE2 0x3e8
#define SERIALE3 0x2e8
*/


u_char *bufMDM;
u_short nbufMDM=0;
u_int ritorno;

void mandaS(u_char *mnd1) {
  if (reteDEBUG&2) printf("mandaS\n");  
  printf(">%s\n",mnd1);
  while (*mnd1)
   if ((*mnd1=='\n')||(*mnd1=='\r')) {
     scrivibyteseriale(CRLF);mnd1++;
   } else scrivibyteseriale(*(mnd1++));
}

void riceviS() {
 u_char ch;
  if (reteDEBUG&2) printf("riceviS\n");  
/* printf("\nriceviS\n"); */
 setmem(bufMDM,0,200);nbufMDM=0;
 ch=(u_char)((ritorno=leggibyteseriale())&0xff);
 while ((ch!=10)&&(ch!=13)&&(!kbhit())) {
/*         printf("-%x- ",ch); */
         if (ch&&(ritorno!=NO_DATA)) { bufMDM[nbufMDM]=ch; nbufMDM++;}
         ch=(u_char)((ritorno=leggibyteseriale())&0xff);
/*         printf("+%x+ ",ch); */
 }
 bufMDM[nbufMDM]=0;
/*  printf("\nriceviS_F %s\n",bufMDM); */
}

u_char connettiMDM(u_char *num) {
  if (reteDEBUG&2) printf("connettiMDM\n");  
  if (bufMDM==0) bufMDM=malloc(200);
  setmem(bufMDM,0,200);nbufMDM=0;
  mandaS("\rat\r");
  while ((str_str(bufMDM,"OK")==0)&&(!kbhit())) {
   riceviS();
   if (*bufMDM) printf("(%s)\n",bufMDM);
   if (str_str(bufMDM,"ABORT")!=0) goto errore;
   if (str_str(bufMDM,"DIAL")!=0) goto errore;
   if (str_str(bufMDM,"ERROR")!=0) goto errore;
  } 
  printf("Modem: OK\n");
  mandaS("atdt");
  mandaS(num);
  mandaS("\r");
  while (!kbhit()) {
   riceviS();
   if (*bufMDM) printf("(%s)\n",bufMDM);
   if (str_str(bufMDM,"CONNECT")!=0) { free(bufMDM); return 1; }
   if (str_str(bufMDM,"ABORT")!=0) goto errore;
   if (str_str(bufMDM,"DIAL")!=0) {
     mandaS("atx4dt");mandaS(num);mandaS("\r");}
   if (str_str(bufMDM,"ERROR")!=0) goto errore;
  }
errore:
  free(bufMDM);  
  return 0;
}

u_char ParlaModem() {
  u_char ch;
  ch=0;
  while (ch!=27) {
    ch=(u_char)((ritorno=leggibyteseriale())&0xff);
    if (ch&&(ritorno!=NO_DATA)) putchar(ch);
    ch=0;
    if (kbhit()) { 
        ch=getch(); 
        if ((ch==13)||(ch==10)) ch=CRLF;
        scrivibyteseriale(ch);
    }
  } 
  return 1;
}

