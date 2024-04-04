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
   4.inviato FINACK
   7.ricevuto FinAck
   5.registraUDP
   6.ascoltaUDP
   9.ascoltaEchoreply
*/

  struct TSok {
    u_char stato;
    u_int IPsrc,IPdst;
    u_short PRTsrc,PRTdst;
    u_short proto;
    u_int seqSIN,seqACK;
    u_char bufTX[3000];
    u_char bufRX[3000];
    u_int task;
    u_char flag;/* 1 per TX e 2 per RX 4 per appena creato */
    u_short buflenTX,buflenRX; 
    int timeout,timeout2; /* necessari 2 timeout in secondi*/
  };

extern u_int cur_task;
#define MAX_LUN_SK 3000

/*
#define USERID "prova1"
#define PASSWORD "prova2"
#define NUMEROTEL "0422326888"
*/
#define LOCALHOST "ItaliOS"

#define FIN 0x01
#define SYN 0x02
#define RST 0x04
#define PSH 0x08
#define ACK 0x10
#define URG 0x20



#ifndef r_MAIN
extern int adesso;
extern u_short leds;
extern unsigned char RETE_;
extern unsigned int porta,porta1;
extern unsigned short reteDEBUG;
extern u_char NumTEL[20],UserID[20],PassWD[20],CRLF;
extern u_short Errori[10];
#endif
#ifndef r_IRC
extern void irc(u_int Ip,u_short Porta);
#endif
#ifndef r_SOCKET
extern u_char trasmetti(u_char sk,u_char *bufferTX,u_short lungh);
extern u_char trasmetti_str(u_char sk,u_char *buff2);
extern u_short ricevi(u_char sk,u_char bufferRX,u_short lungh);
extern u_short ricevi_str(u_char sk,u_char *buffTCP,u_short lungh);
extern void GestioneSocket();
#endif

#ifndef r_ICMP
extern void ServerEcho(u_int Ip,u_char *buffer1,u_short lungh);
extern u_char Ping();
#endif

#define A_OPEN_PPP stato=0x0400
#ifndef r_PPP
extern u_char  inizializzaPPP();
extern u_char  chiudiPPP();
extern u_char  mandaPPP(u_char *bufin,u_short lungh);
extern u_short riceviPPP();
extern u_int   IPlocale,IPremoto;
extern u_int   IPDNS1,IPDNS2;
extern u_int  stato;
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
extern int Tempo();
#endif

#ifndef r_IP
extern u_int z1;
extern u_char mandaIP(u_int Ip,u_char proto,u_char *buff,u_short lungh);
extern u_short riceviIP(u_char *buff,u_short lungh);
extern u_char registraIP(u_short porta1,u_short porta2,u_int ip1,u_int ip2,u_char proto);
extern u_char deregistraIP(u_char skt);
extern u_char TxIP(u_int Ip,u_char proto,u_char *buffIP,u_short lungh);
extern u_char TrasmettiIP();

extern struct TSok *Sok[10];
#endif
#define SkEsiste(nn) (Sok[nn]!=NULL)
#ifndef r_TCP
extern void Init_tcp();
extern u_char disconnettiTCP(u_char sk);
extern u_char connettiTCP(u_char *sk,u_int Ip,u_short Porta);
extern u_char riceviTCP(u_char sk,u_int Ip,u_char *buffTCP,u_short lungh);
extern u_char nonsoTCP(u_int Ip,u_char *buffTCP,u_short lungh);
extern void TimeoutTCP();
extern u_char ascoltaTCP(u_short Porta);
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
extern u_int TraduciDNS(u_char *dnsn,u_int Ip);
extern u_int host(u_char *shost);
#endif
#ifndef r_MODEM
extern void seriali();
extern u_char connettiMDM(u_char *num);
extern u_char ParlaModem();
#endif
#ifndef r_TELNET
extern void TelnetRX(u_short Porta);
extern void telnet(u_int Ip,u_short Porta);
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
#define Debug(dd) if (reteDEBUG&2) printf(dd);
#define Errore(b,a) { Errori[b]++; if (reteDEBUG&2) printf("%s\n",a); goto errore; }

/*
 Errori:
  0: mandaIP: errore mandaPPP
  1: riceviIP: errore check
  2: TxTCP
  3: ConnettiTCP,disconnettiTCP
  4,"rxTCP:SynAck e socket!=1");
  5,"rxTCP:SynAck e seqSIN!=+1");
  6,"rxTCP:SynAck errore mandaIP");
  4,"rxTCP:FinAck e socket!=3");
  6,"rxTCP:FinAck errore mandaIP ACK");
  7,"rxTCP:PshAck errore seqSIN");
  7,"rxTCP:PshAck errore seqACK");
  7,"rxTCP:PshAck errore seqACK fuorirange");
  6,"rxTCP:PshAck errore mandaIP");
  6,"nonsoTCP:errore mandaIP RST");
*/

