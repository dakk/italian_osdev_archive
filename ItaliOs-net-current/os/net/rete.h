#include <io.h>
#include <keyboard.h>
#include <mem.h>
#include <serial.h>
#include <task.h>
#include <stdio.h>
#include <string.h>
#include <delay.h>
#include <video.h>

#define u_char unsigned char
#define u_short unsigned short
#define u_int unsigned int
#define u_ll unsigned long long

/*
#define scrivibyteseriale(a) uart_putchar_polling(porta,a)
#define leggibyteseriale(porta)  uart_getchar_interrupt(porta)
*/

/*
 stato di TSok
   0.non esistente o disconnesso
   1.mandato SYN
   2.mandato SYNACK
   3.connesso
   3.inviato FINACK
*/

  struct TSok {
    u_char stato;
    u_int IPsrc,IPdst;
    u_short PRTsrc,PRTdst;
    u_short proto;
    u_int seqSIN,seqACK;
    u_char bufTX[3000];
    u_char bufRX[3000];
    u_short buflen,buflenRX; 
  };

/*
#define USERID "prova1"
#define PASSWORD "prova2"
#define NUMEROTEL "0422326888"
*/
#define LOCALHOST "italianos"

extern unsigned long long get_ticks(void);

#ifndef r_MAIN
extern unsigned char RETE_;
extern unsigned int porta,porta1;
extern unsigned char reteDEBUG;
extern u_char NumTEL[20],UserID[20],PassWD[20],CRLF;
#endif


#ifndef r_ICMP
extern void ServerEcho(u_int Ip,u_char *buffer1,u_short lungh);
extern u_char Ping();
#endif

#ifndef r_PPP
extern void mandaPPP1();
extern u_char  inizializzaPPP();
extern u_char  chiudiPPP();
extern u_char  mandaPPP(u_char *bufin,u_short lungh);
extern u_short riceviPPP();
extern u_int   IPlocale,IPremoto;
extern u_int   IPDNS1,IPDNS2;
extern u_char  stato;
extern u_int indirizzoPPP();
#endif

#ifndef r_LOCALE
extern u_char mandaLOCALE(u_char *bufLOC,u_short lungh);
extern u_char riceviLOCALE();
#endif

/*#ifndef r_LINUX
extern u_char leggibyteseriale();
extern void scrivibyteseriale(u_char CH);
extern void apriseriale(u_char num);
extern void chiudiseriale();
extern u_char NIENTE;
extern void BarraSocket();
#endif*/

#ifndef r_FUNZCOMUNI
extern void copymem(u_char *bsrc,u_char *bdst,u_short blen);
extern void shiftDX(u_char *bsrc,u_short blenoff,u_short blen);
extern void shiftSX(u_char *bsrc,u_short blenoff,u_short blen);
extern void setmem(void *bsrc,u_char bst,u_short blen);
extern u_short scbyte(u_short bsrc);
extern u_int scbyte4(u_int bsrc);
extern u_int IPinNUM(char *ip);
extern u_char *NUMinIP(u_int ip1);
extern unsigned int str_str(char *s1,char *s2);
extern unsigned int atou(char *nu);
extern u_int ogni(u_char n1,u_int n2);
#endif

#ifndef r_IP
extern u_int z1;
extern u_char mandaIP(u_int Ip,u_char proto,u_char *buff,u_short lungh);
extern u_short riceviIP(u_char *buff,u_short lungh);
extern u_char registraIP(u_short porta1,u_short porta2,u_int ip1,u_int ip2,u_char proto);
extern u_char deregistraIP(u_char skt);
extern struct TSok Sok[10];
extern u_short Errori[10];
#endif

#ifndef r_TCP
extern u_char connettiTCP(u_char *sk,u_int Ip,u_short Porta);
extern u_char disconnettiTCP(u_char sk);
extern u_char TxTCP(u_char sk,u_char *buff1,u_short lungh);
extern u_short RxTCP(u_char sk,u_char *buffTCP,u_short lungh);
extern u_char riceviTCP(u_char sk,u_int Ip,u_char *buffTCP,u_short lungh);
extern u_char nonsoTCP(u_int Ip,u_char *buffTCP,u_short lungh);
#endif
#ifndef r_UDP
extern u_char mandaUDP(u_char sk,u_int Ip,u_short Porta,u_char *bufUDP,u_short lungh);
extern u_char riceviUDP(u_char sk,u_char *bufUDP,u_short lungh);
extern u_char registraUDP(u_char *sk,u_short Porta,u_int Ip);
extern u_char deregistraUDP(u_char sk);
extern u_char ascoltaUDP(u_char *sk,u_short Porta,u_int Ip);
extern u_short RxUDP(u_char sk,u_char *buffUDP,u_short lungh);
#endif
#ifndef r_DNS
extern u_int TraduciDNS(u_char *dnsn);
#endif
#ifndef r_MODEM
extern u_char connettiMDM(u_char *num);
extern u_char ParlaModem();
#endif
#ifndef r_TELNET
extern void telnet();
#endif


/*#define NO_DATA 0x5555 */


#ifndef r_SERIALE
/*extern void provamodem(); */
extern unsigned int uart_open2(unsigned short number1, unsigned char mode,
		       unsigned int velocita, unsigned char parita,
		       unsigned char len_word, unsigned char stop_bit);
extern void scrivibyteseriale(u_char Chg);
extern u_int leggibyteseriale();
extern void setta_irq(u_char Uirq);
#endif
