#ifndef _UDP_H_
#define  _UDP_H_

u_char mandaUDP(u_char sk,u_int Ip,u_short Porta,u_char *bufUDP,u_short lungh);/* manda un pacchetto UDP */
u_char registraUDP(u_char *sk,u_short Porta,u_int Ip);/* registra la ricezione di un pacchetto UDP */
u_char deregistraUDP(u_char sk);
u_char ascoltaUDP(u_char *sk,u_short Porta,u_int Ip);
u_short RxUDP(u_char sk,u_char *buffUDP,u_short lungh);/* riceve un pacchetto UDP da un sochet */

/* richiamato da RiceviIP() */
u_char riceviUDP(u_char sk,u_char *bufUDP,u_short lungh);
#endif
