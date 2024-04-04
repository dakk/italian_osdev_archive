#include <fpu_emu.h>
#include <cstddef>
#include <cstring>
#include <cmath>

using namespace myfloat;

void fpu_emu_data::fld(float valore){
  int top = (registri->get_sr() & 0x3800) >> 11;
  my_float app;
  if (top==0){
    registri->set_sr(registri->get_sr()|fpu_const::UF);
    exce->add(IS);
    return;
  }
  if ((registri->get_tr() & (fpu_const::TR_EMPTY << (2*top)))!= (fpu_const::TR_EMPTY << (2*(top))))
  {
    top--;
    registri->set_sr((registri->get_sr() & ~0x3800) | (top << 11));
  }
  memcpy(&app,&valore,sizeof(float));
  registri->R[top].strutturato.sign = app.strutturato.sign;
  registri->R[top].strutturato.exp = app.strutturato.exp + fpu_const::BIAS_80 - fpu_const::BIAS_32;
  registri->R[top].strutturato.number = (unsigned long long)(app.strutturato.number << 8);
  registri->R[top].strutturato.number|=0x80000000;
  registri->R[top].strutturato.number<<=32;
  //da mettere a pieno il registro tr
  registri->set_tr(registri->get_tr() & (fpu_const::TR_VAL<<(2*top)));
  return;
}

void fpu_emu_data::fld(double valore){
  int top = (registri->get_sr() & 0x3800) >> 11;
  my_sdouble app;
  if (top==0){
    registri->set_sr(registri->get_sr()|fpu_const::UF);
    exce->add(IS);
    return;
  }
  if ((registri->get_tr() & (fpu_const::TR_EMPTY << (2*top)))!= (fpu_const::TR_EMPTY << (2*(top))) && (top != 0))
  {
    top--;
    registri->set_sr((registri->get_sr() & ~0x3800) | (top << 11));
  };
  memcpy(&app,&valore,sizeof(double));
  registri->R[top].strutturato.sign = app.strutturato.sign;
  registri->R[top].strutturato.exp = app.strutturato.exp + fpu_const::BIAS_80 - fpu_const::BIAS_64;
  registri->R[top].strutturato.number = (app.strutturato.number << 11) | 0x8000000000000000LL ;
  //da mettere a pieno il registro tr
  registri->set_tr(registri->get_tr() & (fpu_const::TR_VAL<<(2*top)));
  return;
}

void fpu_emu_data::fld(long double valore){
  int top = (registri->get_sr() & 0x3800) >> 11;
  if (top==0){
    registri->set_sr(registri->get_sr()|fpu_const::UF);
    exce->add(IS);
    return;
  }
  if ((registri->get_tr() & (fpu_const::TR_EMPTY << (2*top)))!= (fpu_const::TR_EMPTY << (2*(top))) && (top != 0))
  {
    top--;
    registri->set_sr((registri->get_sr() & ~0x3800) | (top << 11));
  };
  memcpy(&registri->R[top],&valore,sizeof(long double));
  //da mettere a pieno il registro tr
  registri->set_tr(registri->get_tr() & (fpu_const::TR_VAL<<(2*top)));
  return;
}


void print_bin(unsigned long long d)
{
	char tmp[70];
	int i;
	i = 0;
	do {
		tmp[i++] = '0' + (d & 1);
		d >>= 1;
	}
	while (d > 0);
	for (i--; i >= 0; i--) {
		kout << tmp[i];
	}
}

void fpu_emu_data::fild(short int valore){
  int top = (registri->get_sr() & 0x3800) >> 11;
  int i=14;
  if (top==0){
    registri->set_sr(registri->get_sr()|fpu_const::UF);
    exce->add(IS);
    return;
  }
  if ((registri->get_tr() & (fpu_const::TR_EMPTY << (2*top)))!= (fpu_const::TR_EMPTY << (2*(top))) && (top != 0))
  {
    top--;
    registri->set_sr((registri->get_sr() & ~0x3800) | (top << 11));
  };
  //conversione del numero da intero a float
    //identificazione del segno
  if (valore<0) {
	registri->R[top].strutturato.sign=1;
	valore*=-1;
  }
  else registri->R[top].strutturato.sign=0;
  while ((i>=0) && !(valore & (1<<i))){
  	--i;
  }
  registri->R[top].strutturato.number=(unsigned long long)valore << (63-i);
  registri->R[top].strutturato.exp=i+fpu_const::BIAS_80;
  //da mettere a pieno il registro tr
  registri->set_tr(registri->get_tr() & (fpu_const::TR_VAL<<(2*top)));
  return;
}

void fpu_emu_data::fild(int valore){
  int top = (registri->get_sr() & 0x3800) >> 11;
  int i=30;
  if (top==0){
    registri->set_sr(registri->get_sr()|fpu_const::UF);
    exce->add(IS);
    return;
  }
  if ((registri->get_tr() & (fpu_const::TR_EMPTY << (2*top)))!= (fpu_const::TR_EMPTY << (2*(top))) && (top != 0))
  {
    top--;
    registri->set_sr((registri->get_sr() & ~0x3800) | (top << 11));
  };
  //conversione del numero da intero a float
    //identificazione del segno
  if (valore<0) {
	registri->R[top].strutturato.sign=1;
	valore*=-1;
  }
  else registri->R[top].strutturato.sign=0;
  while ((i>=0) && !(valore & (1<<i))){
  	--i;
  }
  registri->R[top].strutturato.number=(unsigned long long)valore << (63-i);
  registri->R[top].strutturato.exp=i+fpu_const::BIAS_80;
  //da mettere a pieno il registro tr
  registri->set_tr(registri->get_tr() & (fpu_const::TR_VAL<<(2*top)));
  return;
}

void fpu_emu_data::fild(long long valore){
  int top = (registri->get_sr() & 0x3800) >> 11;
  int i=62;
  if (top==0){
    registri->set_sr(registri->get_sr()|fpu_const::UF);
    exce->add(IS);
    return;
  }
  if ((registri->get_tr() & (fpu_const::TR_EMPTY << (2*top)))!= (fpu_const::TR_EMPTY << (2*(top))) && (top != 0))
  {
    top--;
    registri->set_sr((registri->get_sr() & ~0x3800) | (top << 11));
  };
  //conversione del numero da intero a float
    //identificazione del segno
  if (valore<0) {
	registri->R[top].strutturato.sign=1;
	valore*=-1;
  }
  else registri->R[top].strutturato.sign=0;
  while ((i>=0) && !(valore & pow(2,i))){
  	--i;
  }
  registri->R[top].strutturato.number=(unsigned long long)valore << (63-i);
  registri->R[top].strutturato.exp=i+fpu_const::BIAS_80;
  //da mettere a pieno il registro tr
  registri->set_tr(registri->get_tr() & (fpu_const::TR_VAL<<(2*top)));
  return;
}

void fpu_emu_data::fxch(void){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_double temp = registri->R[top];
	registri->R[top]=registri->R[top+1];
	registri->R[top+1]=temp;
	return;
}

void fpu_emu_data::fxch(unsigned short int num){
	int top = (registri->get_sr() & 0x3800) >> 11;
	my_double temp = registri->R[top];
	registri->R[top]=registri->R[top+num];
	registri->R[top+num]=temp;
	return;
}

void fpu_emu_data::fpu_emu_data_init(fpu_emu_reg * regi, fpu_emu_exception * exc){
	registri=regi;
	exce=exc;
	return;
}

void fpu_emu_data::fst(long double * dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	memcpy(dato,&registri->R[top],sizeof(long double));
	return;
}

void fpu_emu_data::fstp(long double * dato){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	control.fpu_emu_control_init(registri,exce);
	fst(dato);
	control.ffree(top);
	top++;
	if (top==8){
		registri->set_sr(registri->get_sr() | fpu_const::OF);
		exce->add(IS);
		top=0;
	}
	registri->set_sr((registri->get_sr() & ~0x3800) | (top << 11));
	return;
}

void fpu_emu_data::fst(unsigned int dest){
	int top = (registri->get_sr() & 0x3800) >> 11;
	if ((top+dest)>7){
		registri->R[top+dest-8].strutturato= registri->R[top].strutturato;
		registri->set_tr(registri->get_tr() & !(fpu_const::TR_VAL << 2*(top+dest-8)));
	}
	else{
		registri->R[top+dest].strutturato= registri->R[top].strutturato;
		registri->set_tr(registri->get_tr() & !(fpu_const::TR_VAL << 2*(top+dest)));
	}
	return;
}

void fpu_emu_data::fstp(unsigned int dest){
	int top = (registri->get_sr() & 0x3800) >> 11;
	fpu_emu_control control;
	control.fpu_emu_control_init(registri,exce);
	fst(dest);
	control.ffree(top);
	top++;
	if (top==8){
		registri->set_sr(registri->get_sr() | fpu_const::OF);
		exce->add(IS);
	}
	registri->set_sr((registri->get_sr() & ~0x3800) | (top << 11));
	return;
}
