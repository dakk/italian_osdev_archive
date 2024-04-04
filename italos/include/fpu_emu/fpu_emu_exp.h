#ifdef __FPU_EMU__

#ifndef __FPU_EMU_EXP__
#define __FPU_EMU_EXP__

  #include <stddef.h>
  #include <fpu_const.h>
  #include <fpu_emu_reg.h>
  #include <fpu_emu_exception.h>
  

	class fpu_emu_exp{
	  private:
	    fpu_emu_reg * registri;
	    fpu_emu_exception * exce;
	  public:
	    void fyl2x();
	    void fyl2xp1();
	    void f2xm1();
	    void fscale();
	    void fpu_emu_exp_init(fpu_emu_reg *, fpu_emu_exception *);
	};

#endif //__FPU_EMU_EXP__
#endif //__FPU_EMU__
