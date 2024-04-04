#ifdef __FPU_EMU__

#ifndef __FPU_EMU_COMP__
#define __FPU_EMU_COMP__

  #include <stddef.h>
  #include <fpu_const.h>
  #include <fpu_emu_reg.h>
  #include <fpu_emu_exception.h>
  

	class fpu_emu_comp{
	  private:
	    fpu_emu_reg * registri;
	    fpu_emu_exception * exce;
	  public:
	    void fcom(void);//confronta st(0) con st(1)
	    void fcom(unsigned int);//confronta st(0) con st(i)
	    void fcom(float);//confronta st(0) con il parametro a 32 bit
	    void fcom(double);//confronta st(0) con il parametro a 64 bit
	    void fcomp(void);//confronta st(0) con st(1) e fa una pop
	    void fcomp(unsigned int);//confronta st(0) con st(i) e fa una pop
	    void fcomp(float);//confronta st(0) con il parametro a 32 bit e fa una pop
	    void fcomp(double);//confronta st(0) con il parametro a 64 bit e fa una pop
	    void fcompp(void);//confronta st(0) con st(1) e fa due pop
	    void fucom();
	    void fucomp();
	    void fucompp();
	    void ficom();
	    void ficomp();
	    void fcomi();
	    void fcomip();
	    void fucomi();
	    void fucomip();
	    void ftst(void);//conftronta il valore di st(0) con 0.0
	    void fxam(void);//classifica il valore di st(0)
	    void fpu_emu_comp_init(fpu_emu_reg *, fpu_emu_exception *);
	};

#endif //__FPU_EMU_COMP__
#endif //__FPU_EMU__
