#ifndef _IP_H_
#define  _IP_H_

#include <rete.h>
/* le uniche 2 funzioni che possono servire */
u_char registraIP(u_short porta1,u_short porta2,u_int ip1,u_int ip2,u_char proto);
u_char deregistraIP(u_char skt);


extern struct TSok *Sok[10];
#endif

/* di prossima implementazione le funzioni RxIP e TxIP per l'invio e la ricezione di
   pacchetti IP in formato RAW */
