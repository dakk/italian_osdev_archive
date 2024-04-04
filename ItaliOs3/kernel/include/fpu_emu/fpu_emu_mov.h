#ifdef __FPU_EMU__

#ifndef __FPU_EMU_MOV__
#define __FPU_EMU_MOV__

  #include <stddef.h>
  #include <fpu_const.h>
  #include <fpu_emu_reg.h>
  #include <fpu_emu_exception.h>

	class fpu_emu_mov{
	  private:
	    fpu_emu_reg * registri;
	    fpu_emu_exception * exce;
	  public:
	    void fcmovb(unsigned int);//fatta
	    void fcmovnb(unsigned int);//fatta
	    void fcmove(unsigned int);//fatta
	    void fcmovne(unsigned int);//fatta
	    void fcmovbe(unsigned int);//fatta
	    void fcmovnbe(unsigned int);//fatta
	    void fcmovu(unsigned int);//fatta
	    void fcmovnu(unsigned int);//fatta
	    void fpu_emu_mov_init(fpu_emu_reg *, fpu_emu_exception *);
	};

#endif //__FPU_EMU_MOV__
#endif //__FPU_EMU__
