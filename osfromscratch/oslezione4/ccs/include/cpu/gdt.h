#ifndef _GDT_H_
#define _GDT_H_

#include <stddef.h>

extern qword gdt[3];
void init_gdt();

#endif
