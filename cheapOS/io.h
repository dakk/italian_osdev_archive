
#ifndef _IO_
#define _IO_

#include "config.h"

//Primitive di accesso alle porte i/o
inline unsigned char inportb(unsigned int port);
inline unsigned int inportw(unsigned int port);
inline void outportb(unsigned int port, unsigned char val);
inline void outportw(unsigned int port, unsigned int val);

#endif
