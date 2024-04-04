#include <fpu_emu.h>
#include <fpu_emu_demo.h>

volatile double pippo;

void fpu_demo(){
	fpu_emu num;
	if(check_fpu())
		kout << "La fpu è presente\n";
	else
		kout << "La fpu non è presente\n";
	pippo = 0.5;
	kout<< "carico i valori\n";
	num.data.fld(1.570796327);
	//num.data.fld(0.0);
	kout << "ho caricato i valori\n";
	num.trig.fsin();
	//num.data.fld(64.550);
	//num.data.fld(4.410);
	//num.math.faddp();
	kout << "\nIl risultato è: ";
	kout << num.regi.R[((num.regi.get_sr() & 0x3800) >> 11)].numero<<'\n';

}
