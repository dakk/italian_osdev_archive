#ifndef _TASTIERA_H_
#define _TASTIERA_H_

#include <cstddef>

void init_tastiera();
dword get_char();
void setleds(byte led_status);
byte kbpress();


#endif
