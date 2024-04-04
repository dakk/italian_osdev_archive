#define r_MAIN
#include "rete.h"

 unsigned int porta=0,porta1=0x3f8;
 unsigned char RETE_=0;
 unsigned char reteDEBUG=0;
 u_char *videoMEM=(u_char *)0xB8000;
 u_char interruptRETE=4;
 extern u_short bufLOC1,bufLOC2;
 extern u_short bufTX1,bufTX2;
 extern u_short buffRX2,npunta,npunta2;
 u_char NumTEL[20],UserID[20],PassWD[20],CRLF;
 
void reteRX() {
 while (RETE_)
  {
    riceviPPP();
    riceviLOCALE();

  }
 stato=0; 
 outb(inb(porta1+4)&0xfc,porta1+4); /* RTS e DTR OFF */
 IPlocale=0;IPremoto=0;IPDNS1=0;IPDNS2=0;
 suicide();
/*  if ((ch1=uart_getchar_interrupt(porta))!=0xf1) putchar(ch1); */
/*        uart_putchar_polling(0,ch); */
}


void statoRETE() {
  int t;
  printf("\nStato RETE:%u\n",RETE_);
  printf("IPlocale=%s\n",NUMinIP(IPlocale));
  printf("IPremoto=%s\n",NUMinIP(IPremoto));
  printf("DNS1=%s DNS2=%s\n",NUMinIP(IPDNS1),NUMinIP(IPDNS2));
  printf(" ppp0: stato=%u ",stato);

  t=(int)inb(porta1+4);
  if (t&1) printf(" DTR");
  if (t&2) printf(" RTS");
  t=(int)inb(porta1+6);
  if (t&0x10) printf(" CTS");
  if (t&0x20) printf(" DSR");
  if (t&0x40) printf(" RI");
  if (t&0x80) printf(" DCD");

  printf("\n Socket:\n");
  for (t=0;t<10;t++)
    if (Sok[t].stato)
      printf("Sok%u: stato=%u proto=%u src=%s:%u dst=%s:%u code:%u,%u\n",
      t,Sok[t].stato,Sok[t].proto,NUMinIP(Sok[t].IPsrc),Sok[t].PRTsrc,
      NUMinIP(Sok[t].IPdst),Sok[t].PRTdst,Sok[t].buflen,Sok[t].buflenRX);
  printf("ppp    TX:%u %u RX:%u (%u %u)\n",bufTX1,bufTX2,buffRX2,npunta,npunta2);
  printf("locale %u %u \n",bufLOC1,bufLOC2);
}



void rete() {
 u_char buffRete[50];
 unsigned short velocita=38400;
 statoRETE();
 *buffRete=0;
 setmem(buffRete,0,50);
 while (strcmp(buffRete, "torna")) {
  printf("Help: stato term ping debug telnet connetti torna giu tick traduci\n"); 
 setmem(buffRete,0,50);
  printf("\nRete> ");getstring(buffRete);printf("\n");
  if (!strcmp(buffRete, "ping")) {
     		Ping();
	} else
/*  if (!strcmp(buffRete, "provamodem")) {
                provamodem();
	} else*/
  if (!strcmp(buffRete, "term")) {
     setmem(buffRete,0,50);
     printf("\nNumero Seriale da usare:[1..4]");getstring(buffRete);
     if (!(porta=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nVelocita': ");getstring(buffRete);
     if (!(velocita=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nInterrupt: ");getstring(buffRete);
     if (!(interruptRETE=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nCRLF= ");getstring(buffRete);
     if (!(CRLF=atou(buffRete))) return;
     switch(porta) {
      case 1: porta1=0x3f8;break;
      case 2: porta1=0x2f8;break;
      case 3: porta1=0x3e8;break;
      case 4: porta1=0x2e8;break;
     }
     if ((porta = uart_open2(porta1, INTR, velocita,NO_PARITY,BITS_8_PER_WORD,STOP_BIT_1)) == NO_DEVICE) {
 		printf("\nERRORE: impossibile aprire COM1");
 		return;
     }
     setta_irq(interruptRETE);
     printf("\nporta=seriale%u (0x%x) irq=%u vel=%u\n",porta,porta1,interruptRETE,velocita);
     printf("rete: porta=%u\n",porta);
     		ParlaModem();
	} else
  if (!strcmp(buffRete, "stato")) {
               	statoRETE();
	} else
  if (!strcmp(buffRete, "giu")) {
                 stato=0; RETE_=0;
                 outb(inb(porta1+4)&0xfc,porta1+4); /* RTS e DTR OFF */
	} else
  if (!strcmp(buffRete, "debug")) {
     		printf("   Debug=%u\n",reteDEBUG=reteDEBUG+1-3*(reteDEBUG>1));
	} else
  if (!strcmp(buffRete, "telnet")) {
               	telnet();
	} else
  if (!strcmp(buffRete, "tick")) {
               printf("%u\n",get_ticks());
	} else
  if (!strcmp(buffRete, "traduci")) {
               setmem(buffRete,0,50);
               printf("\nHost: ");getstring(buffRete);printf("->%s\n",buffRete);
               printf("%s\n",NUMinIP(TraduciDNS(buffRete)));
	} else
  if (!strcmp(buffRete, "connetti")) {
    if (RETE_) RETE_=0; else {
     printf("\nNumero Seriale da usare:[1..4]");getstring(buffRete);
     if (!(porta=atou(buffRete))) return;
     printf("\nVelocita': ");getstring(buffRete);
     if (!(velocita=atou(buffRete))) return;
     printf("\nInterrupt: ");getstring(buffRete);
     if (!(interruptRETE=atou(buffRete))) return;
     setmem(buffRete,0,50);
     printf("\nNumero Telefonico: ");getstring(NumTEL);printf("\n");
     setmem(buffRete,0,50);
     printf("\nUserID: ");getstring(UserID);
     setmem(buffRete,0,50);
     printf("\nPassWD: ");getstring(PassWD);
     printf("\nCRLF= ");getstring(buffRete);
     if (!(CRLF=atou(buffRete))) return;
     switch(porta) {
      case 1: porta1=0x3f8;break;
      case 2: porta1=0x2f8;break;
      case 3: porta1=0x3e8;break;
      case 4: porta1=0x2e8;break;
     }
     if ((porta = uart_open2(porta1, INTR, velocita,NO_PARITY,BITS_8_PER_WORD,STOP_BIT_1)) == NO_DEVICE) {
 		printf("\nERRORE: impossibile aprire COM1");
 		return;
     }
     setta_irq(interruptRETE);
     printf("\nporta=seriale%u (0x%x) irq=%u vel=%u\n",porta,porta1,interruptRETE,velocita);
     printf("rete: porta=%u\n",porta);
     if (connettiMDM(NumTEL)) printf("Connesso"); else 
       {printf("errore");goto errore;}
     inizializzaPPP();
     
 
     RETE_=1;  add_task((void *)reteRX,"ReteRX");
     printf("\n");
     delay(2000);
     mandaPPP1();
 }
 }
} 


 errore:
}


