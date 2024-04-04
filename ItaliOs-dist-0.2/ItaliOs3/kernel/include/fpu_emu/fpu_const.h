#ifndef __FPU_CONST__
#define __FPU_CONST__

#include <stddef.h>

	class fpu_const {
	  public:
	    //costanti per la verifica e la mascherazione
	    //dei flag delle eccezzioni
	    static const word IF  = 0x0001;
	    static const word DF  = 0x0002;
	    static const word ZF  = 0x0004;
	    static const word OF  = 0x0008;
	    static const word UF  = 0x0010;
	    static const word PF  = 0x0020;
	    static const word CLEX = 0x7F00;
	    //altre costanti di SR
	    static const word STF = 0x0040;
	    static const word ESF = 0x0080;
	    static const word CODE = 0x4700;
	    static const word C0 = 0x0100;
	    static const word C1 = 0x0200;
	    static const word C2 = 0x0400;
	    static const word C3 = 0x4000;
	    static const word CO23 = 0x4500;
	    //costanti per il byte alto di CR
	      //costanti per la precisione
	    static const word PC = 0x0300;
	    static const word PC_24 = 0x0000;
	    static const word PC_RES = 0x0100;
	    static const word PC_53 = 0x0200;
	    static const word PC_64 = 0x0300;
	      //costanti per l'arrotondamento
	    static const word RC = 0x0C00;
	    static const word RC_NEAR = 0x0000;
	    static const word RC_DOWN = 0x0400;
	    static const word RC_UP = 0x0800;
	    static const word RC_ZERO = 0x0C00;
	    //costanti per il controllo sul TR
	    //vanno shiftate di 2*(registro)
	    static const word TR_VAL = 0x0000;
	    static const word TR_ZERO = 0x0001;
	    static const word TR_SPEC = 0x0002;
	    static const word TR_EMPTY = 0x0003;
	    //costanti per l'inizializzazione della FPU
	    static const word INIT_TR = 0xFFFF;
	    static const word INIT_CR = 0x037F;
	    static const word INIT_SR = 0x3800;
	    //costanti per il BIAS dell'esponente
	    static const word BIAS_32 = 127;
	    static const word BIAS_64 = 1023;
	    static const word BIAS_80 = 16383;
	    
	    //costanti per i flag del registro eflags
	    static const dword CPU_CF = 0x00000001;
	    static const dword CPU_PF = 0x00000004;
	    static const dword CPU_ZF = 0x00000080;
	};

#endif //__FPU_CONST__
