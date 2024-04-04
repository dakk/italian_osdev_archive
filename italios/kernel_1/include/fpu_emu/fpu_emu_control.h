#ifdef __FPU_EMU__

#ifndef __FPU_EMU_CONTROL__
#define __FPU_EMU_CONTROL__

  #include <stddef.h>
  #include <fpu_const.h>
  #include <fpu_emu_reg.h>
  #include <fpu_emu_exception.h>
  
	class fpu_emu_control{
	  private:
	    fpu_emu_reg * registri;
	    fpu_emu_exception * exce;
	  public:
	    void finit(void);//da fare il controllo delle eccezioni pendenti
	    void fninit(void);//fatta
	    void fldcw(unsigned short int);//fatta
	    void fstcw(unsigned short int *);//da fare il controllo delle eccezioni pendenti
	    void fnstcw(unsigned short int *);//fatta
	    void fstsw(unsigned short int *);//da fare il controllo delle eccezioni pendenti
	    void fnstsw(unsigned short int *);//fatta
	    void fclex(void);//da fare il controllo delle eccezioni pendenti
	    void fnclex(void);//fatta
	    void fldenv(reg_env *);//fatta
	    void fstenv(reg_env *);//da fare il controllo delle eccezioni pendenti
	    void fnstenv(reg_env *);//fatta
	    void frstor(reg *);//fatta
	    void fsave(reg *);//da fare il controllo delle eccezioni pendenti
	    void fnsave(reg *);//fatta
	    void fdecstp(void);//fatta
	    void fincstp(void);//fatta
	    void ffree(unsigned short int);//fatta
	    void fnop(void);// fatta
	    void fwait(void);//fatta
	    void fpu_emu_control_init(fpu_emu_reg *, fpu_emu_exception *);
	};

#endif //__FPU_EMU_CONTROL__
#endif //__FPU_EMU__
