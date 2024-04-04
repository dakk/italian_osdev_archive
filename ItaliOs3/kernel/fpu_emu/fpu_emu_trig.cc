#include <fpu_emu.h>
#include <cstddef>
#include <cmath>

#ifndef TRIG_PRECISION
	#define TRIG_PRECISION 15
#endif

using namespace myfloat;
using namespace fpu_trig_value;

void fpu_emu_trig::fpu_emu_trig_init(fpu_emu_reg * regi, fpu_emu_exception * exc){
	registri=regi;
	exce=exc;
	//inizializzazione dei vettori per il calcolo del sin e cos
	memcpy(&VET_SIN[0],&VSIN0,sizeof(my_double));
	memcpy(&VET_SIN[1],&VSIN1,sizeof(my_double));
	memcpy(&VET_SIN[2],&VSIN2,sizeof(my_double));
	memcpy(&VET_SIN[3],&VSIN3,sizeof(my_double));
	memcpy(&VET_SIN[4],&VSIN4,sizeof(my_double));
	memcpy(&VET_SIN[5],&VSIN5,sizeof(my_double));
	memcpy(&VET_SIN[6],&VSIN6,sizeof(my_double));
	memcpy(&VET_SIN[7],&VSIN7,sizeof(my_double));
	memcpy(&VET_SIN[8],&VSIN8,sizeof(my_double));
	memcpy(&VET_SIN[9],&VSIN9,sizeof(my_double));
	memcpy(&VET_SIN[10],&VSIN10,sizeof(my_double));
	memcpy(&VET_SIN[11],&VSIN11,sizeof(my_double));
	memcpy(&VET_SIN[12],&VSIN12,sizeof(my_double));
	memcpy(&VET_SIN[13],&VSIN13,sizeof(my_double));
	memcpy(&VET_SIN[14],&VSIN14,sizeof(my_double));
	memcpy(&VET_SIN[15],&VSIN15,sizeof(my_double));
	memcpy(&VET_SIN[16],&VSIN16,sizeof(my_double));
	memcpy(&VET_SIN[17],&VSIN17,sizeof(my_double));
	memcpy(&VET_SIN[18],&VSIN18,sizeof(my_double));
	memcpy(&VET_SIN[19],&VSIN19,sizeof(my_double));
	memcpy(&VET_SIN[20],&VSIN20,sizeof(my_double));
	memcpy(&VET_SIN[21],&VSIN21,sizeof(my_double));
	memcpy(&VET_SIN[22],&VSIN22,sizeof(my_double));
	memcpy(&VET_SIN[23],&VSIN23,sizeof(my_double));

	memcpy(&VET_COS[0],&VCOS0,sizeof(my_double));
	memcpy(&VET_COS[1],&VCOS1,sizeof(my_double));
	memcpy(&VET_COS[2],&VCOS2,sizeof(my_double));
	memcpy(&VET_COS[3],&VCOS3,sizeof(my_double));
	memcpy(&VET_COS[4],&VCOS4,sizeof(my_double));
	memcpy(&VET_COS[5],&VCOS5,sizeof(my_double));
	memcpy(&VET_COS[6],&VCOS6,sizeof(my_double));
	memcpy(&VET_COS[7],&VCOS7,sizeof(my_double));
	memcpy(&VET_COS[8],&VCOS8,sizeof(my_double));
	memcpy(&VET_COS[9],&VCOS9,sizeof(my_double));
	memcpy(&VET_COS[10],&VCOS10,sizeof(my_double));
	memcpy(&VET_COS[11],&VCOS11,sizeof(my_double));
	memcpy(&VET_COS[12],&VCOS12,sizeof(my_double));
	memcpy(&VET_COS[13],&VCOS13,sizeof(my_double));
	memcpy(&VET_COS[14],&VCOS14,sizeof(my_double));
	memcpy(&VET_COS[15],&VCOS15,sizeof(my_double));
	memcpy(&VET_COS[16],&VCOS16,sizeof(my_double));
	memcpy(&VET_COS[17],&VCOS17,sizeof(my_double));
	memcpy(&VET_COS[18],&VCOS18,sizeof(my_double));
	memcpy(&VET_COS[19],&VCOS19,sizeof(my_double));
	memcpy(&VET_COS[20],&VCOS20,sizeof(my_double));
	memcpy(&VET_COS[21],&VCOS21,sizeof(my_double));
	memcpy(&VET_COS[22],&VCOS22,sizeof(my_double));
	memcpy(&VET_COS[23],&VCOS23,sizeof(my_double));

	memcpy(&NEAR[0],&VNEAR0,sizeof(my_double));
	memcpy(&NEAR[1],&VNEAR1,sizeof(my_double));
	memcpy(&NEAR[2],&VNEAR2,sizeof(my_double));
	memcpy(&NEAR[3],&VNEAR3,sizeof(my_double));
	memcpy(&NEAR[4],&VNEAR4,sizeof(my_double));
	memcpy(&NEAR[5],&VNEAR5,sizeof(my_double));
	memcpy(&NEAR[6],&VNEAR6,sizeof(my_double));
	memcpy(&NEAR[7],&VNEAR7,sizeof(my_double));
	memcpy(&NEAR[8],&VNEAR8,sizeof(my_double));
	memcpy(&NEAR[9],&VNEAR9,sizeof(my_double));
	memcpy(&NEAR[10],&VNEAR10,sizeof(my_double));
	memcpy(&NEAR[11],&VNEAR11,sizeof(my_double));
	memcpy(&NEAR[12],&VNEAR12,sizeof(my_double));
	memcpy(&NEAR[13],&VNEAR13,sizeof(my_double));
	memcpy(&NEAR[14],&VNEAR14,sizeof(my_double));
	memcpy(&NEAR[15],&VNEAR15,sizeof(my_double));
	memcpy(&NEAR[16],&VNEAR16,sizeof(my_double));
	memcpy(&NEAR[17],&VNEAR17,sizeof(my_double));
	memcpy(&NEAR[18],&VNEAR18,sizeof(my_double));
	memcpy(&NEAR[19],&VNEAR19,sizeof(my_double));
	memcpy(&NEAR[20],&VNEAR20,sizeof(my_double));
	memcpy(&NEAR[21],&VNEAR21,sizeof(my_double));
	memcpy(&NEAR[22],&VNEAR22,sizeof(my_double));
	memcpy(&NEAR[23],&VNEAR23,sizeof(my_double));

	memcpy(&VETT[0],&VVETT0,sizeof(my_double));
	memcpy(&VETT[1],&VVETT1,sizeof(my_double));
	memcpy(&VETT[2],&VVETT2,sizeof(my_double));
	memcpy(&VETT[3],&VVETT3,sizeof(my_double));
	memcpy(&VETT[4],&VVETT4,sizeof(my_double));
	memcpy(&VETT[5],&VVETT5,sizeof(my_double));
	memcpy(&VETT[6],&VVETT6,sizeof(my_double));
	memcpy(&VETT[7],&VVETT7,sizeof(my_double));
	memcpy(&VETT[8],&VVETT8,sizeof(my_double));
	memcpy(&VETT[9],&VVETT9,sizeof(my_double));
	memcpy(&VETT[10],&VVETT10,sizeof(my_double));
	memcpy(&VETT[11],&VVETT11,sizeof(my_double));
	memcpy(&VETT[12],&VVETT12,sizeof(my_double));
	memcpy(&VETT[13],&VVETT13,sizeof(my_double));
	memcpy(&VETT[14],&VVETT14,sizeof(my_double));
	memcpy(&VETT[15],&VVETT15,sizeof(my_double));
	memcpy(&VETT[16],&VVETT16,sizeof(my_double));
	memcpy(&VETT[17],&VVETT17,sizeof(my_double));
	memcpy(&VETT[18],&VVETT18,sizeof(my_double));
	memcpy(&VETT[19],&VVETT19,sizeof(my_double));
	memcpy(&VETT[20],&VVETT20,sizeof(my_double));
	memcpy(&VETT[21],&VVETT21,sizeof(my_double));
	memcpy(&VETT[22],&VVETT22,sizeof(my_double));
	memcpy(&VETT[23],&VVETT23,sizeof(my_double));
	return;
}

long int fpu_emu_trig::fattoriale(long int numero){
	if ((numero==1)||(numero==0)) return 1;
	return (numero*fattoriale(numero-1));
}

my_double fpu_emu_trig::converti(int valore){
	my_double reg;
	int i=30;
	if (valore<0) {
		reg.strutturato.sign=1;
		valore*=-1;
	}
	else reg.strutturato.sign=0;
	while ((i>=0) && !(valore & (1<<i))){
		--i;
	}
	reg.strutturato.number=(unsigned long long)valore << (63-i);
	reg.strutturato.exp=i+fpu_const::BIAS_80;
	return reg;
}

int fpu_emu_trig::get_nearest(void){
	int top=(registri->get_sr() & 0x3800) >> 11;
	int sr_default = registri->get_sr();
	int i;
	for (i=0; i<24; i++){
		compare(VETT[i],registri->R[top],registri);
		if (registri->get_sr()) break;;
	}
	registri->set_sr(sr_default);
	return i;
}

void fpu_emu_trig::fsin(void){
	int top=(registri->get_sr() & 0x3800) >> 11;
	int counter=0;
	int i;
	//int fatt1,fatt2;
	int value = get_nearest();
	long long int app1;
	my_double app,res,a,b,c,d,f,s/*,fatt1dbl,fatt2dbl*/;
	//in app metto 2^63
	app1 = pow(2,63);
	app = converti(app1);
	res.strutturato.sign=0;
	res.strutturato.exp=0;
	res.strutturato.number=0;
	//ora devo controllare che il numero sia compreso tra
	//2^63 e -2^63 se così non è setto c2 a 1;
	compare(registri->R[top],app,registri);
	if (!(registri->get_sr() & fpu_const::CO23)){
		//setto c2
		registri->set_sr(registri->get_sr() | fpu_const::C2);
		return;
	}
	app.strutturato.sign=1;
	compare(registri->R[top],app,registri);
	if ((registri->get_sr() & fpu_const::CO23)==fpu_const::C0){
		//setto c2
		registri->set_sr(registri->get_sr() | fpu_const::C2);
		return;
	}
	//pulisco c2
	registri->set_sr((registri->get_sr() & (~fpu_const::C2)));
	//qui viene calcolato il seno utilizzo TRIG_PRECISION
	//per il numero di elementi da utilizzare nella serie
	for (counter=0;counter<TRIG_PRECISION;counter++){
		//kout << "\n counter: " << counter;
		app = NEAR[value];
		app.strutturato.sign^=1;
		s = somma(registri->R[top],app);
		a = b = c = d = s;
		for (i=1;i<(counter*4);i++){
			a=moltiplica(a,s);
		}
		f = converti(fattoriale(counter*4));
		a = dividi(a,f);
		a = moltiplica(a,VET_SIN[value]);

		for (i=1;i<((counter*4)+1);i++){
			b=moltiplica(b,s);
		}
		f = converti(fattoriale((counter * 4) + 1));
		b = dividi(b,f);
		b = moltiplica(b,VET_COS[value]);

		for (i=1;i<((counter*4)+2);i++){
			c=moltiplica(c,s);
		}
		f = converti(fattoriale((counter * 4) + 2));
		c = dividi(c,f);
		c = moltiplica(c,VET_SIN[value]);

		for (i=1;i<((counter*4)+3);i++){
			d=moltiplica(d,s);
		}
		f = converti(fattoriale((counter * 4) + 3));
		d = dividi(d,f);
		d = moltiplica(d,VET_COS[value]);
		s = somma (a,b);
		c.strutturato.sign^=1;
		d.strutturato.sign^=1;
		s = somma(s,c);
		s = somma(s,d);
		res = somma(res,s);
	}
	registri->R[top]=res;
	return;
}

void fpu_emu_trig::fcos(void){
	int top=(registri->get_sr() & 0x3800) >> 11;
	int counter=0;
	int i;
	//int fatt1,fatt2;
	int value = get_nearest();
	long long int app1;
	my_double app,res,a,b,c,d,f,s/*,fatt1dbl,fatt2dbl*/;
	//in app metto 2^63
	app1 = pow(2,63);
	app = converti(app1);
	res.strutturato.sign=0;
	res.strutturato.exp=0;
	res.strutturato.number=0;
	//ora devo controllare che il numero sia compreso tra
	//2^63 e -2^63 se così non è setto c2 a 1;
	compare(registri->R[top],app,registri);
	if (!(registri->get_sr() & fpu_const::CO23)){
		//setto c2
		registri->set_sr(registri->get_sr() | fpu_const::C2);
		return;
	}
	app.strutturato.sign=1;
	compare(registri->R[top],app,registri);
	if ((registri->get_sr() & fpu_const::CO23)==fpu_const::C0){
		//setto c2
		registri->set_sr(registri->get_sr() | fpu_const::C2);
		return;
	}
	//pulisco c2
	registri->set_sr((registri->get_sr() & (~fpu_const::C2)));
	//qui viene calcolato il seno utilizzo TRIG_PRECISION
	//per il numero di elementi da utilizzare nella serie
	for (counter=0;counter<TRIG_PRECISION;counter++){
		//kout << "\n counter: " << counter;
		app = NEAR[value];
		app.strutturato.sign^=1;
		s = somma(registri->R[top],app);
		a = b = c = d = s;
		for (i=1;i<(counter*4);i++){
			a=moltiplica(a,s);
		}
		f = converti(fattoriale(counter*4));
		a = dividi(a,f);
		a = moltiplica(a,VET_COS[value]);

		for (i=1;i<((counter*4)+1);i++){
			b=moltiplica(b,s);
		}
		f = converti(fattoriale((counter * 4) + 1));
		b = dividi(b,f);
		b = moltiplica(b,VET_SIN[value]);

		for (i=1;i<((counter*4)+2);i++){
			c=moltiplica(c,s);
		}
		f = converti(fattoriale((counter * 4) + 2));
		c = dividi(c,f);
		c = moltiplica(c,VET_COS[value]);

		for (i=1;i<((counter*4)+3);i++){
			d=moltiplica(d,s);
		}
		f = converti(fattoriale((counter * 4) + 3));
		d = dividi(d,f);
		d = moltiplica(d,VET_SIN[value]);
		b.strutturato.sign^=1;
		s = somma (a,b);
		d.strutturato.sign^=1;
		s = somma(s,c);
		s = somma(s,d);
		res = somma(res,s);
	}
	registri->R[top]=res;
	return;
}

void fpu_emu_trig::fsincos(void){
	fsin();
	fcos();
	return;
}
