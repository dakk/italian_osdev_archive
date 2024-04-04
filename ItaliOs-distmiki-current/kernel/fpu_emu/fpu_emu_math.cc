#include <fpu_emu.h>
#include <cstring>
#include <cstddef>
#include <cmath>

using namespace myfloat;

void fpu_emu_math::fabs(void){
	word i = (registri->get_sr() & fpu_const::INIT_SR)>>11;
	registri->R[i].strutturato.sign &= 0;
	return;
}

void fpu_emu_math::fchs(void){
	word i = (registri->get_sr() & fpu_const::INIT_SR)>>11;
	registri->R[i].strutturato.sign = ~registri->R[i].strutturato.sign;
	return;
}

//add
void fpu_emu_math::fadd(float dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_float app1;
	my_double app;
	memcpy(&app1,&dato,sizeof(my_float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.number=(app1.strutturato.number << 8) | 0x80000000;
	app.strutturato.number<<=32;
	app.strutturato.exp=app.strutturato.exp+fpu_const::BIAS_80-fpu_const::BIAS_32;
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fadd(double dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_sdouble app1;
	my_double app;
	memcpy(&app1,&dato,sizeof(my_float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.number=(app1.strutturato.number<<11)|0x8000000000000000LL;
	app.strutturato.exp=app.strutturato.exp+fpu_const::BIAS_80-fpu_const::BIAS_64;
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fadd(short int i, short int par_res){
	int top = (registri->get_sr() & 0x3800) >> 11;
	registri->R[(!par_res)?top:(((top+i)>7?(top+i-7):top+i))]=somma(registri->R[top],registri->R[((top+i)>7)?(top+i-7):top+i]);
	return;
}

void fpu_emu_math::faddp(unsigned short int i){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[(top+i)>7?top+i-7:top+i]=somma(registri->R[top],registri->R[(top+i)>7?top+i-7:top+i]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::faddp(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[(top+1)>7?top+1-7:top+1]=somma(registri->R[top],registri->R[(top+1)>7?top+1-7:top+1]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fiadd(short int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=14;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=1;
		valore*=-1;
	}
  	else app.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fiadd(int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=30;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=1;
		valore*=-1;
	}
  	else app.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	registri->R[top]=somma(registri->R[top],app);
	return;
}


//sub
void fpu_emu_math::fsub(float dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_float app1;
	my_double app;
	memcpy(&app1,&dato,sizeof(my_float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.number=(app1.strutturato.number<<8) | 0x80000000;
	app.strutturato.number<<=32;
	app.strutturato.exp=app.strutturato.exp+fpu_const::BIAS_80-fpu_const::BIAS_32;
	app.strutturato.sign^=1;
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fsub(double dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_sdouble app1;
	my_double app;
	memcpy(&app1,&dato,sizeof(my_float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.number=(app1.strutturato.number<<11)|0x8000000000000000LL;
	app.strutturato.exp=app.strutturato.exp+fpu_const::BIAS_80-fpu_const::BIAS_64;
	app.strutturato.sign^=1;
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fsub(short int i, short int par_res){
	int top = (registri->get_sr() & 0x3800) >> 11;
	registri->R[(par_res)?top:(((top+i)>7)?top+i-7:top+i)].strutturato.sign^=1;
	registri->R[(!par_res)?top:(((top+i)>7?(top+i-7):top+i))]=somma(registri->R[top],registri->R[((top+i)>7)?(top+i-7):top+i]);
	return;
}

void fpu_emu_math::fsubp(unsigned short int i){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	fchs();
	registri->R[(top+i)>7?top+i-7:top+i]=somma(registri->R[top],registri->R[(top+i)>7?top+i-7:top+i]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fisub(short int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=14;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=0;
		valore*=-1;
	}
  	else app.strutturato.sign=1;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fisub(int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=30;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=0;
		valore*=-1;
	}
  	else app.strutturato.sign=1;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	registri->R[top]=somma(registri->R[top],app);
	return;
}

//sub reverse
void fpu_emu_math::fsubr(float dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_float app1;
	my_double app;
	memcpy(&app1,&dato,sizeof(my_float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.number=(app1.strutturato.number<<8)|0x80000000;
	app.strutturato.number<<=32;
	app.strutturato.exp=app.strutturato.exp+fpu_const::BIAS_80-fpu_const::BIAS_32;
	fchs();
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fsubr(double dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_sdouble app1;
	my_double app;
	memcpy(&app1,&dato,sizeof(my_float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.number=(app1.strutturato.number<<11)|0x8000000000000000LL;
	app.strutturato.exp=app.strutturato.exp+fpu_const::BIAS_80-fpu_const::BIAS_64;
	fchs();
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fsubr(short int i, short int par_res){
	int top = (registri->get_sr() & 0x3800) >> 11;
	registri->R[(!par_res)?top:(((top+i)>7)?top+i-7:top+i)].strutturato.sign^=1;
	registri->R[(!par_res)?top:(((top+i)>7?(top+i-7):top+i))]=somma(registri->R[top],registri->R[((top+i)>7)?(top+i-7):top+i]);
	return;
}

void fpu_emu_math::fsubrp(unsigned short int i){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[((top+i)>7)?top+i-7:top+i].strutturato.sign^=1;
	registri->R[(top+i)>7?top+i-7:top+i]=somma(registri->R[top],registri->R[(top+i)>7?top+i-7:top+i]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fsubrp(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[((top+1)>7)?top+1-7:top+1].strutturato.sign^=1;
	registri->R[(top+1)>7?top+1-7:top+1]=somma(registri->R[top],registri->R[(top+1)>7?top+1-7:top+1]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fisubr(short int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=14;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=1;
		valore*=-1;
	}
  	else app.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	fchs();
	registri->R[top]=somma(registri->R[top],app);
	return;
}

void fpu_emu_math::fisubr(int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=30;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=1;
		valore*=-1;
	}
  	else app.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	fchs();
	registri->R[top]=somma(registri->R[top],app);
	return;
}

//mul
void fpu_emu_math::fmul(float dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_float app1;
	my_double app;
	memcpy(&app1,&dato,sizeof(my_float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.number=(app1.strutturato.number<<8)|0x80000000;
	app.strutturato.number<<=32;
	app.strutturato.exp=app.strutturato.exp+fpu_const::BIAS_80-fpu_const::BIAS_32;
	registri->R[top]=moltiplica(registri->R[top],app);
}

void fpu_emu_math::fmul(double dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_sdouble app1;
	my_double app;
	memcpy(&app1,&dato,sizeof(my_double));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.number=(app1.strutturato.number<<11)|0x8000000000000000LL;
	app.strutturato.exp=app.strutturato.exp+fpu_const::BIAS_80-fpu_const::BIAS_64;
	registri->R[top]=moltiplica(registri->R[top],app);
	return;
}

void fpu_emu_math::fmul(short int i, short int par_res){
	int top = (registri->get_sr() & 0x3800) >> 11;
	registri->R[(!par_res)?top:(((top+i)>7?(top+i-7):top+i))]=moltiplica(registri->R[top],registri->R[((top+i)>7)?(top+i-7):top+i]);
	return;
}

void fpu_emu_math::fmulp(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[(top+1)>7?top+1-7:top+1]=moltiplica(registri->R[top],registri->R[(top+1)>7?top+1-7:top+1]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fmulp(short int i){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[(top+i)>7?top+i-7:top+i]=moltiplica(registri->R[top],registri->R[(top+i)>7?top+i-7:top+i]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fimul(int valore){
	unsigned int top = (registri->get_sr() & 0x3800) >> 11;
	int i;
	my_double app;
	if (valore<0) {
		fchs();
		valore *= -1;
	}
	if (valore == 0) {
		registri->R[top].numero=0;
		return;
	}
	app.numero=0;
	for (i=1; i<valore; i++){
		//eseguo la somma tra il st(0) e (i-1)st(0);
		app=somma(app,registri->R[top]);
	}
	registri->R[top]=app;
	return;
}

void fpu_emu_math::fimul(short int valore){
	unsigned int top = (registri->get_sr() & 0x3800) >> 11;
	int i;
	my_double app;
	if (valore<0) {
		fchs();
		valore *= -1;
	}
	if (valore == 0) {
		registri->R[top].numero=0;
		return;
	}
	app.numero=0;
	for (i=1; i<valore; i++){
		//eseguo la somma tra il st(0) e (i-1)st(0);
		app=somma(app,registri->R[top]);
	}
	registri->R[top]=app;
	return;
}

//divisioni
void fpu_emu_math::fdiv(float dato){
	int top = (registri->get_sr() & 0x3800)>>11;
	my_double app;
	my_float app1;
	memcpy(&app1,&dato,sizeof(float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.exp=app1.strutturato.exp + fpu_const::BIAS_80 - fpu_const::BIAS_32;
	app.strutturato.number=(app1.strutturato.number << 8)|0x80000000;
	app.strutturato.number<<=32;
	registri->R[top]=dividi(registri->R[top],app);
	return;
}

void fpu_emu_math::fdiv(double dato){
	int top = (registri->get_sr() & 0x3800)>>11;
	my_double app;
	my_sdouble app1;
	memcpy(&app1,&dato,sizeof(double));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.exp=app1.strutturato.exp + fpu_const::BIAS_80 - fpu_const::BIAS_64;
	app.strutturato.number=(app1.strutturato.number << 11)|0x8000000000000000LL;
	registri->R[top]=dividi(registri->R[top],app);
	return;
}

void fpu_emu_math::fdiv(short int i, short int par_res){
	int top = (registri->get_sr() & 0x3800) >> 11;
	registri->R[(!par_res)?top:(((top+i)>7?(top+i-7):top+i))]=dividi(registri->R[(!par_res)?top:(((top+i)>7?(top+i-7):top+i))],registri->R[par_res?top:(((top+i)>7?(top+i-7):top+i))]);
	return;
}

void fpu_emu_math::fdivp(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[(top+1)>7?top+1-7:top+1]=dividi(registri->R[(top+1)>7?top+1-7:top+1],registri->R[top]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fdivp(unsigned short int i){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[(top+i)>7?top+i-7:top+i]=dividi(registri->R[(top+i)>7?top+i-7:top+i],registri->R[top]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fidiv(short int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=14;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=1;
		valore*=-1;
	}
  	else app.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	registri->R[top]=dividi(registri->R[top],app);
	return;
}

void fpu_emu_math::fidiv(int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=30;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=1;
		valore*=-1;
	}
  	else app.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	registri->R[top]=dividi(registri->R[top],app);
	return;
}

//div reverse
void fpu_emu_math::fdivr(float dato){
	int top = (registri->get_sr() & 0x3800)>>11;
	my_double app;
	my_float app1;
	memcpy(&app1,&dato,sizeof(float));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.exp=app1.strutturato.exp + fpu_const::BIAS_80 - fpu_const::BIAS_32;
	app.strutturato.number=(app1.strutturato.number << 8)|0x80000000;
	app.strutturato.number<<=32;
	registri->R[top]=dividi(app,registri->R[top]);
	return;
}

void fpu_emu_math::fdivr(double dato){
	int top = (registri->get_sr() & 0x3800)>>11;
	my_double app;
	my_sdouble app1;
	memcpy(&app1,&dato,sizeof(double));
	app.strutturato.sign=app1.strutturato.sign;
	app.strutturato.exp=app1.strutturato.exp + fpu_const::BIAS_80 - fpu_const::BIAS_64;
	app.strutturato.number=(app1.strutturato.number << 11)|0x8000000000000000LL;
	registri->R[top]=dividi(app,registri->R[top]);
	return;
}
 
void fpu_emu_math::fdivr(short int i, short int par_res){
	int top = (registri->get_sr() & 0x3800) >> 11;
	registri->R[(!par_res)?top:(((top+i)>7?(top+i-7):top+i))]=dividi(registri->R[par_res?top:(((top+i)>7?(top+i-7):top+i))],registri->R[(!par_res)?top:(((top+i)>7?(top+i-7):top+i))]);
	return;
}

void fpu_emu_math::fdivrp(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[(top+1)>7?top+1-7:top+1]=dividi(registri->R[top],registri->R[(top+1)>7?top+1-7:top+1]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fdivrp(unsigned short int i){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	registri->R[(top+i)>7?top+i-7:top+i]=dividi(registri->R[top],registri->R[(top+i)>7?top+i-7:top+i]);
	control.fpu_emu_control_init(registri,exce);
	control.ffree(top);
	control.fincstp();
	return;
}

void fpu_emu_math::fidivr(short int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=14;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=1;
		valore*=-1;
	}
  	else app.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	registri->R[top]=dividi(app,registri->R[top]);
	return;
}

void fpu_emu_math::fidivr(int valore){
	int top = (registri->get_sr() & 0x3800) >> 11;
	int i=30;
	my_double app;
	if (valore<0) {
		app.strutturato.sign=1;
		valore*=-1;
	}
  	else app.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	app.strutturato.number=(unsigned long long)valore << (63-i);
	app.strutturato.exp=i+fpu_const::BIAS_80;
	registri->R[top]=dividi(app,registri->R[top]);
	return;
}


/*-------------------------------------------------------------*/
//funzioni di inizializzazione della classe
void fpu_emu_math::fpu_emu_math_init(fpu_emu_reg * regi, fpu_emu_exception * exc){
	registri=regi;
	exce=exc;
	return;
}
