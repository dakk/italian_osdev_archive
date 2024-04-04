#ifndef _TCP_H_
#define _TCP_H_

#include <rete.h>
u_char connettiTCP(u_char *sk,u_int Ip,u_short Porta);/* instaurazione di una sessione TCP */
u_char disconnettiTCP(u_char sk);/* disconnessione di una sessione TCP */
u_char TxTCP(u_char sk,u_char *buffTCP,u_short lungh);/* invio dati tramite una sessione TCP */
u_char TxTCPstr(u_char sk,u_char *buff2);/* invio dati tramite una sessione TCP con l'aggiunta di CR e LF*/
u_short RxTCP(u_char sk,u_char *buffTCP,u_short lungh);/* ricezione dati da una sessione TCP */
u_short RxTCPstr(u_char sk,u_char *buffTCP,u_short lungh);/* ricezione di una stringa da una sessione TCP */

/* funzioni di sistema richiamate dalla funzione reteRX() */
u_char riceviTCP(u_char sk,u_int Ip,u_char *buffTCP,u_short lungh);
u_char nonsoTCP(u_int Ip,u_char *buffTCP,u_short lungh);
void TimeoutTCP();
#endif
