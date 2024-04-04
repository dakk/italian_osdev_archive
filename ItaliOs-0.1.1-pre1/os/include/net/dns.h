#ifndef _DNS_H_
#define  _DNS_H_

#include <rete.h>

u_int TraduciDNS(u_char *dnsn); /* funzione che interroga il DNS e che restituisce l'IP */
u_int host(u_char *shost);  /* funzione che prima prova con IPinNUM e se non va  a buon fine prova con TraduciDNS */
#endif
