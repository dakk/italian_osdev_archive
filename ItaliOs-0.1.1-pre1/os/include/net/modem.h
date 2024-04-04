#ifndef _MODEM_H_
#define  _MODEM_H_

#include <rete.h>
u_char connettiMDM(u_char *num);/* seleziona un NUMERO TELEFONICO su una
                                   seriale precedentemente aperta */
u_char ParlaModem();/* programma molto spartano simile al minicom di linux */
#endif
