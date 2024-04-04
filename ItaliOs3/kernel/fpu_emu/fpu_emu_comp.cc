#include <fpu_emu.h>
#include <cstddef>

using namespace myfloat;

void fpu_emu_comp::fpu_emu_comp_init(fpu_emu_reg * regi, fpu_emu_exception * exc){
	registri=regi;
	exce=exc;
	return;
}

void fpu_emu_comp::fcom(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	compare(registri->R[top],registri->R[((top+1)>7)?top+1-8:top+1],registri);
	return;
}

void fpu_emu_comp::fcom(unsigned int i){
	int top = (registri->get_sr() & 0x3800) >> 11;
	compare(registri->R[top],registri->R[((top+i)>7)?top+i-8:top+i],registri);
	return;
}

void fpu_emu_comp::fcom(float dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_double app;
	my_float app1;
	memcpy(&app1,&dato,sizeof(float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.exp=app1.strutturato.exp + fpu_const::BIAS_80 - fpu_const::BIAS_32;
	app.strutturato.number=(app1.strutturato.number << 8)|0x80000000;
	app.strutturato.number<<=32;
	compare(registri->R[top],app,registri);
	return;
}

void fpu_emu_comp::fcom(double dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_double app;
	my_sdouble app1;
	memcpy(&app1,&dato,sizeof(double));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.exp=app1.strutturato.exp + fpu_const::BIAS_80 - fpu_const::BIAS_64;
	app.strutturato.number=(app1.strutturato.number << 11)|0x8000000000000000LL;
	compare(registri->R[top],app,registri);
	return;
}

void fpu_emu_comp::ftst(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_double app;
	app.strutturato.sign=0;
	app.strutturato.exp=0;
	app.strutturato.number=0;
	compare(registri->R[top],app,registri);
	return;
}

void fpu_emu_comp::fcomp(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	fcom();
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
}

void fpu_emu_comp::fcomp(unsigned int i){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	fcom(i);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
}
void fpu_emu_comp::fcomp(float dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	fcom(dato);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
}
void fpu_emu_comp::fcomp(double dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	fcom(dato);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
}
void fpu_emu_comp::fcompp(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	fcom();
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.ffree(top+1);
	control.fincstp();
	control.fincstp();
}

void fpu_emu_comp::fxam(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int tag = (registri->get_tr() & (fpu_const::TR_EMPTY << (top*2))) >> (top*2);
	if (tag==fpu_const::TR_EMPTY) {
		registri->set_sr(registri->get_sr() & !(fpu_const::C0|fpu_const::C2|fpu_const::C3) | fpu_const::C0 | fpu_const::C3);
		return;  //empty
	}
	if (registri->R[top].strutturato.sign){
		registri->set_sr(registri->get_sr() | fpu_const::C1);
	}
	else {
		registri->set_sr(registri->get_sr() & !(fpu_const::C1));
	} //se il registro è pieno mette in C1 il segno del numero
	if ((registri->R[top].strutturato.exp==0) && (registri->R[top].strutturato.number==0)){
		registri->set_sr(registri->get_sr() & !(fpu_const::C0|fpu_const::C2|fpu_const::C3) | fpu_const::C3);
		return;  //zero
	}
	if ((registri->R[top].strutturato.exp==0) && (registri->R[top].strutturato.number<0x7FFFFFFFFFFFFFFFLL)){
		registri->set_sr(registri->get_sr() & !(fpu_const::C0|fpu_const::C2|fpu_const::C3) | fpu_const::C2 | fpu_const::C3);
		return;  //denormalizzati
	}
	if ((registri->R[top].strutturato.exp==0x7FFF) && (registri->R[top].strutturato.number==0x8000000000000000LL)){
		registri->set_sr(registri->get_sr() & !(fpu_const::C0|fpu_const::C2|fpu_const::C3) | fpu_const::C0 | fpu_const::C2);
		return;  //infinito
	}
	if (registri->R[top].strutturato.exp==0x7FFF){
		registri->set_sr(registri->get_sr() & !(fpu_const::C0|fpu_const::C2|fpu_const::C3) | fpu_const::C0);
		return;  //NaN
	}
	if ((registri->R[top].strutturato.exp>0) && (registri->R[top].strutturato.number<0x7FFFFFFFFFFFFFFFLL)){
		registri->set_sr(registri->get_sr() & !(fpu_const::C0|fpu_const::C2|fpu_const::C3));
		return;  //unsupported
	}
	registri->set_sr(registri->get_sr() & !(fpu_const::C0|fpu_const::C2|fpu_const::C3) | fpu_const::C2);
	return; //normali
}
