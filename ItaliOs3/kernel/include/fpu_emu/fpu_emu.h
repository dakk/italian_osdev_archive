#ifndef __FPU_EMU__
#define __FPU_EMU__

  #include <cstdio>
  #include <video.h>
  #include <cstdlib>
  #include <cstring>
  #include <cpu_oper.h>
  #include <fpu_const.h>
  #include <fpu_type.h>
  #include <fpu_emu_reg.h>
  #include <my_double.h>
  #include <fpu_emu_math.h>
  #include <fpu_emu_trig.h>
  #include <fpu_emu_mov.h>
  #include <fpu_emu_comp.h>
  #include <fpu_emu_data.h>
  #include <fpu_emu_control.h>
  #include <fpu_emu_exp.h>
  #include <fpu_emu_exception.h>
  
	class fpu_emu{
	  public:
	    fpu_emu_reg       regi;
	    fpu_emu_exception exce;
	    fpu_emu_math      math;
	    fpu_emu_trig      trig;
	    fpu_emu_mov       mov;
	    fpu_emu_comp      comp;
	    fpu_emu_data      data;
	    fpu_emu_control   control;
	    fpu_emu_exp       exp;
	    fpu_emu();
	};

#endif //__FPU_EMU__
