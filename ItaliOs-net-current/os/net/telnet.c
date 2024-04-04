#define r_TELNET
#include "rete.h"

#define Errore(a) { printf("%s\n",a); goto errore;}

u_char TelSK=0;
u_char *buffTELNET;

  u_int Ip;
  u_short Porta;
  u_char ch;
  int tTEL=0;

void telnet() {
  if (reteDEBUG&2) printf("telnet\n");  
  buffTELNET=malloc(200);
  printf("\nIP dest:");getstring(buffTELNET);
  Ip=IPinNUM(buffTELNET);
  printf("\nPorta dest:");getstring(buffTELNET);printf("\n");
  Porta=atou(buffTELNET);
  if (!connettiTCP(&TelSK,Ip,Porta)) Errore("telnet: errore connessione");
  printf("Connesso\n");
  ch=0;
  while (ch!='\033') {
      if (Sok[TelSK].buflenRX) {
        buffTELNET[RxTCP(TelSK,buffTELNET,200)]=0;
	printf("%s\n",buffTELNET);
      }
      if (kbhit()) {
        printf(">");getstring(buffTELNET);ch=*buffTELNET;printf("\n");
        tTEL=strlen(buffTELNET);
	buffTELNET[tTEL]=13;
        buffTELNET[tTEL+1]=10;buffTELNET[tTEL+2]=0;
        TxTCP(TelSK,buffTELNET,tTEL+2);
      }
  }
  disconnettiTCP(TelSK);
  free(buffTELNET);
  return;
  errore:
  free(buffTELNET);
  return;
}
