#include <fpu_emu.h>
#include <cstddef>

using namespace myfloat;

void fpu_emu_mov::fpu_emu_mov_init(fpu_emu_reg * regi, fpu_emu_exception * exc){
	registri=regi;
	exce=exc;
	return;
}

void fpu_emu_mov::fcmovb(unsigned int regi){
	int top = (registri->get_sr() & 0x3800) >> 11;
	unsigned int flag = get_flag();
	if (flag & fpu_const::CPU_CF) {
		registri->R[top]= ((regi+top)>7)?registri->R[top+regi-8]:registri->R[top+regi];
	}
	return;
}

void fpu_emu_mov::fcmovnb(unsigned int regi){
	int top = (registri->get_sr() & 0x3800) >> 11;
	unsigned int flag = get_flag();
	if (!(flag & fpu_const::CPU_CF)) {
		registri->R[top]= ((regi+top)>7)?registri->R[top+regi-8]:registri->R[top+regi];
	}
	return;
}

void fpu_emu_mov::fcmove(unsigned int regi){
	int top = (registri->get_sr() & 0x3800) >> 11;
	unsigned int flag = get_flag();
	if (flag & fpu_const::CPU_ZF) {
		registri->R[top]= ((regi+top)>7)?registri->R[top+regi-8]:registri->R[top+regi];
	}
	return;
}

void fpu_emu_mov::fcmovne(unsigned int regi){
	int top = (registri->get_sr() & 0x3800) >> 11;
	unsigned int flag = get_flag();
	if (!(flag & fpu_const::CPU_ZF)) {
		registri->R[top]= ((regi+top)>7)?registri->R[top+regi-8]:registri->R[top+regi];
	}
	return;
}

void fpu_emu_mov::fcmovbe(unsigned int regi){
	int top = (registri->get_sr() & 0x3800) >> 11;
	unsigned int flag = get_flag();
	if (flag & (fpu_const::CPU_CF | fpu_const::CPU_ZF)) {
		registri->R[top]= ((regi+top)>7)?registri->R[top+regi-8]:registri->R[top+regi];
	}
	return;
}

void fpu_emu_mov::fcmovnbe(unsigned int regi){
	int top = (registri->get_sr() & 0x3800) >> 11;
	unsigned int flag = get_flag();
	if (!(flag & (fpu_const::CPU_CF | fpu_const::CPU_ZF))) {
		registri->R[top]= ((regi+top)>7)?registri->R[top+regi-8]:registri->R[top+regi];
	}
	return;
}

void fpu_emu_mov::fcmovu(unsigned int regi){
	int top = (registri->get_sr() & 0x3800) >> 11;
	unsigned int flag = get_flag();
	if (flag & fpu_const::CPU_PF) {
		registri->R[top]= ((regi+top)>7)?registri->R[top+regi-8]:registri->R[top+regi];
	}
	return;
}

void fpu_emu_mov::fcmovnu(unsigned int regi){
	int top = (registri->get_sr() & 0x3800) >> 11;
	unsigned int flag = get_flag();
	if (!(flag & fpu_const::CPU_PF)) {
		registri->R[top]= ((regi+top)>7)?registri->R[top+regi-8]:registri->R[top+regi];
	}
	return;
}
