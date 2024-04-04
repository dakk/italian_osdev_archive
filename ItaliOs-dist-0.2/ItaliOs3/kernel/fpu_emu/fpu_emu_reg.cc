#include <fpu_emu.h>
#include <cstddef>

using namespace myfloat;

word fpu_emu_reg::get_cr(void){
	return CR & 0xFFFF;
}

void fpu_emu_reg::set_cr(word dato){
	CR = dato & 0xFFFF;
	return;
}

word fpu_emu_reg::get_sr(void){
	return SR & 0xFFFF;
}

void fpu_emu_reg::set_sr(word dato){
	SR = dato & 0xFFFF;
	return;
}

word fpu_emu_reg::get_tr(void){
	return TR & 0xFFFF;
}

void fpu_emu_reg::set_tr(word dato){
	TR = dato & 0xFFFF;
	return;
}

word fpu_emu_reg::get_opcode(void){
	return OPCODE & 0x07FF;
}

void fpu_emu_reg::set_opcode(word dato){
	OPCODE=dato & 0x07FF;
	return;
}

qword fpu_emu_reg::get_fip(void){
	return FIP & 0x0000FFFFFFFFFFFFLL;
}

void fpu_emu_reg::set_fip(qword dato){
	FIP = dato & 0x0000FFFFFFFFFFFFLL;
	return;
}

qword fpu_emu_reg::get_fop(void){
	return FOP & 0x0000FFFFFFFFFFFFLL;
}

void fpu_emu_reg::set_fop(qword dato){
	FOP = dato & 0x0000FFFFFFFFFFFFLL;
	return;
}
