#ifndef _PARALLEL_H_
#define _PARALLEL_H_

#include <mytypes.h>

#define DATA 0
#define STATUS 1
#define CONTROL 2

void printx(int d) ;
void detect_lpt_base_address(void);

/*num_port può assumere valori 1 , 2 , 3 e indica il numero della porta*/
/*se viene rotirnato -1 c'è il num_port sbagliato*/
int get_data(unsigned int num_port);
int get_status(unsigned int num_port);

BOOL put_control(unsigned char control, unsigned int num_port);
BOOL put_data(unsigned char data, unsigned int num_port);

#endif
