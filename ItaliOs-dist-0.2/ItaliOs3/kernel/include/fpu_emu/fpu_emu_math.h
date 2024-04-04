#ifdef __FPU_EMU__

#ifndef __FPU_EMU_MATH__
#define __FPU_EMU_MATH__

  #include <stddef.h>
  #include <fpu_const.h>
  #include <fpu_emu_reg.h>
  #include <fpu_emu_exception.h>

	class fpu_emu_math{
	  private:
	    fpu_emu_reg * registri;
	    fpu_emu_exception * exce;
	  public:
	    //istruzioni aritmetiche di base
	    //void fadd(short int, short int);//fatta
	    void fadd(float);
	    void fadd(double);
	    void fadd(short int, short int);//il secondo parametro vale 0 o 1
	    void faddp(void);
	    void faddp(unsigned short int);
	    void fiadd(short int);
	    void fiadd(int);
	    void fsub(float);
	    void fsub(double);
	    void fsub(short int, short int);//il secondo parametro vale 0 o 1
	    void fsubp(void);
	    void fsubp(unsigned short int);
	    void fisub(short int);
	    void fisub(int);
	    void fsubr(float);
	    void fsubr(double);
	    void fsubr(short int, short int);//il secondo parametro vale 0 o 1
	    void fsubrp(void);
	    void fsubrp(unsigned short int);
	    void fisubr(short int);
	    void fisubr(int);
	    void fmul(float);
	    void fmul(double);
	    void fmul(short int, short int);
	    void fmulp(void);
	    void fmulp(short int);
	    void fimul(short int);
	    void fimul(int);//moltiplica st(0) per un intero	    
	    void fdiv(float);
	    void fdiv(double);
	    void fdiv(short int, short int);//il secondo parametro vale 0 o 1
	    void fdivp(void);
	    void fdivp(unsigned short int);
	    void fidiv(short int);
	    void fidiv(int);
	    void fdivr(float);
	    void fdivr(double);
	    void fdivr(short int, short int);//il secondo parametro vale 0 o 1
	    void fdivrp(void);
	    void fdivrp(unsigned short int);
	    void fidivr(short int);
	    void fidivr(int);
	    void fabs(void);//fatta
	    void fchs(void);//fatta
	    void fsqrt();
	    void fprem();
	    void fprem1();
	    void frndint();
	    void fxtract();
	    void fpu_emu_math_init(fpu_emu_reg *, fpu_emu_exception *);
	};
  
#endif //__FPU_EMU_MATH__

#endif //__FPU_EMU__
