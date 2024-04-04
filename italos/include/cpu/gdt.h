#ifndef _GDT_H_
#define _GDT_H_

#include <cstddef>

extern qword gdt[5];
void init_gdt();

#endif
