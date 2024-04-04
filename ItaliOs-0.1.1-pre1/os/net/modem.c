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
  while (*mnd1) {
   delay(100);
   scrivibyteseriale(*(mnd1++));
  } 
}

void riceviS() {
 u_char ch;
  if (reteDEBUG&2) printf("riceviS\n");  
/* printf("\nriceviS\n"); */
 setmem(bufMDM,0,200);nbufMDM=0;
 ch=(u_char)((ritorno=leggibyteseriale())&0xff);
 while ((ch!=10)&&(ch!=13)&&(!kbhit())) {
         if (ch&&(ritorno!=NO_DATA)) {
             if (reteDEBUG&2)   printf("+%x+ ",ch); 
             bufMDM[nbufMDM]=ch; nbufMDM++;
             }
         ch=(u_char)((ritorno=leggibyteseriale())&0xff);
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
        if ((ch==13)||(ch==10)) {
           scrivibyteseriale(13);
           scrivibyteseriale(10);
        } else
           scrivibyteseriale(ch);
    }
  } 
  return 1;
}

void seriali() {
  u_short t,t1;
  u_short io[]= {0x3f8,0x2f8,0x3e8,0x2e8};
  for (t1=0;t1<4;t1++) {
    printf("\n%x: ",io[t1]);
    for (t=0;t<8;t++) {
      printf("%x ",inb(io[t1]+t));
    }
  }
  printf("\n");

}