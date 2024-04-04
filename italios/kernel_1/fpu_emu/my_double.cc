#include <fpu_emu.h>
#include <cstddef>
#include <cmath>

using namespace myfloat;

//funzione che esegue somma e sottrazioni
my_double myfloat::somma(my_double uno, my_double due){
	my_double ret;
	int diff;
	int i;
	int128 primo, secondo, result;
	diff=uno.strutturato.exp-due.strutturato.exp;
	//se un operando è molto maggiore dell'altro restituisco immediatamente quello
	if ((diff > 64) || (diff < -64)) {
		ret.strutturato=(diff > 0)?uno.strutturato:due.strutturato;
		return ret;
	}
	//se la differenza è compresa tra -64 e + 64 devo shiftare
	//allo stesso grado i due numeri in due variabili a 128 bit
	if (!(uno.strutturato.sign ^ due.strutturato.sign)){
		ret.strutturato.sign = due.strutturato.sign;
		//devo eseguire una somma a 128
		if (diff > 0){
			ret.strutturato.exp = due.strutturato.exp;
			secondo.basso = due.strutturato.number;
			secondo.alto = 0;
			primo.basso = uno.strutturato.number << diff;
			primo.alto = uno.strutturato.number >> (64 - diff);
		}
		else {
			ret.strutturato.exp = uno.strutturato.exp;
			primo.basso = uno.strutturato.number;
			primo.alto = 0;
			secondo.basso = due.strutturato.number << (-diff);
			secondo.alto = due.strutturato.number >> (64 + diff);
		}
		result = somma128(primo, secondo);
	}
	else {
		//devo eseguire una differenza
		if (diff > 0){
			ret.strutturato.sign = uno.strutturato.sign;
			ret.strutturato.exp = due.strutturato.exp;
			secondo.basso = due.strutturato.number;
			secondo.alto = 0;
			primo.basso = uno.strutturato.number << diff;
			primo.alto = uno.strutturato.number >> (64 - diff);
			result = sottrai128(primo,secondo);
		}
		else {
			ret.strutturato.sign = due.strutturato.sign;
			ret.strutturato.exp = uno.strutturato.exp;
			primo.basso = uno.strutturato.number;
			primo.alto = 0;
			secondo.basso = due.strutturato.number << (-diff);
			secondo.alto = due.strutturato.number >> (64 + diff);
			result = sottrai128(secondo,primo);
		}
	}
	i=63;
	while (((i>=0) && ((result.alto>>i)!=1))){
			i--;
	}
	ret.strutturato.number=(result.basso>>(i+1))|(result.alto<<(63-i));
	ret.strutturato.exp+=i+1;
	return ret;
}

//funzione che esegue le moltiplicazioni
my_double myfloat::moltiplica(my_double uno, my_double due){
	int i=0;
	int128 app[64];
	int128 ris;
	my_double ret;
	ret.strutturato.sign=uno.strutturato.sign^due.strutturato.sign;
        ret.strutturato.exp=uno.strutturato.exp+due.strutturato.exp-fpu_const::BIAS_80;
	for (i=0;i<64;i++){
		app[i].basso=0;
		app[i].alto=0;
	}
	for (i=0;i<64;i++){
		if (due.strutturato.number & pow(2,i)){
			//split
			app[i].alto=((uno.strutturato.number>>(64-i))& 0xFFFFFFFFFFFFFFFFLL);
			app[i].basso=uno.strutturato.number<<i;
		}
		else {
			app[i].alto=0;
			app[i].basso=0;
		}
	}
	ris.alto=0;
	ris.basso=0;
	for (i=0;i<64;i++)
		ris=somma128(ris,app[i]);
	i=63;
	while (((i>=0) && ((ris.alto>>i)!=1))){
			i--;
	}
	ret.strutturato.number=(ris.basso>>(i+1))|(ris.alto<<(63-i));
	ret.strutturato.exp+=(64-i)/i;
	return ret;
}

//funzione che esegue le divisioni
my_double myfloat::dividi(my_double uno, my_double due){
	my_double ret;
	unsigned int i;
	int128 primo, secondo;
	secondo.basso=due.strutturato.number;
	secondo.alto=0;
	ret.strutturato.number=0;
	ret.strutturato.sign=uno.strutturato.sign^due.strutturato.sign;
	ret.strutturato.exp=uno.strutturato.exp+fpu_const::BIAS_80-due.strutturato.exp;
	primo.basso=uno.strutturato.number;
	primo.alto=0;
	//algoritmo per la divisione
	//qui eseguo una divisione in colonna;
	ret.strutturato.number=0;
	for (i=64;i>0;--i){
		//qui vanno fatti i calcoli per determinare
		//se nel bit i-esimo del risultato va messo
		//0 oppure 1
		if (confronta128(primo,secondo)){
			ret.strutturato.number+=pow(2,(i-1));
			primo=sottrai128(primo,secondo);
		}
		primo.alto=primo.basso>>63;
		primo.basso<<=1;
	}
	i=63;
	while (((i>=0) && ((ret.strutturato.number>>i)!=1))){
			i--;
	}
	ret.strutturato.number<<=(63-i);
	ret.strutturato.exp-=(64-i)/i;
	return ret;
}

void myfloat::compare(my_double a, my_double b, fpu_emu_reg* registri){
	short int res=2;
	int diff;
	//qui vanno eseguiti i test per determinare
	if (!(a.strutturato.sign ^ b.strutturato.sign)){
	//se il segno è diverso basta controllare il segno di uno dei due per determinare il maggiore
		res=(a.strutturato.sign)?1:-1;
	}
	else {
	//se il segno è uguale bisogna controllare quale è il maggiore;
	//se la differenza tra gli esponenti è maggiore di 64 è inutile andare a controllare la base
		diff=a.strutturato.exp - b.strutturato.exp;
		if ((diff>64)|(diff<-64)){
			res=(diff>0)?1:-1;
		}
		else {
		//qui bisogna mettere i due esponenti allo stesso grado e shiftare la base e poi controllare
			if (diff>0){
			//qui il primo rimane uguale; il secondo viene shiftato a sinistra e l'exp aumentato
				while (a.strutturato.exp>b.strutturato.exp){
					b.strutturato.exp++;
					b.strutturato.number>>=1;
				}
				if ((a.strutturato.number>b.strutturato.number)&&(a.strutturato.number!=b.strutturato.number)){
					res=1;
				}
				else {
					res=-1;
				if (a.strutturato.number==b.strutturato.number) res=0;
				}
			}
			else {
			//qui il secondo rimane uguale; il primo viene shiftato a sinistra e l'exp aumentato
				while (a.strutturato.exp<b.strutturato.exp){
					a.strutturato.exp++;
					a.strutturato.number>>=1;
				}
				if ((a.strutturato.number>b.strutturato.number)&&(a.strutturato.number!=b.strutturato.number)){
					res=1;
				}
				else {
					res=-1;
				if (a.strutturato.number==b.strutturato.number) res=0;
				}
			}
		}
	}
	switch (res) {
		case -1 : registri->set_sr(registri->get_sr() & ~(fpu_const::C0|fpu_const::C2|fpu_const::C3) | fpu_const::C0);break;
		case  0 : registri->set_sr(registri->get_sr() & ~(fpu_const::C0|fpu_const::C2|fpu_const::C3) | fpu_const::C3);break;
		case  1 : registri->set_sr(registri->get_sr() & ~(fpu_const::C0|fpu_const::C2|fpu_const::C3));break;
		case  2 : registri->set_sr(registri->get_sr() | fpu_const::C0 | fpu_const::C2 | fpu_const::C3);break;
	}
	return;
}

//funzioni che lavorano a 128 bit
int128 myfloat::somma128(int128 uno,int128 due){
	unsigned int vaa1, vab1, vba1, vbb1;
	unsigned int vaa2, vab2, vba2, vbb2;
	unsigned long long s1, s2, s3, s4;
	int128 ret;
	
	vaa1 = uno.alto >> 32;
	vba1 = uno.alto & 0xFFFFFFFF;
	vab1 = uno.basso >> 32;
	vbb1 = uno.basso & 0xFFFFFFFF;

	vaa2 = due.alto >> 32;
	vba2 = due.alto & 0xFFFFFFFF;
	vab2 = due.basso >> 32;
	vbb2 = due.basso & 0xFFFFFFFF;
	
	s1 = vbb1 + (unsigned long long)vbb2;
	s2 = vab1 + (unsigned long long)vab2 + (s1 >> 32);
	s3 = vba1 + (unsigned long long)vba2 + (s2 >> 32);
	s4 = vaa1 + (unsigned long long)vaa2 + (s3 >> 32);
	
	ret.alto = (s4 << 32) + (s3 & 0xFFFFFFFF);
	ret.basso = (s2 << 32) + (s1 & 0xFFFFFFFF);
	return ret;
}

int128 myfloat::sottrai128(int128 uno,int128 due){
	int128 ret;
	ret.alto=0;
	ret.basso=1;
	due.alto=~due.alto;
	due.basso=~due.basso;
	ret = somma128(ret,due);
	ret = somma128(ret,uno);
	return ret;
}

int myfloat::confronta128(int128 uno, int128 due){
	if (uno.alto> due.alto) return 1;
	if (uno.alto< due.alto) return 0;
	if (uno.alto == due.alto){
		if (uno.basso>= due.basso) return 1;
		else return 0;
	}
	return -1; //non viene eseguito mai ma evita un warning
}
