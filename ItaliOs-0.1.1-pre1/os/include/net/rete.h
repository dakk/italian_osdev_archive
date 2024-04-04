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
#define USERID "prova1"
#define PASSWORD "prova2"
#define NUMEROTEL "0422326888"
*/
#define LOCALHOST "ItaliOS"

#include <icmp.h>
#include <ppp.h>
#include <ip.h>
#include <tcp.h>
#include <udp.h>
#include <dns.h>
#include <modem.h>
#include <telnet.h>
#include <funzioni_comuni.h>
#include <locale.h>
#include <socket.h>


extern unsigned long long get_ticks(void);

#ifndef r_MAIN
extern unsigned char RETE_;
extern unsigned int porta,porta1;
extern unsigned short reteDEBUG;
extern u_char NumTEL[20],UserID[20],PassWD[20],CRLF;
#endif
#ifndef r_IRC
extern void irc(u_int Ip,u_short Porta);
#endif

#ifndef r_SERIALE
/*extern void provamodem(); */
extern unsigned int uart_open2(unsigned short number1, unsigned char mode,
		       unsigned int velocita, unsigned char parita,
		       unsigned char len_word, unsigned char stop_bit);
extern void scrivibyteseriale(u_char Chg);
extern u_int leggibyteseriale();
extern void setta_irq(u_char Uirq);
#endif
