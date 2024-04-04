#ifdef __FPU_EMU__

#ifndef __MY_DOUBLE_H__
#define __MY_DOUBLE_H__

#include <fpu_type.h>
#include <fpu_const.h>
#include <fpu_emu_reg.h>

namespace myfloat {

my_double somma(my_double, my_double);

my_double moltiplica(my_double,my_double);

my_double dividi(my_double,my_double);

void compare(my_double, my_double, fpu_emu_reg*);

typedef struct {
	unsigned long long alto;
	unsigned long long basso;
} int128;

int128 somma128(int128,int128);

int128 sottrai128(int128,int128);

int confronta128(int128,int128);

}

#endif //__MY_DOUBLE_H__

#endif //__FPU_EMU__
