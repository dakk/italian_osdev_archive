#ifndef _PRINTER_H_
#define _PRINTER_H_


#define LP_PBUSY	0x80 /* active low */
#define LP_PACK		0x40 /* active low */
#define LP_POUTPA	0x20
#define LP_PSELECD	0x10
#define LP_PERRORP	0x08 /* active low*/

#define LP_PINTEN	0x10
#define LP_PSELECP	0x08
#define LP_PINITP	0x04  /* active low */
#define LP_PAUTOLF	0x02
#define LP_PSTROBE	0x01

int reset_lp(unsigned int number);

void print(unsigned char lpchar, unsigned int number);

#endif
