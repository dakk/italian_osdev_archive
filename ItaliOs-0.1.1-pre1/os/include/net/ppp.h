#ifndef _PPP_H_
#define  _PPP_H_

#include <rete.h>

/* descrizione bit della variabile stato 
  0x0001: Tx LCP ConfReq
  0x0002: Rx LCP ConfAck
  0x0004: Rx LCP ConfReq
  0x0008: Tx LCP ConfAck
  0x0010: Tx PAP Req
  0x0020: Rx PAP Ack
  0x0040: Tx IPCP ConfReq
  0x0080: Rx IPCP ConfAck
  0x0100: Rx IPCP ConfReq
  0x0200: Tx IPCP ConfAck
  0x0400: Condizione di Active-Open
  0x0800: Condizione di TIMEOUT
  0x1000: Rx LCP packet
  0x2000: Rx PAP packet
  0x4000: Rx IPCP packet
  0x8000: Rx other packet
*/

#define A_OPEN_PPP stato=0x0400
u_char  inizializzaPPP();
u_char  chiudiPPP();
u_char  mandaPPP(u_char *bufin,u_short lungh);
u_short riceviPPP();
/* le uniche funzioni che possono servire */
u_int IPlocale,IPremoto;/* si commentano da sole */
u_int IPDNS1,IPDNS2;/* anche queste */
u_int stato;/* stato della connessione PPP */
u_int indirizzoPPP();
#endif
