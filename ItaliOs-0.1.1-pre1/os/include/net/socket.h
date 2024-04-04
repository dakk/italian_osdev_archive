#ifndef _SOCKET_H_
#define  _SOCKET_H_

/*
 stato di TSok
   0.non esistente o disconnesso
   1.TCP inviato SYN
   2.TCP inviato SYNACK
   3.TCP connesso
   4.TCP inviato FINACK
   7.TCP ricevuto FINACK
   5.UDP binding
   6.UDP ascolto
   9.ICMP ascolto echo reply (usato dalla funzione Ping per catturare la risposta)

 di prossima implementazione:
   8.TCP ascolto (listen)
*/

  struct TSok {
    u_char stato; /* stato del socket (vedi sopra */
    u_int IPsrc,IPdst;/* indirizzi IP */
    u_short PRTsrc,PRTdst;/* porte (TCP e UDP o type,code del ICMP) */
    u_short proto;/* campo proto dell'IP 6=TCP 17=UDP 1=ICMP */
    u_int seqSIN,seqACK;/* numeri sequenza del TCP */
    u_char bufTX[3000];/* buffer in trasmissione */
    u_char bufRX[3000];/* buffer in ricezione */
    u_short buflen,buflenRX,buflen1,buflen1RX; 
    /* puntatori di scrittura e lettura dei buffer in TX e RX */
    u_int timeout;/* il momento dell'ultima operazione fatta su questo socket */
  };
  #endif
