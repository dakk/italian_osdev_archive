#ifdef __FPU_EMU__

#ifndef __FPU_EMU_DATA__
#define __FPU_EMU_DATA__

  #include <stddef.h>
  #include <fpu_const.h>
  #include <fpu_emu_reg.h>
  #include <fpu_emu_exception.h>
  

	class fpu_emu_data{
	  private:
	    fpu_emu_reg * registri;
	    fpu_emu_exception * exce;
	  public:
	    void fld(float);//fatta
	    void fld(double);//fatta
	    void fld(long double);//fatta
	    void fst(long double *);//fatta
	    void fst(unsigned int);
	    void fstp(long double *);//fatta
	    void fstp(unsigned int);
	    void fxch(void);//fatta
	    void fxch(unsigned short int);//fatta
	    void fild(short int);
	    void fild(int);
	    void fild(long long);
	    void fist();
	    void fistp();
	    void fbld();
	    void fbstp();
	    void fpu_emu_data_init(fpu_emu_reg *, fpu_emu_exception *);
	};

#endif //__FPU_EMU_DATA__
#endif //__FPU_EMU__
