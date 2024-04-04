#ifdef __FPU_EMU__
#ifndef __FPU_EMU_EXCEPTION__
#define __FPU_EMU_EXCEPTION__
	#include <cstddef>
	#include <fpu_const.h>

enum exceptions {P=0x01,U=0x02,O=0x4,Z=0x8,D=0x10,IA=0x20,IS=0x40};

typedef struct exception{
	exceptions ex_code;
	unsigned int * address;
	exception * next;
} exception;

	class fpu_emu_exception{
	    private:
		exception * coda;
	    public:
		exception * get_address(); //restituisce il puntatore alla base della coda
		int add(exceptions);//aggiunge un'eccezione in fondo alla coda
				    //ritorna 0 se tutto è andato bene -1 se ci sono stati problemi nella malloc
		int del();//elimina la prima eccezione che si trova nella coda
		exception * get_first();//restituisce l'indirizzo del primo elemento della coda
		exception * get_last();//restituisce l'indirizzo dell'ultimo elemento della coda
		int solve(exceptions);//risolve la prima eccezione del tipo specificato e la elimina dalla coda
		int solve();//risolve la prima eccezione e la elimina;
		fpu_emu_exception();//costruttore
	};

#endif //__FPU_EMU_EXCEPTION__
#endif //__FPU_EMU__
