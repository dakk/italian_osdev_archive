#include <fpu_emu.h>
#include <cstddef>

using namespace myfloat;

void fpu_emu_exp::fpu_emu_exp_init(fpu_emu_reg * regi, fpu_emu_exception * exc){
	registri=regi;
	exce=exc;
	return;
}
